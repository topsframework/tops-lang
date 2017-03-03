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

#include "config/Domain.hpp"

#include "config/training/ModelConfig.hpp"
#include "config/training/StateConfig.hpp"
#include "config/training/DurationConfig.hpp"

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

/**
 * @class MultipleFilePrinter
 * @brief Class to print config::BasicConfig in multiple files
 */
class MultipleFilePrinter : public FilePrinter {
 public:
  // Aliases
  using Base = FilePrinter;
  using Self = MultipleFilePrinter;

  // Constructors
  explicit MultipleFilePrinter(const std::string &root_dir);

  // Static methods
  template<typename... Args>
  static decltype(auto) make(Args&&... args);

  // Overriden methods
  void changeOstream(const std::string &path) override;

  void startPrinting() override;
  void endPrinting() override;

  void print(co::Domain domain) override;

  void print(cot::Model model) override;
  void print(cot::State state) override;
  void print(cot::Duration duration) override;

  void print(cod::Model model) override;
  void print(cod::State state) override;
  void print(cod::Duration duration) override;
  void print(cod::DependencyTree tree) override;
  void print(cod::FeatureFunctionLibrary library) override;

 protected:
  // Instance variables
  bool change_ostream_;

  std::string root_dir_;
  std::string working_dir_;

  std::list<cot::Model> subtrainings_;

  std::list<cod::Model> submodels_;
  std::list<cod::DependencyTree> trees_;
  std::list<cod::FeatureFunctionLibrary> libraries_;

  // Constructors
  template<typename... Args>
  MultipleFilePrinter(bool change_ostream,
                      const std::string &root_dir,
                      Args&&... args);

 private:
  // Concrete methods
  std::string pathForHelperCall(const std::string &path);

  void printSubtraining(cot::Model subtraining);

  void printSubmodel(cod::Model submodel);
  void printTree(cod::DependencyTree tree);
  void printLibrary(cod::FeatureFunctionLibrary library);
};

}  // namespace lang

// Implementation header
#include "lang/MultipleFilePrinter.ipp"

#endif  // LANG_MULTIPLE_FILE_PRINTER_
