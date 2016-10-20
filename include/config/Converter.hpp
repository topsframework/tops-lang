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

#ifndef CONFIG_CONVERTER_
#define CONFIG_CONVERTER_

// Standard headers
#include <map>
#include <regex>
#include <memory>
#include <string>

// Internal headers
#include "config/Options.hpp"

#include "model/Symbol.hpp"
#include "model/Sequence.hpp"

namespace config {

// Forward declarations
class Converter;

/**
 * @typedef ConverterPtr
 * @brief Alias of pointer to Converter
 */
using ConverterPtr = std::shared_ptr<Converter>;

/**
 * @class Converter
 * @brief Class to convert a std::string of an alphabet in a model::Sequence
 */
class Converter {
 public:
  // Constructors
  explicit Converter(const option::Alphabet &alphabet);

  // Concrete methods
  model::Sequence convert(std::string orig);

 private:
  // Instance variables
  option::Alphabet alphabet_;
  std::map<option::Letter, model::Symbol> dictionary_;
  std::regex tokens_;

  // Concrete methods
  std::string createRegex(const option::Alphabet &alphabet);
};

}  // namespace config

#endif  // CONFIG_CONVERTER_
