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

#ifndef LANG_MULTIPLE_FILE_PRINTER_
#define LANG_MULTIPLE_FILE_PRINTER_

// Standard headers
#include <list>
#include <string>

// Internal headers
#include "lang/FilePrinter.hpp"

#include "config/ModelConfig.hpp"
#include "config/StateConfig.hpp"
#include "config/DependencyTreeConfig.hpp"
#include "config/FeatureFunctionLibraryConfig.hpp"

#include "filesystem/Filesystem.hpp"

namespace lang {

/**
 * @class MultipleFilePrinter
 * @brief Class to print config::BasicConfig in multiple files
 */
class MultipleFilePrinter : public FilePrinter {
 public:
  // Alias
  using Base = FilePrinter;
  using Self = MultipleFilePrinter;

  // Constructors
  explicit MultipleFilePrinter(const std::string &root_dir);

  // Static methods
  template<typename... Args>
  static decltype(auto) make(Args&&... args);

  // Overriden methods
  void changeOstream(const std::string &path) override;

  void endPrinting() override;

  void print(config::ModelConfigPtr config_ptr) override;
  void print(config::StateConfigPtr state_ptr) override;
  void print(config::DurationConfigPtr duration_ptr) override;
  void print(config::FeatureFunctionLibraryConfigPtr library_ptr) override;
  void print(config::DependencyTreeConfigPtr tree_ptr) override;

 protected:
  // Instance variables
  bool change_ostream_;

  std::string root_dir_;
  std::string working_dir_;

  std::list<config::ModelConfigPtr> submodels_;
  std::list<config::FeatureFunctionLibraryConfigPtr> libraries_;
  std::list<config::DependencyTreeConfigPtr> trees_;

  // Constructors
  template<typename... Args>
  MultipleFilePrinter(bool change_ostream,
                      const std::string &root_dir,
                      Args&&... args);

 private:
  // Concrete methods
  std::string pathForHelperCall(const std::string &path);

  void printSubmodel(config::ModelConfigPtr submodel_ptr);
  void printLibrary(config::FeatureFunctionLibraryConfigPtr library_ptr);
  void printTree(config::DependencyTreeConfigPtr tree_ptr);
};

}  // namespace lang

// Implementation header
#include "lang/MultipleFilePrinter.ipp"

#endif  // LANG_MULTIPLE_FILE_PRINTER_
