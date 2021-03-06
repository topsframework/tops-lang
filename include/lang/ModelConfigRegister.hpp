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

#ifndef LANG_MODEL_CONFIG_REGISTER_
#define LANG_MODEL_CONFIG_REGISTER_

// Standard headers
#include <string>
#include <cstddef>

// Internal headers
#include "config/Options.hpp"
#include "config/BasicConfig.hpp"
#include "config/ModelConfigVisitor.hpp"

#include "config/ModelConfig.hpp"
#include "config/StateConfig.hpp"
#include "config/DurationConfig.hpp"
#include "config/DependencyTreeConfig.hpp"
#include "config/FeatureFunctionLibraryConfig.hpp"

// External headers
#include "chaiscript/chaiscript.hpp"

namespace lang {

/**
 * @class ModelConfigRegister
 * Implementation of config::ModelConfigVisitor to register models in ChaiScript
 */
class ModelConfigRegister : public config::ModelConfigVisitor {
 public:
  // Constructors
  explicit ModelConfigRegister(chaiscript::ChaiScript &chai);

 protected:
  // Overriden functions
  void startVisit() override;
  void endVisit() override;

  void visitOption(config::option::Model &visited) override;
  void visitOption(config::option::State &visited) override;
  void visitOption(config::option::Domain &visited) override;
  void visitOption(config::option::Duration &visited) override;
  void visitOption(config::option::DependencyTree &visited) override;
  void visitOption(config::option::FeatureFunctionLibrary &visited) override;

  void visitOption(config::option::Models &visited) override;
  void visitOption(config::option::States &visited) override;
  void visitOption(config::option::Domains &visited) override;
  void visitOption(config::option::DependencyTrees &visited) override;
  void visitOption(config::option::FeatureFunctionLibraries &visited) override;

  void visitOption(config::option::Type &visited) override;
  void visitOption(config::option::Size &visited) override;
  void visitOption(config::option::Alphabet &visited) override;
  void visitOption(config::option::Alphabets &visited) override;
  void visitOption(config::option::Probability &visited) override;
  void visitOption(config::option::Probabilities &visited) override;
  void visitOption(config::option::FeatureFunctions &visited) override;

  void visitOption(config::option::OutToInSymbolFunction &visited) override;
  void visitOption(config::option::InToOutSymbolFunction &visited) override;

  void visitTag(const std::string &tag, std::size_t /* count */,
                                        std::size_t /* max */) override;

  void visitLabel(const std::string &/* label */) override;
  void visitPath(const std::string &/* path */) override;

 private:
  // Instance variables
  chaiscript::ChaiScript &chai_;
  std::string tag_;
};

}  // namespace lang

#endif  // LANG_MODEL_CONFIG_REGISTER_
