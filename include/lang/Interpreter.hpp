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

#ifndef LANG_INTERPRETER_
#define LANG_INTERPRETER_

// Standard headers
#include <string>
#include <memory>

// Internal headers
#include "config/Converter.hpp"
#include "config/ModelConfig.hpp"

// External headers
#include "chaiscript/chaiscript.hpp"
#include "chaiscript/dispatchkit/bootstrap_stl.hpp"

namespace lang {

/**
 * @class Interpreter
 * @brief Interpreter for ToPS' configuration file language
 */
class Interpreter {
 public:
  // Inner struct
  struct Environment {
    config::ModelConfigPtr model_config_ptr;
    config::ConverterPtr converter_ptr;
  };

  // Concrete methods
  Environment evalModel(const std::string &filepath);

 private:
  // Concrete methods
  void checkExtension(const std::string &filepath);

  config::ModelConfigPtr makeModelConfig(const std::string &filepath);
  config::ConverterPtr makeConverver(config::ModelConfigPtr model_cfg);

  std::string findModelType(const std::string &filepath);

  bool missingObjectException(const std::exception &e);

  template<typename Config>
  std::shared_ptr<Config> fillConfig(const std::string &filepath);

  chaiscript::ModulePtr makeInterpreterLibrary(const std::string &filepath);

  void registerTypes(chaiscript::ModulePtr &module,
                     const std::string &filepath);
  void registerHelpers(chaiscript::ModulePtr &module,
                       const std::string &filepath);
  void registerConstants(chaiscript::ModulePtr &module,
                         const std::string &filepath);
  void registerAttributions(chaiscript::ModulePtr &module,
                            const std::string &filepath);
  void registerConcatenations(chaiscript::ModulePtr &module,
                              const std::string &filepath);
};

}  // namespace lang

// Implementation header
#include "lang/Interpreter.ipp"

#endif  // LANG_INTERPRETER_
