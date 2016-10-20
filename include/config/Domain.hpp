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

#ifndef CONFIG_DOMAIN_
#define CONFIG_DOMAIN_

// Standard headers
#include <memory>

// Internal headers
#include "config/Converter.hpp"

namespace config {

// Forward declarations
class Domain;

/**
 * @typedef DomainPtr
 * @brief Alias of pointer to Domain
 */
using DomainPtr = std::shared_ptr<Domain>;

/**
 * @class Domain
 * @brief Class representing a domain of an input sequence
 */
class Domain {
 public:
  // Purely virtual methods
  virtual ConverterPtr makeConverter() const = 0;
};

}  // namespace config

#endif  // CONFIG_DOMAIN_
