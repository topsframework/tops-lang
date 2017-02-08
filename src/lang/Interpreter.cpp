/***********************************************************************/
/*  Copyright 2016 ToPS                                                */
/*                                                                     */
/*  This program is free software; you can redistribute it and/or      */
/*  modify it under the terms of the GNU  General Public License as    */
/*  published by the Free Software Foundation; either version 3 of     */
/*  the License, or (at your option) any later version.                */
/*                                                                     */
/*  This program is distributed in the hope that it will be useful,    */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of     */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      */
/*  GNU General Public License for more details.                       */
/*                                                                     */
/*  You should have received a copy of the GNU General Public License  */
/*  along with this program; if not, write to the Free Software        */
/*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,         */
/*  MA 02110-1301, USA.                                                */
/***********************************************************************/

// Interface header
#include "lang/Interpreter.hpp"

// Standard headers
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <typeinfo>
#include <stdexcept>
#include <unordered_map>
#include <iostream>

// Internal headers
#include "lang/Util.hpp"
#include "lang/ModelConfigRegister.hpp"
#include "lang/DependencyTreeParser.hpp"

#include "config/Domain.hpp"
#include "config/BasicConfig.hpp"
#include "config/StringLiteralSuffix.hpp"

#include "config/Options.hpp"

#include "config/definition/ModelConfig.hpp"
#include "config/definition/HMMConfig.hpp"
#include "config/definition/IIDConfig.hpp"
#include "config/definition/IMCConfig.hpp"
#include "config/definition/MDDConfig.hpp"
#include "config/definition/MSMConfig.hpp"
#include "config/definition/GHMMConfig.hpp"
#include "config/definition/SBSWConfig.hpp"
#include "config/definition/VLMCConfig.hpp"
#include "config/definition/LCCRFConfig.hpp"
#include "config/definition/PeriodicIMCConfig.hpp"

#include "config/definition/StateConfig.hpp"

#include "config/definition/DurationConfig.hpp"
#include "config/definition/FixedDurationConfig.hpp"
#include "config/definition/ExplicitDurationConfig.hpp"
#include "config/definition/GeometricDurationConfig.hpp"
#include "config/definition/MaxLengthDurationConfig.hpp"

#include "config/definition/DependencyTreeConfig.hpp"
#include "config/definition/FeatureFunctionLibraryConfig.hpp"

// External headers
#include "chaiscript/language/chaiscript_engine.hpp"

#include "chaiscript/dispatchkit/any.hpp"
#include "chaiscript/dispatchkit/type_info.hpp"
#include "chaiscript/dispatchkit/boxed_cast.hpp"
#include "chaiscript/dispatchkit/boxed_value.hpp"
#include "chaiscript/dispatchkit/bootstrap_stl.hpp"
#include "chaiscript/dispatchkit/type_conversions.hpp"
#include "chaiscript/dispatchkit/register_function.hpp"

// Using declarations
using config::operator ""_t;

// Namespace aliases
namespace { namespace co = config::option; }
namespace { namespace cod = co::definition; }

