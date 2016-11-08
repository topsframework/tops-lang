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
#include "config/DiscreteDomain.hpp"

// Standard headers
#include <memory>
#include <utility>

// Internal headers
#include "config/DiscreteConverter.hpp"

#include <iostream>

namespace config {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

DiscreteDomain::DiscreteDomain(option::Alphabet alphabet)
    : data_(std::make_shared<DerivedData>("", "discrete_domain")) {
  std::get<decltype("alphabet"_t)>(*data_) = std::move(alphabet);
}

/*----------------------------------------------------------------------------*/
/*                             OVERRIDEN METHODS                              */
/*----------------------------------------------------------------------------*/

ConverterPtr DiscreteDomain::makeConverter() const {
  return std::make_shared<DiscreteConverter>(
      std::get<decltype("alphabet"_t)>(*data_));
}

/*----------------------------------------------------------------------------*/

std::shared_ptr<typename DiscreteDomain::Data> DiscreteDomain::data() {
  std::cerr << "Here!!!!" << std::endl;
  return data_;
}

/*----------------------------------------------------------------------------*/

std::shared_ptr<const typename DiscreteDomain::Data>
DiscreteDomain::data() const {
  std::cerr << "Here!!!!" << std::endl;
  return data_;
}

/*----------------------------------------------------------------------------*/

}  // namespace config
