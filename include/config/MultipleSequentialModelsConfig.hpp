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

#ifndef CONFIG_MULTIPLE_SEQUENTIAL_MODELS_CONFIG_
#define CONFIG_MULTIPLE_SEQUENTIAL_MODELS_CONFIG_

// Internal headers
#include "config/ConfigWithOptions.hpp"

#include "config/ModelConfig.hpp"
#include "config/StateConfig.hpp"

namespace config {

/**
 * @typedef MultipleSequentialModelsConfig
 * @brief Alias to IR of a model::MultipleSequentialModel
 */
using MultipleSequentialModelsConfig
  = config_with_options<
      option::States(decltype("models"_t))
    >::extending<ModelConfig>::type;

/**
 * @typedef MultipleSequentialModelsConfigPtr
 * @brief Alias of pointer to MultipleSequentialModelsConfig
 */
using MultipleSequentialModelsConfigPtr
  = std::shared_ptr<MultipleSequentialModelsConfig>;

}  // namespace config

#endif  // CONFIG_MULTIPLE_SEQUENTIAL_MODELS_CONFIG_
