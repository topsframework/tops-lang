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
#include "config/StringLiteralSuffix.hpp"

#include "config/Options.hpp"

#include "config/training/ModelConfig.hpp"
#include "config/training/UntrainedModelConfig.hpp"
#include "config/training/PretrainedModelConfig.hpp"
#include "config/training/HMMConfig.hpp"
#include "config/training/IIDConfig.hpp"
#include "config/training/MDDConfig.hpp"
#include "config/training/GHMMConfig.hpp"
#include "config/training/VLMCConfig.hpp"
#include "config/training/PeriodicIMCConfig.hpp"

#include "config/training/StateConfig.hpp"

#include "config/training/DurationConfig.hpp"
#include "config/training/FixedDurationConfig.hpp"
#include "config/training/ExplicitDurationConfig.hpp"
#include "config/training/GeometricDurationConfig.hpp"
#include "config/training/MaxLengthDurationConfig.hpp"

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
namespace { namespace ct = config::training; }
namespace { namespace cd = config::definition; }
namespace { namespace cot = co::training; }
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
    using registered_type = get_inner_t<type>; \
    module->add(chaiscript::user_type<registered_type>(), name); \
    assignable_type<registered_type>(name, module); \
  } while (false)

#define REGISTER_VECTOR(type, name) \
  do { \
    using chaiscript::vector_conversion; \
    using chaiscript::bootstrap::standard_library::vector_type; \
    using registered_type = get_inner_t<type>; \
    module->add(vector_type<registered_type>(name)); \
    module->add(vector_conversion<registered_type>()); \
  } while (false)

#define REGISTER_MAP(type, name) \
  do { \
    using chaiscript::map_conversion; \
    using chaiscript::bootstrap::standard_library::map_type; \
    using registered_type = get_inner_t<type>; \
    module->add(map_type<registered_type>(name)); \
    module->add(map_conversion<registered_type>()); \
  } while (false)