namespace lang {

/*----------------------------------------------------------------------------*/
/*                               LOCAL STRUCTS                                */
/*----------------------------------------------------------------------------*/

template<typename T>
struct get_inner {
  using type = std::remove_cv_t<T>;
};

template<typename T>
struct get_inner<std::unique_ptr<T>> {
  using type = typename std::unique_ptr<T>::element_type;
};

template<typename T>
struct get_inner<std::shared_ptr<T>> {
  using type = typename std::shared_ptr<T>::element_type;
};

template<typename T>
using get_inner_t = typename get_inner<T>::type;

/*----------------------------------------------------------------------------*/
/*                                LOCAL MACROS                                */
/*----------------------------------------------------------------------------*/

#define REGISTER_TYPE(type, name) \
  do { \
    using chaiscript::bootstrap::standard_library::assignable_type; \
    using registered_type = get_inner_t<co::type>; \
    module->add(chaiscript::user_type<registered_type>(), name); \
    assignable_type<registered_type>(name, module); \
  } while (false)

#define REGISTER_VECTOR(type, name) \
  do { \
    using chaiscript::vector_conversion; \
    using chaiscript::bootstrap::standard_library::vector_type; \
    using registered_type = get_inner_t<co::type>; \
    module->add(vector_type<registered_type>(name)); \
    module->add(vector_conversion<registered_type>()); \
  } while (false)

#define REGISTER_MAP(type, name) \
  do { \
    using chaiscript::map_conversion; \
    using chaiscript::bootstrap::standard_library::map_type; \
    using registered_type = get_inner_t<co::type>; \
    module->add(map_type<registered_type>(name)); \
    module->add(map_conversion<registered_type>()); \
  } while (false)

#define REGISTER_COMMON_TYPE(type) \
  REGISTER_TYPE(type, #type)
#define REGISTER_DEFINITION_TYPE(type) \
  REGISTER_TYPE(definition::type, #type "Definition")

#define REGISTER_COMMON_VECTOR(type) \
  REGISTER_VECTOR(type, #type)
#define REGISTER_DEFINITION_VECTOR(type) \
  REGISTER_VECTOR(definition::type, #type "Definition")

#define REGISTER_COMMON_MAP(type) \
  REGISTER_MAP(type, #type)
#define REGISTER_DEFINITION_MAP(type) \
  REGISTER_MAP(definition::type, #type "Definition")

/*----------------------------------------------------------------------------*/
/*                              STATIC VARIABLES                              */
/*----------------------------------------------------------------------------*/

const std::unordered_map<std::string, Interpreter::ModelType>
Interpreter::model_type_map {
  { "GHMM"        , Interpreter::ModelType::GHMM         },
  { "HMM"         , Interpreter::ModelType::HMM          },
  { "LCCRF"       , Interpreter::ModelType::LCCRF        },
  { "IID"         , Interpreter::ModelType::IID          },
  { "VLMC"        , Interpreter::ModelType::VLMC         },
  { "IMC"         , Interpreter::ModelType::IMC          },
  { "PeriodicIMC" , Interpreter::ModelType::PeriodicIMC  },
  { "SBSW"        , Interpreter::ModelType::SBSW         },
  { "MSM"         , Interpreter::ModelType::MSM          },
  { "MDD"         , Interpreter::ModelType::MDD          }
};

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

cod::Model Interpreter::evalModel(const std::string &filepath) {
  checkExtension(filepath);
  return makeModelConfig(filepath);
}

/*----------------------------------------------------------------------------*/

void Interpreter::checkExtension(const std::string &filepath) {
  auto suffix = extractSuffix(filepath);

  if (suffix != "tops") {
    std::ostringstream error_message;
    error_message << "Unknown extension ." << suffix;
    throw std::invalid_argument(error_message.str());
  }
}

/*----------------------------------------------------------------------------*/

cod::Model Interpreter::makeModelConfig(const std::string &filepath) {
  auto model_type = findModelType(filepath);

  switch (model_type) {
    using namespace config::definition;  // NOLINT(build/namespaces)
    case ModelType::GHMM:        return fillConfig<GHMMConfig>(filepath);
    case ModelType::HMM:         return fillConfig<HMMConfig>(filepath);
    case ModelType::LCCRF:       return fillConfig<LCCRFConfig>(filepath);
    case ModelType::IID:         return fillConfig<IIDConfig>(filepath);
    case ModelType::VLMC:        return fillConfig<VLMCConfig>(filepath);
    case ModelType::IMC:         return fillConfig<IMCConfig>(filepath);
    case ModelType::PeriodicIMC: return fillConfig<PeriodicIMCConfig>(filepath);
    case ModelType::SBSW:        return fillConfig<SBSWConfig>(filepath);
    case ModelType::MSM:         return fillConfig<MSMConfig>(filepath);
    case ModelType::MDD:         return fillConfig<MDDConfig>(filepath);
  }
}

/*----------------------------------------------------------------------------*/

Interpreter::ModelType Interpreter::findModelType(const std::string &filepath) {
  auto root_dir = extractDir(filepath);

  std::vector<std::string> modulepaths;
  std::vector<std::string> usepaths { root_dir };

  chaiscript::ChaiScript chai(modulepaths, usepaths);
  chai.add(makeInterpreterLibrary(filepath));

  auto cfg = config::definition::ModelConfig::make(filepath);
  cfg->accept(ModelConfigRegister(chai));

  try {
    chai.eval_file(filepath);
  } catch (const std::exception &e) {
    // Explicitly ignore missing object exceptions
    if (!missingObjectException(e)) throw;
  }

  auto model_name = std::get<decltype("model_type"_t)>(*cfg.get());

  try {
    return model_type_map.at(model_name);
  } catch (const std::out_of_range &e) {
    throw std::logic_error(
        filepath + ": Model type "
        + (model_name.empty() ? "not specified!" : "unknown: ")
        + model_name);
  }
}

/*----------------------------------------------------------------------------*/

bool Interpreter::missingObjectException(const std::exception &e) {
  std::string exception(e.what());
  return exception.find("Can not find object:") != std::string::npos;
}

/*----------------------------------------------------------------------------*/

chaiscript::ModulePtr
Interpreter::makeInterpreterLibrary(const std::string &filepath) {
  static auto interpreter_library = std::make_shared<chaiscript::Module>();
  static bool initialized = false;

  if (!initialized) {
    registerTypes(interpreter_library, filepath);
    registerHelpers(interpreter_library, filepath);
    registerConstants(interpreter_library, filepath);
    registerAttributions(interpreter_library, filepath);
    registerConcatenations(interpreter_library, filepath);
    initialized = true;
  }

  return interpreter_library;
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerTypes(chaiscript::ModulePtr &module,
                                const std::string &/* filepath */) {
  // Ordinary types
  REGISTER_COMMON_TYPE(Size);
  REGISTER_COMMON_TYPE(Type);
  REGISTER_COMMON_TYPE(Domain);
  REGISTER_COMMON_TYPE(Domains);
  REGISTER_COMMON_TYPE(Alphabet);
  REGISTER_COMMON_TYPE(Alphabets);
  REGISTER_COMMON_TYPE(Probability);
  REGISTER_COMMON_TYPE(Probabilities);
  REGISTER_COMMON_TYPE(FeatureFunction);
  REGISTER_COMMON_TYPE(FeatureFunctions);

  REGISTER_COMMON_VECTOR(Domains);
  REGISTER_COMMON_VECTOR(Alphabet);
  REGISTER_COMMON_VECTOR(Alphabets);

  REGISTER_COMMON_MAP(Probabilities);
  REGISTER_COMMON_MAP(FeatureFunctions);

  // Definitions
  REGISTER_DEFINITION_TYPE(Model);
  REGISTER_DEFINITION_TYPE(Models);
  REGISTER_DEFINITION_TYPE(State);
  REGISTER_DEFINITION_TYPE(States);
  REGISTER_DEFINITION_TYPE(Duration);
  REGISTER_DEFINITION_TYPE(DependencyTree);
  REGISTER_DEFINITION_TYPE(DependencyTrees);
  REGISTER_DEFINITION_TYPE(FeatureFunctionLibraries);

  REGISTER_DEFINITION_VECTOR(Models);
  REGISTER_DEFINITION_VECTOR(DependencyTrees);
  REGISTER_DEFINITION_VECTOR(FeatureFunctionLibraries);

  REGISTER_DEFINITION_MAP(States);
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerHelpers(chaiscript::ModulePtr &module,
                                  const std::string &filepath) {
  using chaiscript::fun;

  using config::definition::FixedDurationConfig;
  using config::definition::ExplicitDurationConfig;
  using config::definition::GeometricDurationConfig;
  using config::definition::MaxLengthDurationConfig;
  using config::definition::FeatureFunctionLibraryConfig;
  using config::definition::DependencyTreeConfig;

  module->add(fun([this, filepath] (const std::string &file) {
    auto root_dir = extractDir(filepath);
    return this->makeModelConfig(root_dir + file);
  }), "model");

  module->add(fun([this, filepath]() {
    auto duration = GeometricDurationConfig::make(filepath, "geometric");
    return cod::Duration(duration);
  }), "geometric");

  module->add(fun([this, filepath] (const std::string &file) {
    auto duration = ExplicitDurationConfig::make(filepath, "explicit");
    std::get<decltype("model"_t)>(*duration.get())
      = this->makeModelConfig(extractDir(filepath) + file);
    return cod::Duration(duration);
  }), "explicit");

  module->add(fun([this, filepath] (const std::string &file,
                                    unsigned int size) {
    auto duration = ExplicitDurationConfig::make(filepath, "explicit");
    std::get<decltype("max_size"_t)>(*duration.get()) = size;
    std::get<decltype("model"_t)>(*duration.get())
      = this->makeModelConfig(extractDir(filepath) + file);
    return cod::Duration(duration);
  }), "explicit");

  module->add(fun([this, filepath] (cod::Model model,
                                    unsigned int size) {
    auto duration = ExplicitDurationConfig::make(filepath, "explicit");
    std::get<decltype("max_size"_t)>(*duration.get()) = size;
    std::get<decltype("model"_t)>(*duration.get()) = model;
    return cod::Duration(duration);
  }), "explicit");

  module->add(fun([this, filepath] (unsigned int size) {
    auto duration = FixedDurationConfig::make(filepath, "fixed");
    std::get<decltype("size"_t)>(*duration.get()) = size;
    return cod::Duration(duration);
  }), "fixed");

  module->add(fun([this, filepath] (unsigned int size) {
    auto duration = MaxLengthDurationConfig::make(filepath, "max_length");
    std::get<decltype("size"_t)>(*duration.get()) = size;
    return cod::Duration(duration);
  }), "max_length");

  module->add(fun([this, filepath] (const std::string &file) {
    auto root_dir = extractDir(filepath);
    return this->fillConfig<FeatureFunctionLibraryConfig>(root_dir + file);
  }), "lib");

  module->add(fun([this, filepath] (const std::string &file) {
    auto root_dir = extractDir(filepath);

    std::ifstream src(root_dir + file);

    std::string line;
    std::vector<std::string> content;
    while (std::getline(src, line)) {
      content.push_back(line);
    }

    DependencyTreeParser parser(this, root_dir, file, content);
    return parser.parse();
  }), "tree");

  module->add(fun([this] (const co::Alphabet &alphabet) {
    return std::make_shared<config::Domain>(
        typename config::Domain::discrete_domain{}, alphabet);
  }), "discrete_domain");

  module->add(fun([this] (const co::OutToInSymbolFunction &o2i,
                          const co::InToOutSymbolFunction &i2o) {
    return std::make_shared<config::Domain>(
        typename config::Domain::custom_domain{}, o2i, i2o);
  }), "custom_domain");
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerConstants(chaiscript::ModulePtr &module,
                                    const std::string &/* filepath */) {
  using chaiscript::const_var;

  module->add_global_const(const_var(std::string("emission")), "emission");
  module->add_global_const(const_var(std::string("duration")), "duration");
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerAttributions(chaiscript::ModulePtr &module,
                                       const std::string &filepath) {
  using chaiscript::fun;
  using chaiscript::boxed_cast;

  using chaiscript::Boxed_Value;
  using Vector = std::vector<Boxed_Value>;
  using Map = std::map<std::string, Boxed_Value>;

  module->add(fun([] (config::Domain &conv, const Vector &orig) {
    co::Alphabet alphabet;
    for (auto &element : orig)
      alphabet.push_back(boxed_cast<co::Symbol>(element));

    conv = config::Domain(typename config::Domain::discrete_domain{}, alphabet);
  }), "=");

  module->add(fun([] (co::Alphabets &conv, const Vector &orig) {
    for (auto &element : orig) {
      auto inner_orig = boxed_cast<Vector &>(element);
      co::Alphabet inner_conv;

      for (auto &inner_element : inner_orig)
        inner_conv.push_back(boxed_cast<co::Symbol>(inner_element));

      conv.push_back(inner_conv);
    }
  }), "=");

  module->add(fun([filepath] (cod::States &conv, const Map &orig) {
    for (auto &pair : orig) {
      auto inner_orig = boxed_cast<Map &>(pair.second);

      conv[pair.first] = config::definition::StateConfig::make(filepath);

      std::get<decltype("duration"_t)>(*conv[pair.first])
        = boxed_cast<cod::Duration>(inner_orig["duration"]);

      std::get<decltype("emission"_t)>(*conv[pair.first])
        = boxed_cast<cod::Model>(inner_orig["emission"]);
    }
  }), "=");
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerConcatenations(chaiscript::ModulePtr &module,
                                         const std::string &/* filepath */) {
  using chaiscript::fun;

  module->add(fun([] (const std::string &lhs, const std::string &rhs) {
    return lhs + " | " + rhs;
  }), "|");

  module->add(fun([] (const std::string &lhs, const std::string &rhs) {
    return rhs + " | " + lhs;
  }), "->");
}

/*----------------------------------------------------------------------------*/

}  // namespace lang
