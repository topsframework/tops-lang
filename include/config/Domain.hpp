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
#include <iostream>

// Internal headers
#include "config/ConfigWithOptions.hpp"

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
  // Alias
  using Data = config_with_options<>::type;

  // Virtual methods
  virtual ConverterPtr makeConverter() const { return nullptr; }

  virtual std::shared_ptr<Data> data() { std::cerr << "In base" << std::endl; return nullptr; }
  virtual std::shared_ptr<const Data> data() const { std::cerr << "In base" << std::endl; return nullptr; }

  // Destructor
  virtual ~Domain() = default;
};

}  // namespace config

namespace config {
namespace option {

using Domain = DomainPtr;
using Domains = std::vector<DomainPtr>;

using OutToInSymbolFunction
  = std::function<model::Symbol(const option::Symbol&)>;
using InToOutSymbolFunction
  = std::function<option::Symbol(const model::Symbol&)>;

}  // namespace option
}  // namespace config

#endif  // CONFIG_DOMAIN_