#define REGISTER_COMMON_TYPE(type) \
  REGISTER_TYPE(co::type, #type)
#define REGISTER_TRAINING_TYPE(type) \
  REGISTER_TYPE(cot::type, #type "Training")
#define REGISTER_DEFINITION_TYPE(type) \
  REGISTER_TYPE(cod::type, #type "Definition")

#define REGISTER_COMMON_VECTOR(type) \
  REGISTER_COMMON_TYPE(type); REGISTER_VECTOR(co::type, #type)
#define REGISTER_TRAINING_VECTOR(type) \
  REGISTER_TRAINING_TYPE(type); REGISTER_VECTOR(cot::type, #type "Training")
#define REGISTER_DEFINITION_VECTOR(type) \
  REGISTER_DEFINITION_TYPE(type); REGISTER_VECTOR(cod::type, #type "Definition")

#define REGISTER_COMMON_MAP(type) \
  REGISTER_COMMON_TYPE(type); REGISTER_MAP(co::type, #type)
#define REGISTER_TRAINING_MAP(type) \
  REGISTER_TRAINING_TYPE(type); REGISTER_MAP(cot::type, #type "Training")
#define REGISTER_DEFINITION_MAP(type) \
  REGISTER_DEFINITION_TYPE(type); REGISTER_MAP(cod::type, #type "Definition")

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

cot::Model Interpreter::evalModelTraining(const std::string &filepath) {
  checkExtension(filepath);
  return makeModelTrainingConfig(filepath);
}

/*----------------------------------------------------------------------------*/

cod::Model Interpreter::evalModelDefinition(const std::string &filepath) {
  checkExtension(filepath);
  return makeModelDefinitionConfig(filepath);
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

cot::Model Interpreter::makeModelTrainingConfig(const std::string &filepath) {
  const std::string untrained_model = "untrained_model";
  const std::string pretrained_model = "pretrained_model";

  auto category = getConfigOption<
    ct::ModelConfig, decltype("category"_t)>(filepath);

  if (category == "PretrainedModel")
    return fillConfig<ct::PretrainedModelConfig>(filepath, pretrained_model);

  auto model_type = getConfigOption<
    ct::UntrainedModelConfig, decltype("model_type"_t)>(filepath);
  auto training_algorithm = getConfigOption<
    ct::UntrainedModelConfig, decltype("training_algorithm"_t)>(filepath);

  if (model_type == "GHMM") {
    if (training_algorithm == "MaximumLikehood") {
      return fillConfig<ct::GHMM::MaximumLikehoodConfig>(
          filepath, untrained_model);
    } else {
      handleWrongStringOption(filepath, "training_algorithm",
                                         training_algorithm);
    }
  } else if (model_type == "HMM") {
    if (training_algorithm == "MaximumLikehood") {
      return fillConfig<ct::HMM::MaximumLikehoodConfig>(
          filepath, untrained_model);
    } else if (training_algorithm == "BaumWelch") {
      return fillConfig<ct::HMM::BaumWelchConfig>(
          filepath, untrained_model);
    } else {
      handleWrongStringOption(filepath, "training_algorithm",
                                         training_algorithm);
    }
  } else if (model_type == "IID") {
    if (training_algorithm == "MaximumLikehood") {
      return fillConfig<ct::IID::MaximumLikehoodConfig>(
          filepath, untrained_model);
    } else if (training_algorithm == "SmoothedHistogramBurge") {
      return fillConfig<ct::IID::SmoothedHistogramBurgeConfig>(
          filepath, untrained_model);
    } else if (training_algorithm == "SmoothedHistogramStanke") {
      return fillConfig<ct::IID::SmoothedHistogramStankeConfig>(
          filepath, untrained_model);
    } else {
      handleWrongStringOption(filepath, "training_algorithm",
                                         training_algorithm);
    }
  } else if (model_type == "MDD") {
    if (training_algorithm == "Standard") {
      return fillConfig<ct::MDD::StandardConfig>(
          filepath, untrained_model);
    } else {
      handleWrongStringOption(filepath, "training_algorithm",
                                         training_algorithm);
    }
  } else if (model_type == "PeriodicIMC") {
    if (training_algorithm == "Interpolation") {
      return fillConfig<ct::PeriodicIMC::InterpolationConfig>(
          filepath, untrained_model);
    } else {
      handleWrongStringOption(filepath, "training_algorithm",
                                         training_algorithm);
    }
  } else if (model_type == "VLMC") {
    if (training_algorithm == "Context") {
      return fillConfig<ct::VLMC::ContextConfig>(
          filepath, untrained_model);
    } else if (training_algorithm == "FixedLength") {
      return fillConfig<ct::VLMC::FixedLengthConfig>(
          filepath, untrained_model);
    } else if (training_algorithm == "Interpolation") {
      return fillConfig<ct::VLMC::InterpolationConfig>(
          filepath, untrained_model);
    } else {
      handleWrongStringOption(filepath, "training_algorithm",
                                         training_algorithm);
    }
  } else {
    handleWrongStringOption(filepath, "model_type", model_type);
  }

  return nullptr;
}

/*----------------------------------------------------------------------------*/

cod::Model Interpreter::makeModelDefinitionConfig(const std::string &filepath) {
  const std::string model = "model";

  auto model_type = getConfigOption<
    cd::ModelConfig, decltype("model_type"_t)>(filepath);

  if (model_type == "GHMM") {
    return fillConfig<cd::GHMMConfig>(filepath, model);
  } else if (model_type == "HMM") {
    return fillConfig<cd::HMMConfig>(filepath, model);
  } else if (model_type == "LCCRF") {
    return fillConfig<cd::LCCRFConfig>(filepath, model);
  } else if (model_type == "IID") {
    return fillConfig<cd::IIDConfig>(filepath, model);
  } else if (model_type == "VLMC") {
    return fillConfig<cd::VLMCConfig>(filepath, model);
  } else if (model_type == "IMC") {
    return fillConfig<cd::IMCConfig>(filepath, model);
  } else if (model_type == "PeriodicIMC") {
    return fillConfig<cd::PeriodicIMCConfig>(filepath, model);
  } else if (model_type == "SBSW") {
    return fillConfig<cd::SBSWConfig>(filepath, model);
  } else if (model_type == "MSM") {
    return fillConfig<cd::MSMConfig>(filepath, model);
  } else if (model_type == "MDD") {
    return fillConfig<cd::MDDConfig>(filepath, model);
  } else {
    handleWrongStringOption(filepath, "model_type", model_type);
  }

  return nullptr;
}

/*----------------------------------------------------------------------------*/

void Interpreter::handleWrongStringOption(const std::string &filepath,
                                          const std::string& option_name,
                                          const std::string& option_value) {
  if (option_value == "") {
    throw std::logic_error(filepath + ": " + option_name + " not specified!");
  } else {
    throw std::logic_error(
      filepath + ": unknown " + option_name + " \"" + option_value + "\"");
  }
}

/*----------------------------------------------------------------------------*/

bool Interpreter::missingObjectException(const std::exception &e) {
  std::string exception(e.what());
  return exception.find("Can not find object:") != std::string::npos;
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerCommonTypes(chaiscript::ModulePtr &module,
                                      const std::string &/* filepath */) {
  // Ordinary types
  REGISTER_COMMON_TYPE(Size);
  REGISTER_COMMON_TYPE(Type);
  REGISTER_COMMON_TYPE(Domain);
  REGISTER_COMMON_TYPE(Probability);
  REGISTER_COMMON_TYPE(FeatureFunction);

  REGISTER_COMMON_MAP(Probabilities);
  REGISTER_COMMON_MAP(FeatureFunctions);

  REGISTER_COMMON_VECTOR(Domains);
  REGISTER_COMMON_VECTOR(Alphabet);
  REGISTER_COMMON_VECTOR(Alphabets);
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerCommonHelpers(chaiscript::ModulePtr &module,
                                        const std::string &filepath) {
  using chaiscript::fun;

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

void Interpreter::registerCommonConstants(chaiscript::ModulePtr &module,
                                          const std::string &/* filepath */) {
  using chaiscript::const_var;

  module->add_global_const(const_var(std::string("emission")), "emission");
  module->add_global_const(const_var(std::string("duration")), "duration");
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerCommonAttributions(chaiscript::ModulePtr &module,
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
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerCommonConcatenations(
    chaiscript::ModulePtr &module, const std::string &/* filepath */) {
  using chaiscript::fun;

  module->add(fun([] (const std::string &lhs, const std::string &rhs) {
    return lhs + " | " + rhs;
  }), "|");

  module->add(fun([] (const std::string &lhs, const std::string &rhs) {
    return rhs + " | " + lhs;
  }), "->");
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerTrainingTypes(chaiscript::ModulePtr &module,
                                        const std::string &/* filepath */) {
  // Definitions
  REGISTER_TRAINING_TYPE(Model);
  REGISTER_TRAINING_TYPE(State);
  REGISTER_TRAINING_TYPE(Duration);

  REGISTER_TRAINING_MAP(States);

  REGISTER_TRAINING_VECTOR(Models);
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerTrainingHelpers(chaiscript::ModulePtr &module,
                                          const std::string &filepath) {
  using chaiscript::fun;

  using ct::PretrainedModelConfig;
  using ct::FixedDurationConfig;
  using ct::ExplicitDurationConfig;
  using ct::GeometricDurationConfig;
  using ct::MaxLengthDurationConfig;

  module->add(fun([this, filepath] (const std::string &file) {
    auto untrained_model
      = this->makeModelTrainingConfig(extractDir(filepath) + file);
    std::get<decltype("category"_t)>(*untrained_model.get())
      = "untrained_model";
    return cot::Model(untrained_model);
  }), "untrained_model");

  module->add(fun([this, filepath] (const std::string &file) {
    auto pretrained_model
      = PretrainedModelConfig::make(filepath, "pretrained_model");
    std::get<decltype("category"_t)>(*pretrained_model.get())
      = "pretrained_model";
    std::get<decltype("pretrained_model"_t)>(*pretrained_model.get())
      = this->makeModelDefinitionConfig(extractDir(filepath) + file);
    return cot::Model(pretrained_model);
  }), "pretrained_model");

  module->add(fun([this, filepath]() {
    auto duration = GeometricDurationConfig::make(filepath, "geometric");
    return cot::Duration(duration);
  }), "geometric");

  module->add(fun([this, filepath] (const std::string &file) {
    auto duration = ExplicitDurationConfig::make(filepath, "explicit");
    std::get<decltype("model"_t)>(*duration.get())
      = this->makeModelTrainingConfig(extractDir(filepath) + file);
    return cot::Duration(duration);
  }), "explicit");

  module->add(fun([this, filepath] (const std::string &file,
                                    unsigned int size) {
    auto duration = ExplicitDurationConfig::make(filepath, "explicit");
    std::get<decltype("max_size"_t)>(*duration.get()) = size;
    std::get<decltype("model"_t)>(*duration.get())
      = this->makeModelTrainingConfig(extractDir(filepath) + file);
    return cot::Duration(duration);
  }), "explicit");

  module->add(fun([this, filepath] (cot::Model model,
                                    unsigned int size) {
    auto duration = ExplicitDurationConfig::make(filepath, "explicit");
    std::get<decltype("max_size"_t)>(*duration.get()) = size;
    std::get<decltype("model"_t)>(*duration.get()) = model;
    return cot::Duration(duration);
  }), "explicit");

  module->add(fun([this, filepath] (unsigned int size) {
    auto duration = FixedDurationConfig::make(filepath, "fixed");
    std::get<decltype("size"_t)>(*duration.get()) = size;
    return cot::Duration(duration);
  }), "fixed");

  module->add(fun([this, filepath] (unsigned int size) {
    auto duration = MaxLengthDurationConfig::make(filepath, "max_length");
    std::get<decltype("size"_t)>(*duration.get()) = size;
    return cot::Duration(duration);
  }), "max_length");

  module->add(fun([this, filepath] (const std::string &file) {
    return extractDir(filepath) + file;
  }), "dataset");
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerTrainingAttributions(chaiscript::ModulePtr &module,
                                               const std::string &filepath) {
  using chaiscript::fun;
  using chaiscript::boxed_cast;

  using chaiscript::Boxed_Value;
  using Map = std::map<std::string, Boxed_Value>;

  module->add(fun([filepath] (cot::States &conv, const Map &orig) {
    for (auto &pair : orig) {
      auto inner_orig = boxed_cast<Map &>(pair.second);

      conv[pair.first] = ct::StateConfig::make(filepath);

      std::get<decltype("duration"_t)>(*conv[pair.first])
        = boxed_cast<cot::Duration>(inner_orig["duration"]);

      std::get<decltype("emission"_t)>(*conv[pair.first])
        = boxed_cast<cot::Model>(inner_orig["emission"]);
    }
  }), "=");
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerDefinitionTypes(chaiscript::ModulePtr &module,
                                          const std::string &/* filepath */) {
  // Definitions
  REGISTER_DEFINITION_TYPE(Model);
  REGISTER_DEFINITION_TYPE(State);
  REGISTER_DEFINITION_TYPE(Duration);
  REGISTER_DEFINITION_TYPE(DependencyTree);

  REGISTER_DEFINITION_MAP(States);

  REGISTER_DEFINITION_VECTOR(Models);
  REGISTER_DEFINITION_VECTOR(DependencyTrees);
  REGISTER_DEFINITION_VECTOR(FeatureFunctionLibraries);
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerDefinitionHelpers(chaiscript::ModulePtr &module,
                                            const std::string &filepath) {
  using chaiscript::fun;

  using cd::FixedDurationConfig;
  using cd::ExplicitDurationConfig;
  using cd::GeometricDurationConfig;
  using cd::MaxLengthDurationConfig;
  using cd::FeatureFunctionLibraryConfig;
  using cd::DependencyTreeConfig;

  module->add(fun([this, filepath] (const std::string &file) {
    auto root_dir = extractDir(filepath);
    return this->makeModelDefinitionConfig(root_dir + file);
  }), "model");

  module->add(fun([this, filepath]() {
    auto duration = GeometricDurationConfig::make(filepath, "geometric");
    return cod::Duration(duration);
  }), "geometric");

  module->add(fun([this, filepath] (const std::string &file) {
    auto duration = ExplicitDurationConfig::make(filepath, "explicit");
    std::get<decltype("model"_t)>(*duration.get())
      = this->makeModelDefinitionConfig(extractDir(filepath) + file);
    return cod::Duration(duration);
  }), "explicit");

  module->add(fun([this, filepath] (const std::string &file,
                                    unsigned int size) {
    auto duration = ExplicitDurationConfig::make(filepath, "explicit");
    std::get<decltype("max_size"_t)>(*duration.get()) = size;
    std::get<decltype("model"_t)>(*duration.get())
      = this->makeModelDefinitionConfig(extractDir(filepath) + file);
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
}

/*----------------------------------------------------------------------------*/

void Interpreter::registerDefinitionAttributions(chaiscript::ModulePtr &module,
                                                 const std::string &filepath) {
  using chaiscript::fun;
  using chaiscript::boxed_cast;

  using chaiscript::Boxed_Value;
  using Map = std::map<std::string, Boxed_Value>;

  module->add(fun([filepath] (cod::States &conv, const Map &orig) {
    for (auto &pair : orig) {
      auto inner_orig = boxed_cast<Map &>(pair.second);

      conv[pair.first] = cd::StateConfig::make(filepath);

      std::get<decltype("duration"_t)>(*conv[pair.first])
        = boxed_cast<cod::Duration>(inner_orig["duration"]);

      std::get<decltype("emission"_t)>(*conv[pair.first])
        = boxed_cast<cod::Model>(inner_orig["emission"]);
    }
  }), "=");
}

/*----------------------------------------------------------------------------*/

}  // namespace lang
