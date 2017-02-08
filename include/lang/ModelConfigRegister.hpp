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
#include "config/ConfigVisitor.hpp"

#include "config/Options.hpp"
#include "config/definition/ModelConfig.hpp"
#include "config/definition/StateConfig.hpp"
#include "config/definition/DurationConfig.hpp"
#include "config/definition/DependencyTreeConfig.hpp"
#include "config/definition/FeatureFunctionLibraryConfig.hpp"

// External headers
#include "chaiscript/chaiscript.hpp"

// Namespace aliases
namespace { namespace co = config::option; }
namespace { namespace cod = co::definition; }

namespace lang {

/**
 * @class ModelConfigRegister
 * Implementation of config::ConfigVisitor
 * to register models in ChaiScript.
 */
class ModelConfigRegister
    : public config::ConfigVisitor {
 public:
  // Constructors
  explicit ModelConfigRegister(chaiscript::ChaiScript &chai);

 protected:
  // Overriden functions
  void startVisit() override;
  void endVisit() override;

  void visitOption(cod::Model &visited) override;
  void visitOption(cod::State &visited) override;
  void visitOption(cod::Duration &visited) override;
  void visitOption(cod::DependencyTree &visited) override;
  void visitOption(cod::FeatureFunctionLibrary &visited) override;

  void visitOption(cod::Models &visited) override;
  void visitOption(cod::States &visited) override;
  void visitOption(cod::DependencyTrees &visited) override;
  void visitOption(cod::FeatureFunctionLibraries &visited) override;

  void visitOption(co::Size &visited) override;
  void visitOption(co::Type &visited) override;
  void visitOption(co::Domain &visited) override;
  void visitOption(co::Domains &visited) override;
  void visitOption(co::Alphabet &visited) override;
  void visitOption(co::Alphabets &visited) override;
  void visitOption(co::Probability &visited) override;
  void visitOption(co::Probabilities &visited) override;
  void visitOption(co::FeatureFunctions &visited) override;
  void visitOption(co::OutToInSymbolFunction &visited) override;
  void visitOption(co::InToOutSymbolFunction &visited) override;

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
