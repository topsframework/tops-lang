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

#ifndef CONFIG_CONFIG_VISITOR_
#define CONFIG_CONFIG_VISITOR_

// Standard headers
#include <memory>
#include <string>
#include <type_traits>

#include "config/definition/Options.hpp"
#include "config/definition/ModelConfig.hpp"
#include "config/definition/StateConfig.hpp"
#include "config/definition/DurationConfig.hpp"
#include "config/definition/DependencyTreeConfig.hpp"
#include "config/definition/FeatureFunctionLibraryConfig.hpp"

namespace config {

// Forward declaration
class BasicConfigInterface;

/**
 * @class ConfigVisitor
 * @brief Visitor for config::BasicConfig's options
 */
class ConfigVisitor {
 public:
  // Concrete methods
  template<typename Base, typename... Options>
  void visit(std::shared_ptr<BasicConfig<Base, Options...>> config) {
    std::size_t count = 0;
    std::size_t max = config->number_of_options();

    this->visitLabel(config->label());
    this->visitPath(config->path());

    this->startVisit();
    config->for_each([this, &count, &max] (const auto &tag, auto &value) {
      using Tag = std::decay_t<decltype(tag)>;
      using Value = std::decay_t<decltype(value)>;
      this->visitTag(typename Tag::value_type().str(), count, max);
      this->visitOption(const_cast<Value&>(value));
      count++;
    });
    this->endVisit();
  }

  // Virtual destructor
  virtual ~ConfigVisitor() = default;

 protected:
  // Purely virtual methods
  virtual void startVisit() = 0;
  virtual void endVisit() = 0;

  virtual void visitTag(const std::string &, std::size_t, std::size_t) = 0;
  virtual void visitLabel(const std::string &) = 0;
  virtual void visitPath(const std::string &) = 0;

  virtual void visitOption(definition::option::Model &) = 0;
  virtual void visitOption(definition::option::State &) = 0;
  virtual void visitOption(definition::option::Duration &) = 0;
  virtual void visitOption(definition::option::FeatureFunctionLibrary &) = 0;

  virtual void visitOption(definition::option::Models &) = 0;
  virtual void visitOption(definition::option::States &) = 0;
  virtual void visitOption(definition::option::DependencyTrees &) = 0;
  virtual void visitOption(definition::option::FeatureFunctionLibraries &) = 0;

  virtual void visitOption(definition::option::Type &) = 0;
  virtual void visitOption(definition::option::Size &) = 0;
  virtual void visitOption(definition::option::Domain &) = 0;
  virtual void visitOption(definition::option::Domains &) = 0;
  virtual void visitOption(definition::option::Alphabet &) = 0;
  virtual void visitOption(definition::option::Alphabets &) = 0;
  virtual void visitOption(definition::option::Probability &) = 0;
  virtual void visitOption(definition::option::Probabilities &) = 0;
  virtual void visitOption(definition::option::DependencyTree &) = 0;
  virtual void visitOption(definition::option::FeatureFunctions &) = 0;
  virtual void visitOption(definition::option::OutToInSymbolFunction &) = 0;
  virtual void visitOption(definition::option::InToOutSymbolFunction &) = 0;
};

}  // namespace config

#endif  // CONFIG_CONFIG_VISITOR_
