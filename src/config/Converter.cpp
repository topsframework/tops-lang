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
#include "config/Converter.hpp"

// Standard headers
#include <memory>
#include <string>
#include <sstream>

namespace config {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

Converter::Converter(const option::Alphabet &alphabet)
  : alphabet_(alphabet), tokens_(createRegex(alphabet)) {
}

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

model::Sequence Converter::convert(std::string orig) {
  model::Sequence conv;
  std::smatch match;

  while (std::regex_search(orig, match, tokens_)) {
    conv.emplace_back(dictionary.at(match.str()));
    orig = match.suffix().str();
  }

  return conv;
}

/*----------------------------------------------------------------------------*/

std::string Converter::createRegex(const option::Alphabet &alphabet) {
  if (alphabet.size() == 0) return {};

  std::string regex_text { alphabet.front() };
  dictionary[alphabet.front()] = 0;

  for (std::size_t i = 1; i < alphabet.size(); ++i) {
    regex_text += "|" + alphabet[i];
    dictionary[alphabet[i]] = i;
  }

  return regex_text;
}

/*----------------------------------------------------------------------------*/

}  // namespace config
