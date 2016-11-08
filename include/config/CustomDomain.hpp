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
#include "config/BasicConfig.hpp"

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
  // Alias
  using Base = Domain;

  using OutToInSymbolFunction = option::OutToInSymbolFunction;
  using InToOutSymbolFunction = option::InToOutSymbolFunction;

  // Constructors
  explicit CustomDomain(OutToInSymbolFunction out_to_in,
                        InToOutSymbolFunction in_to_out);

  // Overriden methods
  ConverterPtr makeConverter() const override;

  std::shared_ptr<Data> data() override;
  std::shared_ptr<const Data> data() const override;

 protected:
  // Alias
  using Data = typename Base::Data;

  using DerivedData = config_with_options<
    OutToInSymbolFunction(decltype("out_to_in"_t)),
    InToOutSymbolFunction(decltype("in_to_out"_t))
  >::extending<Data>::type;

  // Instance variables
  std::shared_ptr<DerivedData> data_;
};

}  // namespace config

namespace config {
namespace option {

using CustomDomain = CustomDomainPtr;
using CustomDomains = std::vector<CustomDomainPtr>;

}  // namespace option
}  // namespace config

#endif  // CONFIG_CUSTOM_DOMAIN_
