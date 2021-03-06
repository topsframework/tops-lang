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
#include "lang/SingleFilePrinter.hpp"

// Standard headers
#include <memory>
#include <algorithm>

// Internal headers
#include "lang/ModelConfigSerializer.hpp"

#include "config/BasicConfig.hpp"

namespace lang {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

SingleFilePrinter::SingleFilePrinter(std::shared_ptr<std::ostream> os)
  : Base(os, 0) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

void SingleFilePrinter::print(config::ModelConfigPtr config_ptr) {
  openSection('{');
  config_ptr->accept(ModelConfigSerializer(
        Self::make(os_, depth_)));
  closeSection('}');
}

/*----------------------------------------------------------------------------*/

void SingleFilePrinter::print(config::StateConfigPtr state_ptr) {
  openSection('[');
  state_ptr->accept(ModelConfigSerializer(
        Self::make(os_, depth_, ": ", ",\n")));
  closeSection(']');
}

/*----------------------------------------------------------------------------*/

void SingleFilePrinter::print(config::DurationConfigPtr duration_ptr) {
  openFunction(duration_ptr->label());
  duration_ptr->accept(ModelConfigSerializer(
        Self::make(os_, depth_, "", ", ", "")));
  closeFunction();
}

/*----------------------------------------------------------------------------*/

void SingleFilePrinter::print(
    config::FeatureFunctionLibraryConfigPtr library_ptr) {
  openSection('{');
  copy(library_ptr, os_);
  closeSection('}');
}

/*----------------------------------------------------------------------------*/

void SingleFilePrinter::print(config::DependencyTreeConfigPtr tree_ptr) {
  thread_local unsigned int depth_tree = 0;

  if (depth_tree == 0)
    openFunction("tree");

  tree_ptr->accept(ModelConfigSerializer(
        Self::make(os_, depth_ + depth_tree, "", ", ", "\n")));

  depth_tree++;
  for (auto& child : tree_ptr->children()) {
    indent(depth_tree);
    print(child);
  }
  depth_tree--;

  if (depth_tree == 0) {
    indent(depth_tree);
    closeFunction();
  }
}

/*----------------------------------------------------------------------------*/

void SingleFilePrinter::print(config::DomainPtr domain_ptr) {
  openFunction(domain_ptr->data()->label());
  domain_ptr->data()->accept(ModelConfigSerializer(
                             Self::make(os_, depth_, "", ", ", "")));
  closeFunction();
}

/*----------------------------------------------------------------------------*/

}  // namespace lang
