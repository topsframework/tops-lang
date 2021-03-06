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

#ifndef LANG_FILE_PRINTER_
#define LANG_FILE_PRINTER_

// Standard header
#include <iosfwd>
#include <memory>
#include <string>
#include <algorithm>
#include <functional>
#include <type_traits>

// Internal headers
#include "lang/Util.hpp"

#include "config/Domain.hpp"
#include "config/BasicConfig.hpp"

#include "config/ModelConfig.hpp"
#include "config/StateConfig.hpp"
#include "config/DurationConfig.hpp"
#include "config/DependencyTreeConfig.hpp"
#include "config/FeatureFunctionLibraryConfig.hpp"

namespace lang {

/**
 * @class FilePrinter
 * @brief Base class to print config::BasicConfig derived classes in files
 */
class FilePrinter {
 public:
  // Inner structs
  struct Option {
    std::string attribution = " = ";
    std::string middle = "\n\n";
    std::string end = "\n";
    std::string string_start = "\"";
    std::string string_end = "\"";
  };

  // Purely virtual methods
  virtual void print(config::ModelConfigPtr config_ptr) = 0;
  virtual void print(config::StateConfigPtr state_ptr) = 0;
  virtual void print(config::DurationConfigPtr duration_ptr) = 0;
  virtual void print(config::FeatureFunctionLibraryConfigPtr library_ptr) = 0;
  virtual void print(config::DependencyTreeConfigPtr tree_ptr) = 0;

  virtual void print(config::DomainPtr domain_ptr) = 0;

  // Virtual methods
  virtual void changeOstream(const std::string &/* path */);

  virtual void startPrinting();
  virtual void endPrinting();

  // Concrete methods
  void print(const std::string &string);
  void print(float num);
  void print(double num);
  void print(unsigned int num);
  void print(bool boolean);

  template<typename Return, typename... Params>
  void print(const std::function<Return(Params...)> &/* func */);

  template<typename Pair>
  auto print(const Pair &pair)
      -> std::enable_if_t<is_pair<Pair>::value>;

  template<typename Iterable>
  auto print(const Iterable &iterable)
      -> std::enable_if_t<is_iterable<Iterable>::value>;

  void printTag(const std::string &tag, std::size_t count,
                                        std::size_t max);

  // Destructor
  virtual ~FilePrinter() = default;

 protected:
  // Instance variables
  std::shared_ptr<std::ostream> os_;
  unsigned int depth_;

  // Constructors
  template<typename... OptionArgs>
  FilePrinter(std::shared_ptr<std::ostream> os,
              unsigned int initial_depth,
              OptionArgs&&... option_args);

  // Concrete methods
  void callFunction(const std::string &name, const std::string &content);

  void openFunction(const std::string name);
  void closeFunction();

  void openSection(char separator);
  void closeSection(char separator);

  void indent(unsigned int extra_depth = 0);

  template<typename Base, typename... Options>
  void copy(std::shared_ptr<config::BasicConfig<Base, Options...>> config_ptr,
            std::shared_ptr<std::ostream> os);

 private:
  // Instance variables
  const Option option_;
};

}  // namespace lang

// Implementation header
#include "lang/FilePrinter.ipp"

#endif  // LANG_FILE_PRINTER_
