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

#ifndef CONFIG_DEFINITION_DEFINITION_CONFIG_VISITOR_
#define CONFIG_DEFINITION_DEFINITION_CONFIG_VISITOR_

// Internal headers
#include "config/ConfigVisitor.hpp"
#include "config/BasicConfigInterface.hpp"

#include "config/definition/Options.hpp"
#include "config/definition/ModelConfig.hpp"
#include "config/definition/StateConfig.hpp"
#include "config/definition/DurationConfig.hpp"
#include "config/definition/DependencyTreeConfig.hpp"
#include "config/definition/FeatureFunctionLibraryConfig.hpp"

namespace config {
namespace definition {

class DefinitionConfigVisitor : public ConfigVisitor {
 public:
  // Purely virtual methods
  virtual void visitOption(option::Model &) = 0;
  virtual void visitOption(option::State &) = 0;
  virtual void visitOption(option::Duration &) = 0;
  virtual void visitOption(option::FeatureFunctionLibrary &) = 0;

  virtual void visitOption(option::Models &) = 0;
  virtual void visitOption(option::States &) = 0;
  virtual void visitOption(option::DependencyTrees &) = 0;
  virtual void visitOption(option::FeatureFunctionLibraries &) = 0;

  virtual void visitOption(option::Type &) = 0;
  virtual void visitOption(option::Size &) = 0;
  virtual void visitOption(option::Domain &) = 0;
  virtual void visitOption(option::Domains &) = 0;
  virtual void visitOption(option::Alphabet &) = 0;
  virtual void visitOption(option::Alphabets &) = 0;
  virtual void visitOption(option::Probability &) = 0;
  virtual void visitOption(option::Probabilities &) = 0;
  virtual void visitOption(option::DependencyTree &) = 0;
  virtual void visitOption(option::FeatureFunctions &) = 0;
  virtual void visitOption(option::OutToInSymbolFunction &) = 0;
  virtual void visitOption(option::InToOutSymbolFunction &) = 0;
};

}  // namespace definition
}  // namespace config

#endif  // CONFIG_DEFINITION_DEFINITION_CONFIG_VISITOR_
