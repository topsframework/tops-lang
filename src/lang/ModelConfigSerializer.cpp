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
#include "lang/ModelConfigSerializer.hpp"

// Standard headers
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cstddef>
#include <algorithm>

// Internal headers
#include "lang/FilePrinter.hpp"
#include "lang/SingleFilePrinter.hpp"
#include "lang/MultipleFilePrinter.hpp"

#include "config/Options.hpp"
#include "config/training/ModelConfig.hpp"
#include "config/definition/ModelConfig.hpp"
#include "config/definition/StateConfig.hpp"
#include "config/definition/DurationConfig.hpp"
#include "config/definition/DependencyTreeConfig.hpp"
#include "config/definition/FeatureFunctionLibraryConfig.hpp"

// Namespace aliases
namespace { namespace co = config::option; }
namespace { namespace cot = co::training; }
namespace { namespace cod = co::definition; }

namespace lang {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

ModelConfigSerializer::ModelConfigSerializer(
    std::shared_ptr<FilePrinter> printer)
    : printer_(printer) {
}

/*----------------------------------------------------------------------------*/

ModelConfigSerializer::ModelConfigSerializer(std::ostream &os)
    : printer_(std::make_shared<SingleFilePrinter>(
          std::shared_ptr<std::ostream>(&os, [] (void *) {}))) {
}

/*----------------------------------------------------------------------------*/

ModelConfigSerializer::ModelConfigSerializer(const std::string &root_dir)
    : printer_(std::make_shared<MultipleFilePrinter>(root_dir)) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::startVisit() {
  printer_->startPrinting();
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::endVisit() {
  printer_->endPrinting();
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(co::Size &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(co::Type &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(co::Domain &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(co::Domains &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(co::Alphabet &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(co::Alphabets &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(co::Probability &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(co::Probabilities &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(co::FeatureFunctions &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(co::OutToInSymbolFunction &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(co::InToOutSymbolFunction &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(cot::Model &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(cot::Models &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(cot::State &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(cot::States &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(cot::Duration &visited) {
  printer_->print(visited);
}


/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(cod::Model &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(cod::Models &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(cod::State &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(cod::States &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(cod::Duration &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(cod::DependencyTree &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(cod::DependencyTrees &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(cod::FeatureFunctionLibrary &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitOption(
    cod::FeatureFunctionLibraries &visited) {
  printer_->print(visited);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitTag(const std::string &tag,
                                     std::size_t count,
                                     std::size_t max) {
  printer_->printTag(tag, count, max);
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitLabel(const std::string &/* label */) {
}

/*----------------------------------------------------------------------------*/

void ModelConfigSerializer::visitPath(const std::string &path) {
  printer_->changeOstream(path);
}

/*----------------------------------------------------------------------------*/

}  // namespace lang
