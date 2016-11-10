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

// Standard headers
#include <memory>

namespace config {

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

template<typename Base, typename... Options>
inline void ModelConfigVisitor::visit(
    std::shared_ptr<BasicConfig<Base, Options...>> config_ptr) {
  std::size_t count = 0;
  std::size_t max = config_ptr->number_of_options();

  this->visitLabel(config_ptr->label());
  this->visitPath(config_ptr->path());

  this->startVisit();
  config_ptr->for_each([this, &count, &max] (const auto &tag, auto &value) {
    using Tag = std::remove_cv_t<std::remove_reference_t<decltype(tag)>>;
    using Value = std::remove_cv_t<std::remove_reference_t<decltype(value)>>;
    this->visitTag(typename Tag::value_type().str(), count, max);
    this->visitOption(const_cast<Value&>(value));
    count++;
  });
  this->endVisit();
}

/*----------------------------------------------------------------------------*/

}  // namespace config
