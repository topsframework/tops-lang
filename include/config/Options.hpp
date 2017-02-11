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

#ifndef CONFIG_OPTION_
#define CONFIG_OPTION_

// Standard headers
#include <map>
#include <string>
#include <vector>
#include <functional>

// Internal headers
#include "model/Symbol.hpp"

namespace config {
namespace option {

using Size = unsigned int;
using Length = unsigned int;
using Iterations = unsigned int;

using Type = std::string;
using Symbol = std::string;
using Dataset = std::string;
using Pattern = std::string;
using Sequence = std::string;
using Algorithm = std::string;

using Label = std::string;
using Labels = std::vector<Label>;

using Alphabet = std::vector<Symbol>;
using Alphabets = std::vector<Alphabet>;

using Counter = double;
using Threshold = double;

using Probability = double;
using Probabilities = std::map<std::string, Probability>;

using FeatureFunction = std::function<
  double(unsigned int, unsigned int, std::vector<unsigned int>, unsigned int)>;
using FeatureFunctions = std::map<std::string, FeatureFunction>;

using OutToInSymbolFunction
  = std::function<model::Symbol(const option::Symbol&)>;
using InToOutSymbolFunction
  = std::function<option::Symbol(const model::Symbol&)>;

}  // namespace option
}  // namespace config

#endif  // CONFIG_OPTION_
