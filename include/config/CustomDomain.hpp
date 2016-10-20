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

#ifndef CONFIG_CUSTOM_DOMAIN_
#define CONFIG_CUSTOM_DOMAIN_

// Standard headers
#include <memory>

// Internal headers
#include "config/Domain.hpp"
#include "config/Options.hpp"
#include "config/Converter.hpp"

#include "model/Symbol.hpp"

namespace config {

// Forward declarations
class CustomDomain;

/**
 * @typedef CustomDomainPtr
 * @brief Alias of pointer to CustomDomain
 */
using CustomDomainPtr = std::shared_ptr<CustomDomain>;

/**
 * @class CustomDomain
 * @brief Class representing a customly convertible domain of an input sequence
 */
class CustomDomain : public Domain {
 public:
  // Aliases
  using OutToInFunction = std::function<model::Symbol(const option::Symbol&)>;
  using InToOutFunction = std::function<option::Symbol(const model::Symbol&)>;

  // Constructors
  explicit CustomDomain(OutToInFunction out_to_in,
                        InToOutFunction in_to_out);

  // Overriden methods
  ConverterPtr makeConverter() const override;

 private:
  // Instance variables
  OutToInFunction out_to_in_;
  InToOutFunction in_to_out_;
};

}  // namespace config

#endif  // CONFIG_CUSTOM_DOMAIN_
