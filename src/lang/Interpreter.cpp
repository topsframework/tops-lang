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

#include "config/StringLiteralSuffix.hpp"

#include "config/Domain.hpp"

#include "config/Options.hpp"
#include "config/BasicConfig.hpp"

#include "config/ModelConfig.hpp"
#include "config/HMMConfig.hpp"
#include "config/IIDConfig.hpp"
#include "config/IMCConfig.hpp"
#include "config/MDDConfig.hpp"
#include "config/MSMConfig.hpp"
#include "config/GHMMConfig.hpp"
#include "config/SBSWConfig.hpp"
#include "config/VLMCConfig.hpp"
#include "config/LCCRFConfig.hpp"
#include "config/PeriodicIMCConfig.hpp"

#include "config/StateConfig.hpp"

#include "config/DurationConfig.hpp"
#include "config/FixedDurationConfig.hpp"
#include "config/ExplicitDurationConfig.hpp"
#include "config/GeometricDurationConfig.hpp"
#include "config/MaxLengthDurationConfig.hpp"

#include "config/DependencyTreeConfig.hpp"
#include "config/FeatureFunctionLibraryConfig.hpp"

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

#define REGISTER_TYPE(type) \
  do { \
    using chaiscript::bootstrap::standard_library::assignable_type; \
    using registered_type = get_inner_t<config::option::type>; \
    module->add(chaiscript::user_type<registered_type>(), #type); \
    assignable_type<registered_type>(#type, module); \
  } while (false)

#define REGISTER_VECTOR(type) \
  do { \
    using chaiscript::vector_conversion; \
    using chaiscript::bootstrap::standard_library::vector_type; \
    using registered_type = get_inner_t<config::option::type>; \
    module->add(vector_type<registered_type>(#type)); \
    module->add(vector_conversion<registered_type>()); \
  } while (false)

#define REGISTER_MAP(type) \
  do { \
    using chaiscript::map_conversion; \
    using chaiscript::bootstrap::standard_library::map_type; \
    using registered_type = get_inner_t<config::option::type>; \
    module->add(map_type<registered_type>(#type)); \
    module->add(map_conversion<registered_type>()); \
  } while (false)

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

config::ModelConfigPtr Interpreter::evalModel(const std::string &filepath) {
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

config::ModelConfigPtr
Interpreter::makeModelConfig(const std::string &filepath) {
  auto model_type = findModelType(filepath);

  switch (model_type) {
    using namespace config;  // NOLINT(build/namespaces)
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

  auto cfg = std::make_shared<config::ModelConfig>(filepath);
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
  REGISTER_TYPE(Type);
  REGISTER_TYPE(Alphabet);
  REGISTER_TYPE(Alphabets);
  REGISTER_TYPE(Size);
  REGISTER_TYPE(Probabilities);
  REGISTER_TYPE(Domain);
  REGISTER_TYPE(Domains);
  REGISTER_TYPE(Duration);
  REGISTER_TYPE(Model);
  REGISTER_TYPE(Models);
  REGISTER_TYPE(State);
  REGISTER_TYPE(States);
  REGISTER_TYPE(DependencyTree);
  REGISTER_TYPE(DependencyTrees);
  REGISTER_TYPE(FeatureFunctions);
  REGISTER_TYPE(FeatureFunctionLibraries);

  REGISTER_VECTOR(Alphabet);
  REGISTER_VECTOR(Alphabets);
  REGISTER_VECTOR(Domains);
  REGISTER_VECTOR(Models);
  REGISTER_VECTOR(DependencyTrees);
  REGISTER_VECTOR(FeatureFunctionLibraries);

  REGISTER_MAP(Probabilities);
  REGISTER_MAP(States);
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerHelpers(chaiscript::ModulePtr &module,
                                  const std::string &filepath) {
  using chaiscript::fun;

  using config::FixedDurationConfig;
  using config::ExplicitDurationConfig;
  using config::GeometricDurationConfig;
  using config::MaxLengthDurationConfig;
  using config::FeatureFunctionLibraryConfig;
  using config::DependencyTreeConfig;

  module->add(fun([this, filepath] (const std::string &file) {
    auto root_dir = extractDir(filepath);
    return this->makeModelConfig(root_dir + file);
  }), "model");

  module->add(fun([this, filepath]() {
    auto duration_ptr = GeometricDurationConfig::make(filepath, "geometric");
    return config::DurationConfigPtr(duration_ptr);
  }), "geometric");

  module->add(fun([this, filepath] (const std::string &file) {
    auto duration_ptr = ExplicitDurationConfig::make(filepath, "explicit");
    std::get<decltype("model"_t)>(*duration_ptr.get())
      = this->makeModelConfig(extractDir(filepath) + file);
    return config::DurationConfigPtr(duration_ptr);
  }), "explicit");

  module->add(fun([this, filepath] (const std::string &file,
                                    unsigned int size) {
    auto duration_ptr = ExplicitDurationConfig::make(filepath, "explicit");
    std::get<decltype("max_size"_t)>(*duration_ptr.get()) = size;
    std::get<decltype("model"_t)>(*duration_ptr.get())
      = this->makeModelConfig(extractDir(filepath) + file);
    return config::DurationConfigPtr(duration_ptr);
  }), "explicit");

  module->add(fun([this, filepath] (config::ModelConfigPtr model_ptr,
                                    unsigned int size) {
    auto duration_ptr = ExplicitDurationConfig::make(filepath, "explicit");
    std::get<decltype("max_size"_t)>(*duration_ptr.get()) = size;
    std::get<decltype("model"_t)>(*duration_ptr.get()) = model_ptr;
    return config::DurationConfigPtr(duration_ptr);
  }), "explicit");

  module->add(fun([this, filepath] (unsigned int size) {
    auto duration_ptr = FixedDurationConfig::make(filepath, "fixed");
    std::get<decltype("size"_t)>(*duration_ptr.get()) = size;
    return config::DurationConfigPtr(duration_ptr);
  }), "fixed");

  module->add(fun([this, filepath] (unsigned int size) {
    auto duration_ptr = MaxLengthDurationConfig::make(filepath, "max_length");
    std::get<decltype("size"_t)>(*duration_ptr.get()) = size;
    return config::DurationConfigPtr(duration_ptr);
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

  module->add(fun([this] (const config::option::Alphabet &alphabet) {
    return std::make_shared<config::Domain>(
        typename config::Domain::discrete_domain{}, alphabet);
  }), "discrete_domain");

  module->add(fun([this] (const config::option::OutToInSymbolFunction &o2i,
                          const config::option::InToOutSymbolFunction &i2o) {
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
    config::option::Alphabet alphabet;
    for (auto &element : orig)
      alphabet.push_back(boxed_cast<config::option::Symbol>(element));

    conv = config::Domain(typename config::Domain::discrete_domain{}, alphabet);
  }), "=");

  module->add(fun([] (config::option::Alphabets &conv, const Vector &orig) {
    for (auto &element : orig) {
      auto inner_orig = boxed_cast<Vector &>(element);
      config::option::Alphabet inner_conv;

      for (auto &inner_element : inner_orig)
        inner_conv.push_back(boxed_cast<config::option::Symbol>(inner_element));

      conv.push_back(inner_conv);
    }
  }), "=");

  module->add(fun([filepath] (config::option::States &conv, const Map &orig) {
    for (auto &pair : orig) {
      auto inner_orig = boxed_cast<Map &>(pair.second);

      conv[pair.first] = std::make_shared<config::StateConfig>(filepath);

      std::get<decltype("duration"_t)>(*conv[pair.first])
        = boxed_cast<config::DurationConfigPtr>(inner_orig["duration"]);

      std::get<decltype("emission"_t)>(*conv[pair.first])
        = boxed_cast<config::ModelConfigPtr>(inner_orig["emission"]);
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
