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

#ifndef CONFIG_TRAINING_PRETRAINED_MODEL_CONFIG_
#define CONFIG_TRAINING_PRETRAINED_MODEL_CONFIG_

// Standard headers
#include <memory>
#include <vector>

// Internal headers
#include "config/ConfigWithOptions.hpp"

#include "config/Options.hpp"
#include "config/training/ModelConfig.hpp"
#include "config/definition/ModelConfig.hpp"

namespace config {
namespace training {

/**
 * @typedef PretrainedModelConfig
 * @brief Alias to IR of a pretrained model::ProbabilisticModel
 */
using PretrainedModelConfig
  = config_with_options<
      option::definition::Model(decltype("pretrained_model"_t))
    >::extending<ModelConfig>::type<class PretrainedModelConfigID>;

/**
 * @typedef PretrainedModelConfigPtr
 * @brief Alias of pointer to PretrainedModelConfig
 */
using PretrainedModelConfigPtr
  = std::shared_ptr<PretrainedModelConfig>;

}  // namespace training
}  // namespace config

namespace config {
namespace option {
namespace training {

using PretrainedModel
  = config::training::PretrainedModelConfigPtr;

}  // namespace training
}  // namespace option
}  // namespace config

#endif  // CONFIG_TRAINING_PRETRAINED_MODEL_CONFIG_
