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

#ifndef CONFIG_DEFINITION_OPTION_
#define CONFIG_DEFINITION_OPTION_

// Internal headers
#include "config/Domain.hpp"
#include "config/Options.hpp"

namespace config {
namespace definition {
namespace option {

using Type = config::option::Type;
using Symbol = config::option::Symbol;
using Pattern = config::option::Pattern;
using Sequence = config::option::Sequence;

using Size = config::option::Size;

using Alphabet = config::option::Alphabet;
using Alphabets = config::option::Alphabets;

using Probability = config::option::Probability;
using Probabilities = config::option::Probabilities;

using FeatureFunction = config::option::FeatureFunction;
using FeatureFunctions = config::option::FeatureFunctions;

using OutToInSymbolFunction = config::option::OutToInSymbolFunction;
using InToOutSymbolFunction = config::option::InToOutSymbolFunction;

using Domain = config::option::Domain;
using Domains = config::option::Domains;

}  // namespace option
}  // namespace definition
}  // namespace config

#endif  // CONFIG_DEFINITION_OPTION_
