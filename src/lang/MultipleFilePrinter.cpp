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
#include "lang/MultipleFilePrinter.hpp"

// Standard headers
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

// Internal headers
#include "lang/Util.hpp"
#include "lang/ModelConfigSerializer.hpp"

#include "config/BasicConfig.hpp"

#include "filesystem/Filesystem.hpp"

namespace lang {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

MultipleFilePrinter::MultipleFilePrinter(const std::string &root_dir)
  : Base(nullptr, 0), change_ostream_(true),
    root_dir_(cleanPath(root_dir + "/")) {
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::changeOstream(const std::string &path) {
  working_dir_ = extractDir(extractCorename(path));
  if (change_ostream_) {
    auto new_path = root_dir_ + extractCorename(path);
    filesystem::create_directories(extractDir(new_path));
    os_ = std::make_shared<std::ofstream>(new_path);
  }
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::endPrinting() {
  Base::endPrinting();

  for (auto &submodel : submodels_)
    printSubmodel(submodel);
  for (auto &library : libraries_)
    printLibrary(library);
  for (auto &tree : trees_)
    printTree(tree);
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::print(config::ModelConfigPtr config_ptr) {
  callFunction("model", pathForHelperCall(config_ptr->path()));
  submodels_.push_back(config_ptr);
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::print(config::StateConfigPtr state_ptr) {
  openSection('[');
  state_ptr->accept(ModelConfigSerializer(
        Self::make(false, root_dir_, os_, depth_, ": ", ",\n")));
  closeSection(']');
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::print(config::DurationConfigPtr duration_ptr) {
  openFunction(duration_ptr->label());
  duration_ptr->accept(ModelConfigSerializer(
        Self::make(false, root_dir_, os_, depth_, "", ", ", "")));
  closeFunction();
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::print(
    config::FeatureFunctionLibraryConfigPtr library_ptr) {
  callFunction("lib", pathForHelperCall(library_ptr->path()));
  libraries_.push_back(library_ptr);
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::print(config::DependencyTreeConfigPtr tree_ptr) {
  callFunction("tree", pathForHelperCall(tree_ptr->path()));
  trees_.push_back(tree_ptr);
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

std::string MultipleFilePrinter::pathForHelperCall(const std::string &path) {
  return '"' + removeSubstring(working_dir_, extractCorename(path)) + '"';
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::printSubmodel(config::ModelConfigPtr submodel_ptr) {
  submodel_ptr->accept(ModelConfigSerializer(
        Self::make(true, root_dir_, os_, 0)));
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::printLibrary(
    config::FeatureFunctionLibraryConfigPtr library_ptr) {
  copy(library_ptr, std::make_shared<std::ofstream>(
        root_dir_ + extractCorename(library_ptr->path())));
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::printTree(config::DependencyTreeConfigPtr tree_ptr) {
  thread_local unsigned int tree_depth = 0;
  thread_local std::vector<unsigned int> tree_nodes { 1 };

  if (tree_depth == 0)
    changeOstream(tree_ptr->path());

  indent();
  for (unsigned int i = 1; i < tree_nodes.size(); i++) {
    if (i == tree_depth) {
      if (tree_nodes[i] == 1)
        *os_ << " `- ";
      else
        *os_ << " |- ";
    } else {
      if (tree_nodes[i] == 0)
        *os_ << "    ";
      else
        *os_ << " |  ";
    }
  }

  tree_nodes[tree_depth]--;

  if (tree_nodes.size() == tree_depth + 1
  &&  tree_ptr->children().size() != 0)
    tree_nodes.push_back(tree_ptr->children().size());

  if (tree_nodes.size() == tree_depth + 1
  &&  tree_nodes[tree_depth] == 0)
    tree_nodes.pop_back();

  tree_ptr->accept(ModelConfigSerializer(
          Self::make(false, root_dir_, os_, depth_,
                     "", " ", "\n", "(", ")")));

  tree_depth++;

  for (auto& child : tree_ptr->children())
    printTree(child);

  tree_depth--;
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::print(config::DomainPtr domain_ptr) {
  openSection('{');
  domain_ptr->data()->accept(ModelConfigSerializer(
        Self::make(false, root_dir_, os_, depth_, "(", ", ", ")")));
  closeSection('}');
}

/*----------------------------------------------------------------------------*/

}  // namespace lang
