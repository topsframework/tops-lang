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
#include <string>

// Internal headers
#include "config/Options.hpp"
#include "config/ModelConfig.hpp"
#include "config/StateConfig.hpp"
#include "config/DurationConfig.hpp"
#include "config/DependencyTreeConfig.hpp"
#include "config/FeatureFunctionLibraryConfig.hpp"

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

void ModelConfigRegister::visitOption(config::option::Model &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(config::option::State &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(config::option::Duration &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(
    config::option::FeatureFunctionLibrary &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(config::option::Models &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(config::option::States &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(
    config::option::DependencyTrees &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(
    config::option::FeatureFunctionLibraries &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(config::option::Type &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(config::option::Size &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(config::option::Alphabet &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(config::option::Probability &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(config::option::Probabilities &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(config::option::DependencyTree &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitOption(
    config::option::FeatureFunctions &visited) {
  chai_.add(chaiscript::var(&visited), tag_);
  chai_.add(chaiscript::fun([&visited] (
      const std::string &name, config::option::FeatureFunction fun) {
    visited.emplace(name, fun);
  }), "feature");
}

/*----------------------------------------------------------------------------*/

void ModelConfigRegister::visitTag(
    const std::string &tag, std::size_t /* count */, std::size_t /* max */) {
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
