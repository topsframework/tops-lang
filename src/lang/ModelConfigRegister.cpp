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
#include "lang/ModelConfigRegister.hpp"

// Standard headers
#include <memory>
#include <string>
#include <cstddef>

// Internal headers
#include "config/BasicConfig.hpp"

#include "config/Options.hpp"

#include "config/training/ModelConfig.hpp"
#include "config/training/StateConfig.hpp"
#include "config/training/DurationConfig.hpp"

#include "config/definition/ModelConfig.hpp"
#include "config/definition/StateConfig.hpp"
#include "config/definition/DurationConfig.hpp"
#include "config/definition/DependencyTreeConfig.hpp"
#include "config/definition/FeatureFunctionLibraryConfig.hpp"

// External headers
#include "chaiscript/language/chaiscript_engine.hpp"

#include "chaiscript/dispatchkit/boxed_value.hpp"
#include "chaiscript/dispatchkit/register_function.hpp"

// Namespace aliases
namespace { namespace co = config::option; }
namespace { namespace cot = co::training; }
namespace { namespace cod = co::definition; }

// Remove!
#include <iostream>

namespace lang {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

ModelConfigRegister::ModelConfigRegister(chaiscript::ChaiScript &chai)
    : chai_(chai) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

void ModelConfigRegister::startVisit() {
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::endVisit() {
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::Size &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::Type &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::Domain &visited) {
  visited = std::make_shared<typename co::Domain::element_type>();
  chai_.add(chaiscript::var(visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::Domains &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::Alphabet &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::Alphabets &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::Probability &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::Probabilities &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::FeatureFunctions &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
  chai_.add(chaiscript::fun([&visited] (
      const std::string &name, co::FeatureFunction fun) {
    visited.emplace(name, fun);
  }), "feature");
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::OutToInSymbolFunction &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(co::InToOutSymbolFunction &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cot::Model &visited) {
  visited = std::make_shared<typename cot::Model::element_type>();
  chai_.add(chaiscript::var(visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cot::Models &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cot::State &visited) {
  visited = std::make_shared<typename cot::State::element_type>();
  chai_.add(chaiscript::var(visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cot::States &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cot::Duration &visited) {
  visited = std::make_shared<typename cot::Duration::element_type>();
  chai_.add(chaiscript::var(visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cod::Model &visited) {
  visited = std::make_shared<typename cod::Model::element_type>();
  chai_.add(chaiscript::var(visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cod::Models &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cod::State &visited) {
  visited = std::make_shared<typename cod::State::element_type>();
  chai_.add(chaiscript::var(visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cod::States &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cod::Duration &visited) {
  visited = std::make_shared<typename cod::Duration::element_type>();
  chai_.add(chaiscript::var(visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cod::DependencyTree &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cod::DependencyTrees &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cod::FeatureFunctionLibrary &visited) {
  visited
    = std::make_shared<typename cod::FeatureFunctionLibrary::element_type>();
  chai_.add(chaiscript::var(visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cod::FeatureFunctionLibraries &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}


/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitTag(const std::string &tag,
                                   std::size_t /* count */,
                                   std::size_t /* max */) {
  tag_ = tag;
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitLabel(const std::string &/* label */) {
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitPath(const std::string &/* path */) {
}

/*----------------------------------------------------------------------------*/

}  // namespace lang
