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

// Namespace aliases
namespace { namespace cdo = config::definition::option; }

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

void MultipleFilePrinter::startPrinting() {
  Base::startPrinting();

  if (change_ostream_) {
    *os_ << "// -*- mode: c++ -*-" << std::endl
         << "// vim: ft=chaiscript:" << std::endl
         << std::endl;
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

void MultipleFilePrinter::print(cdo::Model config) {
  callFunction("model", pathForHelperCall(config->path()));
  submodels_.push_back(config);
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::print(cdo::State state) {
  openSection('[');
  state->accept(ModelConfigSerializer(
        Self::make(false, root_dir_, os_, depth_, ": ", ",\n")));
  closeSection(']');
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::print(cdo::Duration duration) {
  openFunction(duration->label());
  duration->accept(ModelConfigSerializer(
        Self::make(false, root_dir_, os_, depth_, "", ", ", "")));
  closeFunction();
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::print(cdo::FeatureFunctionLibrary library) {
  callFunction("lib", pathForHelperCall(library->path()));
  libraries_.push_back(library);
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::print(cdo::DependencyTree tree) {
  callFunction("tree", pathForHelperCall(tree->path()));
  trees_.push_back(tree);
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

std::string MultipleFilePrinter::pathForHelperCall(const std::string &path) {
  return '"' + removeSubstring(working_dir_, extractCorename(path)) + '"';
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::printSubmodel(cdo::Model submodel) {
  submodel->accept(ModelConfigSerializer(
        Self::make(true, root_dir_, os_, 0)));
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::printLibrary(cdo::FeatureFunctionLibrary library) {
  copy(library, std::make_shared<std::ofstream>(
        root_dir_ + extractCorename(library->path())));
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::printTree(cdo::DependencyTree tree) {
  thread_local unsigned int tree_depth = 0;
  thread_local std::vector<unsigned int> tree_nodes { 1 };

  if (tree_depth == 0)
    changeOstream(tree->path());

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
  &&  tree->children().size() != 0)
    tree_nodes.push_back(tree->children().size());

  if (tree_nodes.size() == tree_depth + 1
  &&  tree_nodes[tree_depth] == 0)
    tree_nodes.pop_back();

  tree->accept(ModelConfigSerializer(
          Self::make(false, root_dir_, os_, depth_,
                     "", " ", "\n", "(", ")")));

  tree_depth++;

  for (auto& child : tree->children())
    printTree(child);

  tree_depth--;
}

/*----------------------------------------------------------------------------*/

void MultipleFilePrinter::print(cdo::Domain domain) {
  openFunction(domain->data()->label());
  domain->data()->accept(ModelConfigSerializer(
        Self::make(false, root_dir_, os_, depth_, "", ", ", "")));
  closeFunction();
}

/*----------------------------------------------------------------------------*/

}  // namespace lang
