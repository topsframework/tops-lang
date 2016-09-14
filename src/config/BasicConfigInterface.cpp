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
#include "config/BasicConfigInterface.hpp"

// Standard headers
#include <string>

namespace config {

/*----------------------------------------------------------------------------*/
/*                                CONSTRUCTORS                                */
/*----------------------------------------------------------------------------*/

BasicConfigInterface::BasicConfigInterface(const std::string &path,
                                           const std::string &label)
    : path_(path), label_(label) {
}

/*----------------------------------------------------------------------------*/
/*                              VIRTUAL METHODS                               */
/*----------------------------------------------------------------------------*/

std::string BasicConfigInterface::path() {
  return path_;
}

/*----------------------------------------------------------------------------*/

const std::string BasicConfigInterface::path() const {
  return path_;
}

/*----------------------------------------------------------------------------*/

std::string BasicConfigInterface::label() {
  return label_;
}

/*----------------------------------------------------------------------------*/

const std::string BasicConfigInterface::label() const {
  return label_;
}

/*----------------------------------------------------------------------------*/

std::size_t BasicConfigInterface::number_of_options() const {
  return 0;
}

/*----------------------------------------------------------------------------*/

}  // namespace config
