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

#ifndef CONFIG_LCCRF_CONFIG_
#define CONFIG_LCCRF_CONFIG_

// Standard headers
#include <memory>

// Internal headers
#include "config/ConfigWithOptions.hpp"

#include "config/Options.hpp"
#include "config/DecodableModelConfig.hpp"
#include "config/FeatureFunctionLibraryConfig.hpp"

namespace config {

/**
 * @typedef LCCRFConfig
 * @brief Alias to IR of a model::LinearChainConditionalRandomField
 */
using LCCRFConfig
  = config_with_options<
      option::Probabilities(decltype("feature_parameters"_t)),
      option::FeatureFunctionLibraries(decltype("feature_function_libraries"_t))
    >::extending<DecodableModelConfig>::type;

/**
 * @typedef LCCRFConfigPtr
 * @brief Alias of pointer to LCCRFConfig
 */
using LCCRFConfigPtr = std::shared_ptr<LCCRFConfig>;

}  // namespace config

#endif  // CONFIG_LCCRF_CONFIG_
