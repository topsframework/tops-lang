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

#include "config/definition/Options.hpp"
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
namespace { namespace cdo = config::definition::option; }

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

void ModelConfigRegister::visitOption(cdo::Model &visited) {
  visited = std::make_shared<typename cdo::Model::element_type>();
  chai_.add(chaiscript::var(visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cdo::State &visited) {
  visited = std::make_shared<typename cdo::State::element_type>();
  chai_.add(chaiscript::var(visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cdo::Domain &visited) {
  visited = std::make_shared<typename cdo::Domain::element_type>();
  chai_.add(chaiscript::var(visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cdo::Duration &visited) {
  visited = std::make_shared<typename cdo::Duration::element_type>();
  chai_.add(chaiscript::var(visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cdo::FeatureFunctionLibrary &visited) {
  visited
    = std::make_shared<typename cdo::FeatureFunctionLibrary::element_type>();
  chai_.add(chaiscript::var(visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cdo::Models &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cdo::States &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cdo::Domains &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cdo::DependencyTrees &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cdo::FeatureFunctionLibraries &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cdo::Type &visited) {
  std::cerr << "Registering Type" << std::endl;
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cdo::Size &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cdo::Alphabet &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cdo::Alphabets &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cdo::Probability &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cdo::Probabilities &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cdo::DependencyTree &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cdo::FeatureFunctions &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
  chai_.add(chaiscript::fun([&visited] (
      const std::string &name, cdo::FeatureFunction fun) {
    visited.emplace(name, fun);
  }), "feature");
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cdo::OutToInSymbolFunction &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(cdo::InToOutSymbolFunction &visited) {
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
