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

#ifndef CONFIG_MODEL_CONFIG_VISITOR_
#define CONFIG_MODEL_CONFIG_VISITOR_

// Forward declaration
template<typename Base, typename... Options> class BasicConfig;

// Standard headers
#include <memory>
#include <string>
#include <type_traits>

// Internal headers
#include "config/Options.hpp"
#include "config/ModelConfig.hpp"
#include "config/StateConfig.hpp"
#include "config/DurationConfig.hpp"
#include "config/DependencyTreeConfig.hpp"
#include "config/FeatureFunctionLibraryConfig.hpp"

namespace config {

class ModelConfigVisitor {
 public:
  // Concrete methods
  template<typename Base, typename... Options>
  void visit(std::shared_ptr<BasicConfig<Base, Options...>> config_ptr);

  // Virtual destructor
  virtual ~ModelConfigVisitor() = default;

 protected:
  // Purely virtual methods
  virtual void startVisit() = 0;
  virtual void endVisit() = 0;

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
  virtual void visitOption(option::Alphabet &) = 0;
  virtual void visitOption(option::Alphabets &) = 0;
  virtual void visitOption(option::Probability &) = 0;
  virtual void visitOption(option::Probabilities &) = 0;
  virtual void visitOption(option::DependencyTree &) = 0;
  virtual void visitOption(option::FeatureFunctions &) = 0;

  virtual void visitTag(const std::string &, std::size_t, std::size_t) = 0;
  virtual void visitLabel(const std::string &) = 0;
  virtual void visitPath(const std::string &) = 0;
};

}  // namespace config

// Implementation header
#include "config/ModelConfigVisitor.ipp"

#endif  // CONFIG_MODEL_CONFIG_VISITOR_
