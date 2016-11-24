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
#include "config/definition/DefinitionConfigVisitor.hpp"

#include "config/definition/Options.hpp"
#include "config/definition/ModelConfig.hpp"
#include "config/definition/StateConfig.hpp"
#include "config/definition/DurationConfig.hpp"
#include "config/definition/DependencyTreeConfig.hpp"
#include "config/definition/FeatureFunctionLibraryConfig.hpp"

// External headers
#include "chaiscript/chaiscript.hpp"

// Namespace aliases
namespace { namespace cdo = config::definition::option; }

namespace lang {

/**
 * @class ModelConfigRegister
 * Implementation of config::definition::DefinitionConfigVisitor
 * to register models in ChaiScript.
 */
class ModelConfigRegister
    : public config::definition::DefinitionConfigVisitor {
 public:
  // Constructors
  explicit ModelConfigRegister(chaiscript::ChaiScript &chai);

 protected:
  // Overriden functions
  void startVisit() override;
  void endVisit() override;

  void visitOption(cdo::Model &visited) override;
  void visitOption(cdo::State &visited) override;
  void visitOption(cdo::Duration &visited) override;
  void visitOption(cdo::DependencyTree &visited) override;
  void visitOption(cdo::FeatureFunctionLibrary &visited) override;

  void visitOption(cdo::Models &visited) override;
  void visitOption(cdo::States &visited) override;
  void visitOption(cdo::DependencyTrees &visited) override;
  void visitOption(cdo::FeatureFunctionLibraries &visited) override;

  void visitOption(cdo::Type &visited) override;
  void visitOption(cdo::Size &visited) override;
  void visitOption(cdo::Domain &visited) override;
  void visitOption(cdo::Domains &visited) override;
  void visitOption(cdo::Alphabet &visited) override;
  void visitOption(cdo::Alphabets &visited) override;
  void visitOption(cdo::Probability &visited) override;
  void visitOption(cdo::Probabilities &visited) override;
  void visitOption(cdo::FeatureFunctions &visited) override;
  void visitOption(cdo::OutToInSymbolFunction &visited) override;
  void visitOption(cdo::InToOutSymbolFunction &visited) override;

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
