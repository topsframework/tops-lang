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

// Interface header
#include "config/CustomDomain.hpp"

// Standard headers
#include <memory>
#include <utility>

// Internal headers
#include "config/CustomConverter.hpp"

namespace config {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

CustomDomain::CustomDomain(OutToInSymbolFunction out_to_in,
                           InToOutSymbolFunction in_to_out)
    : data_(std::make_shared<DerivedData>("custom_domain")) {
  std::get<decltype("out_to_in"_t)>(*data_) = std::move(out_to_in);
  std::get<decltype("in_to_out"_t)>(*data_) = std::move(in_to_out);
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

ConverterPtr CustomDomain::makeConverter() const {
  return std::make_shared<CustomConverter>(
      std::get<decltype("out_to_in"_t)>(*data_),
      std::get<decltype("in_to_out"_t)>(*data_));
}

/*----------------------------------------------------------------------------*/

std::shared_ptr<typename CustomDomain::Data> CustomDomain::data() {
  return data_;
}

/*----------------------------------------------------------------------------*/

std::shared_ptr<const typename CustomDomain::Data> CustomDomain::data() const {
  return data_;
}

/*----------------------------------------------------------------------------*/

}  // namespace config
