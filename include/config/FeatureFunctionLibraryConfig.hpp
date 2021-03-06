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

#ifndef CONFIG_FEATURE_FUNCTION_LIBRARY_CONFIG_
#define CONFIG_FEATURE_FUNCTION_LIBRARY_CONFIG_

// Standard headers
#include <memory>
#include <vector>

// Internal headers
#include "config/ConfigWithOptions.hpp"

#include "config/Options.hpp"

namespace config {

/**
 * @typedef FeatureFunctionLibraryConfig
 * @brief Alias to helper IR of a feature function library
 */
using FeatureFunctionLibraryConfig
  = config_with_options<
      option::Alphabet(decltype("observations"_t)),
      option::Alphabet(decltype("labels"_t)),
      option::FeatureFunctions(decltype("feature_functions"_t))
    >::type;

/**
 * @typedef FeatureFunctionLibraryConfigPtr
 * @brief Alias of pointer to FeatureFunctionLibraryConfig
 */
using FeatureFunctionLibraryConfigPtr
  = std::shared_ptr<FeatureFunctionLibraryConfig>;

}  // namespace config

namespace config {
namespace option {

using FeatureFunctionLibrary = FeatureFunctionLibraryConfigPtr;
using FeatureFunctionLibraries = std::vector<FeatureFunctionLibrary>;

}  // namespace option
}  // namespace config

#endif  // CONFIG_FEATURE_FUNCTION_LIBRARY_CONFIG_
