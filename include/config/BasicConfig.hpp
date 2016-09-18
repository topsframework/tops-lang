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

#ifndef CONFIG_BASIC_CONFIG_
#define CONFIG_BASIC_CONFIG_

// Standard headers
#include <memory>
#include <string>
#include <vector>
#include <cstddef>
#include <type_traits>

// Internal headers
#include "ParameterPack.hpp"

#include "config/HasTag.hpp"
#include "config/BasicConfigInterface.hpp"

// External headers
#include "named_types/named_tuple.hpp"

namespace config {

// Forward declarations
class ModelConfigVisitor;

/**
 * @class BasicConfig
 * @brief General class to create an intermediate representations (IR)
 * @tparam Base Base class for the config IR
 * @tparam Options List of Type(decltype("name"_t)) to be stored in config IR
 */
template<typename Base, typename... Options>
class BasicConfig : public Base {
 public:
  // Alias
  using Self = BasicConfig<Base, Options...>;
  using Tuple = named_types::named_tuple<Options...>;

  using SelfPtr = std::shared_ptr<BasicConfig<Base, Options...>>;

  // Constructors
  explicit BasicConfig(const std::string &path = {},
                       const std::string &label = {});

  // Static methods
  template<typename... Params>
  static SelfPtr make(Params&&... params);

  // Overriden methods
  void accept(ModelConfigVisitor &visitor) const override;
  void accept(ModelConfigVisitor &&visitor) const override;

  std::size_t number_of_options() const override;

  // Concrete methods
  template<typename Func>
  constexpr void for_each(Func&& func) const;

  template<typename... Args>
  void initialize(Args&&... args);

  template<class Tag>
  constexpr decltype(auto)
  get(std::enable_if_t<!has_tag<Tag, Options...>()>* = nullptr) const &;

  template<class Tag>
  constexpr decltype(auto)
  get(std::enable_if_t<has_tag<Tag, Options...>()>* = nullptr) const &;

  template<class Tag>
  decltype(auto)
  get(std::enable_if_t<!has_tag<Tag, Options...>()>* = nullptr) &;

  template<class Tag>
  decltype(auto)
  get(std::enable_if_t<has_tag<Tag, Options...>()>* = nullptr) &;

  template<class Tag>
  decltype(auto)
  get(std::enable_if_t<!has_tag<Tag, Options...>()>* = nullptr) &&;

  template<class Tag>
  decltype(auto)
  get(std::enable_if_t<has_tag<Tag, Options...>()>* = nullptr) &&;

  std::vector<SelfPtr> &children();
  const std::vector<SelfPtr> &children() const;

 private:
  // Instance variables
  Tuple attrs_;
  std::vector<SelfPtr> children_;

  // Concrete methods
  template<typename... Args>
  void initialize_base(Args&&... args);

  template<typename... Args>
  void initialize_tuple(Args&&... args);
};

}  // namespace config

// Implementation header
#include "config/BasicConfig.ipp"

#endif  // CONFIG_BASIC_CONFIG_
