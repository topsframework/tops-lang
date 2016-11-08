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

#ifndef CONFIG_DISCRETE_DOMAIN_
#define CONFIG_DISCRETE_DOMAIN_

// Standard headers
#include <memory>

// Internal headers
#include "config/Domain.hpp"
#include "config/Options.hpp"
#include "config/Converter.hpp"
#include "config/BasicConfig.hpp"

namespace config {

// Forward declarations
class DiscreteDomain;

/**
 * @typedef DiscreteDomainPtr
 * @brief Alias of pointer to DiscreteDomain
 */
using DiscreteDomainPtr = std::shared_ptr<DiscreteDomain>;

/**
 * @class DiscreteDomain
 * @brief Class representing a discrete domain of an input sequence
 */
class DiscreteDomain : public Domain {
 public:
  // Alias
  using Base = Domain;

  // Constructors
  DiscreteDomain(option::Alphabet alphabet);

  // Overriden methods
  ConverterPtr makeConverter() const override;

  std::shared_ptr<Data> data() override;
  std::shared_ptr<const Data> data() const override;

 protected:
  // Alias
  using Data = typename Base::Data;

  using DerivedData = config_with_options<
    option::Alphabet(decltype("alphabet"_t))
  >::extending<Data>::type;

  // Instance variables
  std::shared_ptr<DerivedData> data_;
};

}  // namespace config

namespace config {
namespace option {

using DiscreteDomain = DiscreteDomainPtr;
using DiscreteDomains = std::vector<DiscreteDomainPtr>;

}  // namespace option
}  // namespace config

#endif  // CONFIG_DISCRETE_DOMAIN_
