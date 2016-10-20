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
  // Constructors
  DiscreteDomain(option::Alphabet alphabet);

  // Overriden methods
  ConverterPtr makeConverter() const override;

 private:
  // Instance variables
  option::Alphabet alphabet_;
};

}  // namespace config

#endif  // CONFIG_DISCRETE_DOMAIN_
