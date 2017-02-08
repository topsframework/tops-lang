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

#ifndef CONFIG_DEFINITION_DEPENDENCY_TREE_CONFIG_
#define CONFIG_DEFINITION_DEPENDENCY_TREE_CONFIG_

// Standard headers
#include <memory>
#include <vector>

// Internal headers
#include "config/ConfigWithOptions.hpp"

#include "config/Options.hpp"
#include "config/definition/ModelConfig.hpp"

namespace config {
namespace definition {

/**
 * @typedef DependencyTreeConfig
 * @brief Alias to helper IR of a dependency tree of a config::MDDConfig
 */
using DependencyTreeConfig
  = config_with_options<
      option::Pattern(decltype("position"_t)),
      option::definition::Model(decltype("configuration"_t))
    >::type;

/**
 * @typedef DependencyTreeConfigPtr
 * @brief Alias of pointer to DependencyTreeConfig
 */
using DependencyTreeConfigPtr = std::shared_ptr<DependencyTreeConfig>;

}  // namespace definition
}  // namespace config

namespace config {
namespace option {
namespace definition {

using DependencyTree = config::definition::DependencyTreeConfigPtr;
using DependencyTrees = std::vector<DependencyTree>;

}  // namespace definition
}  // namespace option
}  // namespace config

#endif  // CONFIG_DEFINITION_DEPENDENCY_TREE_CONFIG_
