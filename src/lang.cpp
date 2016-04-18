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
#include <map>
#include <list>
#include <regex>
#include <memory>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <utility>
#include <iostream>
#include <iterator>
#include <typeinfo>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <functional>
#include <type_traits>

// Internal headers
#include "ParameterPack.hpp"
#include "filesystem/Filesystem.hpp"

// External headers
#include "chaiscript/chaiscript.hpp"
#include "chaiscript/dispatchkit/bootstrap_stl.hpp"

#include "named_types/named_tuple.hpp"
#include "named_types/extensions/type_traits.hpp"

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                  HELPER TRAITS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

namespace config {

template<typename T>
struct delayed_true : public std::true_type {};

template<typename T>
struct delayed_false : public std::false_type {};

}  // namespace config

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                 CONFIG FACTORY
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

namespace config {

// Forward declaration
class BasicConfigInterface;
template<typename Base, typename... Options> class BasicConfig;

template<typename... Options>
struct config_with_options {
  using type = BasicConfig<BasicConfigInterface, Options...>;

  template<typename T>
  struct extending {
    static_assert(delayed_false<T>::value, "Is not a configuration");
  };

  template<typename BaseBase, typename... BaseOptions>
  struct extending<BasicConfig<BaseBase, BaseOptions...>> {
    using type
      = BasicConfig<BasicConfig<BaseBase, BaseOptions...>, Options...>;
  };
};

}  // namespace config

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                     CONFIGS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

template<typename T, T... chars>
constexpr decltype(auto) operator ""_t () {
  return named_types::named_tag<named_types::string_literal<T, chars...>>{};
}

namespace config {

/*----------------------------------------------------------------------------*/

namespace option {

using Type = std::string;
using Letter = std::string;
using Alphabet = std::vector<Letter>;

}  // namespace option

/*----------------------------------------------------------------------------*/

using ModelConfig
  = config_with_options<
      option::Type(decltype("model_type"_t)),
      option::Alphabet(decltype("observations"_t))
    >::type;

using ModelConfigPtr = std::shared_ptr<ModelConfig>;

/*----------------------------------------------------------------------------*/

using DurationConfig
  = config_with_options<>::type;

using DurationConfigPtr = std::shared_ptr<DurationConfig>;

/*----------------------------------------------------------------------------*/

using GeometricDurationConfig
  = config_with_options<>::extending<DurationConfig>::type;

using GeometricDurationConfigPtr = std::shared_ptr<GeometricDurationConfig>;

/*----------------------------------------------------------------------------*/

namespace option {

using Size = unsigned int;

}  // namespace option

/*----------------------------------------------------------------------------*/

using MaxLenghtConfig
  = config_with_options<
      option::Size(decltype("size"_t))
    >::extending<DurationConfig>::type;

using MaxLenghtConfigPtr = std::shared_ptr<MaxLenghtConfig>;

/*----------------------------------------------------------------------------*/

using SignalDurationConfig
  = config_with_options<
      option::Size(decltype("size"_t))
    >::extending<DurationConfig>::type;

using SignalDurationConfigPtr = std::shared_ptr<SignalDurationConfig>;

/*----------------------------------------------------------------------------*/


namespace option {

using Model = ModelConfigPtr;

}  // namespace option

/*----------------------------------------------------------------------------*/

using ExplicitDurationConfig
  = config_with_options<
      option::Model(decltype("model"_t)),
      option::Size(decltype("max_size"_t))
    >::extending<DurationConfig>::type;

using ExplicitDurationConfigPtr = std::shared_ptr<ExplicitDurationConfig>;

/*----------------------------------------------------------------------------*/

namespace option {

using Duration = DurationConfigPtr;

}  // namespace option

/*----------------------------------------------------------------------------*/

using StateConfig
  = config_with_options<
      option::Duration(decltype("duration"_t)),
      option::Model(decltype("emission"_t))
    >::type;

using StateConfigPtr = std::shared_ptr<StateConfig>;

/*----------------------------------------------------------------------------*/

namespace option {

using Probability = double;
using Probabilities = std::map<std::string, Probability>;

}  // namespace option

/*----------------------------------------------------------------------------*/

using IIDConfig
  = config_with_options<
      option::Probabilities(decltype("emission_probabilities"_t))
    >::extending<ModelConfig>::type;

using IIDConfigPtr = std::shared_ptr<IIDConfig>;

/*----------------------------------------------------------------------------*/

using VLMCConfig
  = config_with_options<
      option::Probabilities(decltype("context_probabilities"_t))
    >::extending<ModelConfig>::type;

using VLMCConfigPtr = std::shared_ptr<VLMCConfig>;

/*----------------------------------------------------------------------------*/

namespace option {

using Models = std::vector<ModelConfigPtr>;

}  // namespace option

/*----------------------------------------------------------------------------*/

using IMCConfig
  = config_with_options<
      option::Models(decltype("position_specific_distributions"_t))
    >::extending<ModelConfig>::type;

using IMCConfigPtr = std::shared_ptr<IMCConfig>;

/*----------------------------------------------------------------------------*/

using PeriodicIMCConfig
  = config_with_options<
      option::Models(decltype("position_specific_distributions"_t))
    >::extending<ModelConfig>::type;

using PeriodicIMCConfigPtr = std::shared_ptr<PeriodicIMCConfig>;

/*----------------------------------------------------------------------------*/

namespace option {

using State = StateConfigPtr;
using States = std::map<std::string, State>;

}  // namespace option

/*----------------------------------------------------------------------------*/

using GHMMConfig
  = config_with_options<
      option::Probabilities(decltype("initial_probabilities"_t)),
      option::Probabilities(decltype("transition_probabilities"_t)),
      option::States(decltype("states"_t))
    >::extending<ModelConfig>::type;

using GHMMConfigPtr = std::shared_ptr<GHMMConfig>;

/*----------------------------------------------------------------------------*/

using HMMConfig
  = config_with_options<
      option::Probabilities(decltype("initial_probabilities"_t)),
      option::Probabilities(decltype("transition_probabilities"_t)),
      option::Probabilities(decltype("emission_probabilities"_t)),
      option::Alphabet(decltype("labels"_t))
    >::extending<ModelConfig>::type;

using HMMConfigPtr = std::shared_ptr<HMMConfig>;

/*----------------------------------------------------------------------------*/

namespace option {

using Sequence = std::string;

}  // namespace option

/*----------------------------------------------------------------------------*/

using SBSWConfig
  = config_with_options<
      option::Probabilities(decltype("sequences"_t)),
      option::Probability(decltype("normalizer"_t)),
      option::Size(decltype("skip_offset"_t)),
      option::Size(decltype("skip_length"_t)),
      option::Sequence(decltype("skip_sequence"_t))
    >::extending<ModelConfig>::type;

using SBSWConfigPtr = std::shared_ptr<SBSWConfig>;

/*----------------------------------------------------------------------------*/

using MultipleSequentialModelsConfig
  = config_with_options<
      option::States(decltype("models"_t))
    >::extending<ModelConfig>::type;

using MultipleSequentialModelsConfigPtr
  = std::shared_ptr<MultipleSequentialModelsConfig>;

/*----------------------------------------------------------------------------*/

namespace option {

using FeatureFunction = std::function<
  double(unsigned int, unsigned int, std::vector<unsigned int>, unsigned int)>;
using FeatureFunctions = std::map<std::string, FeatureFunction>;

}  // namespace option

/*----------------------------------------------------------------------------*/

using FeatureFunctionLibraryConfig
  = config_with_options<
      option::Alphabet(decltype("observations"_t)),
      option::Alphabet(decltype("labels"_t)),
      option::FeatureFunctions(decltype("feature_functions"_t))
    >::type;

using FeatureFunctionLibraryConfigPtr
  = std::shared_ptr<FeatureFunctionLibraryConfig>;

/*----------------------------------------------------------------------------*/

namespace option {

using FeatureFunctionLibrary = FeatureFunctionLibraryConfigPtr;
using FeatureFunctionLibraries = std::vector<FeatureFunctionLibrary>;

}  // namespace option

/*----------------------------------------------------------------------------*/

using LCCRFConfig
  = config_with_options<
      option::Alphabet(decltype("labels"_t)),
      option::Probabilities(decltype("feature_parameters"_t)),
      option::FeatureFunctionLibraries(decltype("feature_function_libraries"_t))
    >::extending<ModelConfig>::type;

using LCCRFConfigPtr = std::shared_ptr<LCCRFConfig>;

}  // namespace config

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                    VISITOR
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

namespace config {

class ModelConfigVisitor {
 public:
  // Concrete methods
  template<typename Base, typename... Options>
  void visit(std::shared_ptr<BasicConfig<Base, Options...>> config_ptr) {
    std::size_t count = 0;
    std::size_t max = config_ptr->number_of_options();
    this->startVisit();
    this->visitLabel(config_ptr->label());
    this->visitPath(config_ptr->path());
    config_ptr->for_each([this, &count, &max](const auto &tag, auto &value) {
      using Tag = std::remove_cv_t<std::remove_reference_t<decltype(tag)>>;
      using Value = std::remove_cv_t<std::remove_reference_t<decltype(value)>>;
      this->visitTag(typename Tag::value_type().str(), count, max);
      this->visitOption(const_cast<Value&>(value));
      count++;
    });
    this->endVisit();
  }

  // Virtual destructor
  virtual ~ModelConfigVisitor() = default;

 protected:
  // Purely virtual methods
  virtual void startVisit() = 0;
  virtual void endVisit() = 0;

  virtual void visitOption(option::Model &) = 0;
  virtual void visitOption(option::State &) = 0;
  virtual void visitOption(option::Duration &) = 0;
  virtual void visitOption(option::FeatureFunctionLibrary &) = 0;

  virtual void visitOption(option::Models &) = 0;
  virtual void visitOption(option::States &) = 0;
  virtual void visitOption(option::FeatureFunctionLibraries &) = 0;

  virtual void visitOption(option::Type &) = 0;
  virtual void visitOption(option::Size &) = 0;
  virtual void visitOption(option::Alphabet &) = 0;
  virtual void visitOption(option::Probability &) = 0;
  virtual void visitOption(option::Probabilities &) = 0;
  virtual void visitOption(option::FeatureFunctions &) = 0;

  virtual void visitTag(const std::string &, std::size_t, std::size_t) = 0;
  virtual void visitLabel(const std::string &) = 0;
  virtual void visitPath(const std::string &) = 0;
};

}  // namespace config

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                             BASIC CONFIG INTERFACE
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

namespace config {

class BasicConfigInterface
    : public std::enable_shared_from_this<BasicConfigInterface> {
 public:
  // Alias
  using Self = BasicConfigInterface;

  // Constructors
  explicit BasicConfigInterface(const std::string &path,
                                const std::string &label)
    : path_(path), label_(label) {
  }

  // Purely virtual methods
  virtual void accept(ModelConfigVisitor &/* visitor */) const = 0;
  virtual void accept(ModelConfigVisitor &&/* visitor */) const = 0;

  // Virtual methods
  virtual std::string path() {
    return path_;
  }

  virtual const std::string path() const {
    return path_;
  }

  virtual std::string label() {
    return label_;
  }

  virtual const std::string label() const {
    return label_;
  }

  virtual std::size_t number_of_options() const {
    return 0;
  }

  // Concrete methods
  template<typename Func>
  constexpr void for_each(Func&& /* func */) const {}

  template<typename... Args>
  void initialize(Args&&... /* args */) const {}

  template<class Tag>
  inline constexpr decltype(auto) get() const {}

  // Destructor
  virtual ~BasicConfigInterface() = default;

 private:
  std::string path_;
  std::string label_;
};

}  // namespace config

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                           BASIC CONFIG IMPLEMENTATION
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

namespace config {

template <class Tag, class... Types>
inline constexpr bool has_tag() {
  bool value = false;
  using swallow = bool[];
  (void) swallow {
    bool{},
    (value = value || std::is_same<
      Tag, named_types::__ntuple_tag_spec_t<Types>>::value)... };
  return value;
}

template<typename Base, typename... Options>
class BasicConfig : public Base {
 public:
  // Alias
  using Self = BasicConfig<Base, Options...>;
  using Tuple = named_types::named_tuple<Options...>;

  // Constructors
  explicit BasicConfig(const std::string &path = {},
                       const std::string &label = {})
      : Base(path, label) {
  }

  // Overriden methods
  void accept(ModelConfigVisitor &visitor) const override {
    auto ptr = std::static_pointer_cast<Self>(
      const_cast<Self*>(this)->shared_from_this());
    visitor.visit(ptr);
  }

  void accept(ModelConfigVisitor &&visitor) const override {
    auto ptr = std::static_pointer_cast<Self>(
      const_cast<Self*>(this)->shared_from_this());
    visitor.visit(ptr);
  }

  std::size_t number_of_options() const override {
    return this->Base::number_of_options()
      + std::tuple_size<decltype(
          named_types::forward_as_concatenated_tuple(attrs_))>::value;
  }

  // Concrete methods
  template<typename Func>
  constexpr void for_each(Func&& func) const {
    this->Base::for_each(func);
    named_types::for_each(func, attrs_);
  }

  template<typename... Args>
  void initialize(Args&&... args) {
    static_assert(sizeof...(Args) >= sizeof...(Options),
      "Must have least as many arguments as options in `initialize`");

    initialize_base(std::forward<Args>(args)...);
    initialize_tuple(std::forward<Args>(args)...);
  }

  template<class Tag> inline constexpr decltype(auto) get(
      std::enable_if_t<!has_tag<Tag, Options...>()>* = nullptr) const & {
    return this->Base::template get<Tag>();
  };

  template<class Tag> inline constexpr decltype(auto) get(
      std::enable_if_t<has_tag<Tag, Options...>()>* = nullptr) const & {
    return std::get<Tag>(attrs_);
  };

  template<class Tag> inline decltype(auto) get(
      std::enable_if_t<!has_tag<Tag, Options...>()>* = nullptr) & {
    return this->Base::template get<Tag>();
  };

  template<class Tag> inline decltype(auto) get(
      std::enable_if_t<has_tag<Tag, Options...>()>* = nullptr) & {
    return std::get<Tag>(attrs_);
  };

  template<class Tag> inline decltype(auto) get(
      std::enable_if_t<!has_tag<Tag, Options...>()>* = nullptr) && {
    return this->Base::template get<Tag>();
  };

  template<class Tag> inline decltype(auto) get(
      std::enable_if_t<has_tag<Tag, Options...>()>* = nullptr) && {
    return std::get<Tag>(std::move(attrs_));
  };

 private:
  // Instance variables
  Tuple attrs_;

  // Concrete methods
  template<typename... Args>
  void initialize_base(Args&&... args) {
    forward_subpack(
      [this](auto&&... types) {
        this->Base::initialize(std::forward<decltype(types)>(types)...); },
      index_range<0, sizeof...(Args) - sizeof...(Options)>(),
      std::forward<Args>(args)...);
  }

  template<typename... Args>
  void initialize_tuple(Args&&... args) {
    forward_subpack(
      [this](auto&&... types) {
        attrs_ = Tuple(std::forward<decltype(types)>(types)...); },
      index_range<sizeof...(Args) - sizeof...(Options), sizeof...(Args)>(),
      std::forward<Args>(args)...);
  }
};

}  // namespace config

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                  GET OVERLOAD
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

namespace std {

template<typename Tag, typename Base, typename... Options>
decltype(auto) get(config::BasicConfig<Base, Options...> const &input) {
  return input.template get<Tag>();
}

template<typename Tag, typename Base, typename... Options>
decltype(auto) get(config::BasicConfig<Base, Options...> &input) {
  return input.template get<Tag>();
}

template<typename Tag, typename Base, typename... Options>
decltype(auto) get(config::BasicConfig<Base, Options...> &&input) {
  return move(input).template get<Tag>();
}

}  // namespace std

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                    SEQUENCE
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

namespace model {

using Symbol = unsigned int;
using Sequence = std::vector<Symbol>;

}  // namespace model

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                    CONVERTER
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

namespace config {

class Converter {
 public:
  // Constructors
  explicit Converter(const option::Alphabet &alphabet)
    : alphabet_(alphabet), tokens_(createRegex(alphabet)) {
  }

  // Concrete methods
  model::Sequence convert(std::string orig) {
    model::Sequence conv;
    std::smatch match;

    while (std::regex_search(orig, match, tokens_)) {
      conv.emplace_back(map.at(match.str()));
      orig = match.suffix().str();
    }

    return conv;
  }

 private:
  // Instance variables
  option::Alphabet alphabet_;
  std::map<option::Letter, model::Symbol> map;
  std::regex tokens_;

  // Concrete methods
  std::string createRegex(const option::Alphabet &alphabet) {
    if (alphabet.size() == 0) return {};

    std::string regex_text { alphabet.front() };
    map[alphabet.front()] = 0;

    for (std::size_t i = 1; i < alphabet.size(); ++i) {
      regex_text += "|" + alphabet[i];
      map[alphabet[i]] = i;
    }

    return regex_text;
  }
};

using ConverterPtr = std::shared_ptr<Converter>;

}  // namespace config

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                 PRINTER HELPERS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

namespace lang {

namespace detail {

// To allow ADL with custom begin/end and custom to_string
using std::begin;
using std::end;
using std::to_string;

// is_iterable

template<typename T>
auto is_iterable_impl(void * = nullptr)
    -> decltype(begin(std::declval<T>()) != end(std::declval<T>()),
                ++std::declval<decltype(begin(std::declval<T>()))>(),
                *begin(std::declval<T>()),
                std::true_type{});

template<typename T>
std::false_type is_iterable_impl(...);

// is_pair

template<typename>
struct is_pair_impl : std::false_type {};

template<typename F, typename S>
struct is_pair_impl<std::pair<F, S>> : std::true_type {};

}  // namespace detail

template <typename T>
using is_iterable = decltype(detail::is_iterable_impl<T>(nullptr));

template <typename T>
using is_pair = detail::is_pair_impl<T>;

// Path manipulation

std::string extractRoot(const std::string &filepath) {
  auto found = filepath.find_first_of("/\\");
  return filepath.substr(0, found + 1);
}

std::string extractCorename(const std::string &filepath) {
  auto found = filepath.find_first_of("/\\");
  return filepath.substr(found + 1);
}

std::string extractDir(const std::string &filepath) {
  auto found = filepath.find_last_of("/\\");
  return filepath.substr(0, found + 1);
}

std::string extractBasename(const std::string &filepath) {
  auto found = filepath.find_last_of("/\\");
  return filepath.substr(found + 1);
}

}  // namespace lang

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                 PRINTER VISITOR
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

namespace lang {

class ModelConfigPrinter : public config::ModelConfigVisitor {
 public:
  // Constructors
  explicit ModelConfigPrinter(std::ostream &os, unsigned int depth = 0,
                              std::string option_attribution = " = ",
                              std::string option_middle = "\n\n",
                              std::string option_end = "\n")
      : os_(os), depth_(depth), initial_depth_(depth),
        option_attribution_(option_attribution),
        option_middle_(option_middle),
        option_end_(option_end) {
  }

 protected:
  // Overriden functions
  void startVisit() override {
  }

  void endVisit() override {
    os_ << option_end_;
  }

  void visitOption(config::option::Model &visited) override {
    print(visited);
  }

  void visitOption(config::option::State &visited) override {
    print(visited);
  }

  void visitOption(config::option::Duration &visited) override {
    print(visited);
  }

  void visitOption(config::option::FeatureFunctionLibrary &visited) override {
    print(visited);
  }

  void visitOption(config::option::Models &visited) override {
    print(visited);
  }

  void visitOption(config::option::States &visited) override {
    print(visited);
  }

  void visitOption(config::option::FeatureFunctionLibraries &visited) override {
    print(visited);
  }

  void visitOption(config::option::Type &visited) override {
    print(visited);
  }

  void visitOption(config::option::Size &visited) override {
    print(visited);
  }

  void visitOption(config::option::Alphabet &visited) override {
    print(visited);
  }

  void visitOption(config::option::Probability &visited) override {
    print(visited);
  }

  void visitOption(config::option::Probabilities &visited) override {
    print(visited);
  }

  void visitOption(config::option::FeatureFunctions &visited) override {
    print(visited);
  }

  void visitTag(const std::string &tag, std::size_t count,
                                        std::size_t max) override {
    if (count > 0 && count < max) {
      os_ << option_middle_;
    }
    if (!option_attribution_.empty()) {
      indent();
      os_ << tag << option_attribution_;
    }
  }

  void visitLabel(const std::string &/* label */) override {
  }

  void visitPath(const std::string &/* path */) override {
  }

 private:
  // Instance variables
  std::ostream &os_;
  unsigned int depth_;

  const unsigned int initial_depth_;

  const std::string option_attribution_;
  const std::string option_middle_;
  const std::string option_end_;

  // Concrete methods
  void print(const std::string &string) {
    os_ << '"' << string << '"';
  }

  void print(float num) {
    os_ << num;
  }

  void print(double num) {

    os_ << num;
  }

  void print(unsigned int num) {
    os_ << num;
  }

  void print(bool boolean) {
    os_ << std::boolalpha << boolean;
  }

  template<typename Return, typename... Params>
  auto print(const std::function<Return(Params...)> &/* function */) {
    os_ << "fun()";
  }

  template<typename Pair>
  auto print(const Pair &pair)
      -> std::enable_if_t<is_pair<Pair>::value> {
    print(pair.first);
    os_ << ": ";
    print(pair.second);
  }

  template<typename Iterable>
  auto print(const Iterable &iterable)
      -> std::enable_if_t<is_iterable<Iterable>::value> {
    open_iterable();
    for (auto it = std::begin(iterable); it != std::end(iterable); ++it) {
      indent();
      print(*it);
      os_ << (it == std::prev(std::end(iterable)) ? "" : ",") << "\n";
    }
    close_iterable();
  }

  void open_iterable() {
    os_ << "[ " << "\n";
    depth_++;
  }

  void close_iterable() {
    depth_--;
    indent();
    os_ << "]";
  }

  void indent() {
    std::fill_n(std::ostreambuf_iterator<char>(os_), 2*depth_, ' ');
  }

  void print(config::ModelConfigPtr config_ptr) {
    os_ << "{ " << "\n";
    depth_++;
    config_ptr->accept(ModelConfigPrinter(os_, depth_));
    depth_--;
    indent();
    os_ << "}";
  }

  void print(config::StateConfigPtr state_ptr) {
    os_ << "[ " << "\n";
    depth_++;
    state_ptr->accept(ModelConfigPrinter(os_, depth_, ": ", ",\n\n"));
    depth_--;
    indent();
    os_ << "]";
  }

  void print(config::DurationConfigPtr duration_ptr) {
    os_ << "(";
    duration_ptr->accept(ModelConfigPrinter(os_, depth_, "", ", ", ""));
    os_ << ")";
  }

  void print(config::FeatureFunctionLibraryConfigPtr config_ptr) {
    os_ << "{ " << "\n";
    depth_++;
    config_ptr->accept(ModelConfigPrinter(os_, depth_));
    depth_--;
    indent();
    os_ << "}";
  }
};

}  // namespace lang

template<typename... Ts>
std::ostream &operator<<(std::ostream &os,
                         const config::BasicConfig<Ts...> &config) {
  auto visitor = std::make_shared<lang::ModelConfigPrinter>(os);
  config.accept(*std::static_pointer_cast<config::ModelConfigVisitor>(visitor));
  return os;
}

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                               SERIALIZER VISITOR
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

namespace lang {

class ModelConfigSerializer : public config::ModelConfigVisitor {
 public:
  // Constructors
  explicit ModelConfigSerializer(const std::string &root_dir,
                                 std::string option_attribution = " = ",
                                 std::string option_middle = "\n\n",
                                 std::string option_end = "\n")
      : root_dir_(root_dir), os_(), depth_(0),
        option_attribution_(option_attribution),
        option_middle_(option_middle),
        option_end_(option_end) {
  }

 protected:
  // Overriden functions
  void startVisit() override {
  }

  void endVisit() override {
    os_ << option_end_;

    for (auto &submodel : submodels_) {
      submodel->accept(ModelConfigSerializer(root_dir_));
    }
    for (auto &library : libraries_) {
      std::ifstream src(
        library->path(), std::ios::binary);
      std::ofstream dst(
        root_dir_ + extractCorename(library->path()), std::ios::binary);
      dst << src.rdbuf();
    }
  }

  void visitOption(config::option::Model &visited) override {
    print(visited);
  }

  void visitOption(config::option::State &visited) override {
    print(visited);
  }

  void visitOption(config::option::Duration &visited) override {
    print(visited);
  }

  void visitOption(config::option::FeatureFunctionLibrary &visited) override {
    print(visited);
  }

  void visitOption(config::option::Models &visited) override {
    print(visited);
  }

  void visitOption(config::option::States &visited) override {
    print(visited);
  }

  void visitOption(config::option::FeatureFunctionLibraries &visited) override {
    print(visited);
  }

  void visitOption(config::option::Type &visited) override {
    print(visited);
  }

  void visitOption(config::option::Size &visited) override {
    print(visited);
  }

  void visitOption(config::option::Alphabet &visited) override {
    print(visited);
  }

  void visitOption(config::option::Probability &visited) override {
    print(visited);
  }

  void visitOption(config::option::Probabilities &visited) override {
    print(visited);
  }

  void visitOption(config::option::FeatureFunctions &visited) override {
    print(visited);
  }

  void visitTag(const std::string &tag, std::size_t count,
                                        std::size_t max) override {
    if (count > 0 && count < max) {
      os_ << option_middle_;
    }
    if (!option_attribution_.empty()) {
      indent();
      os_ << tag << option_attribution_;
    }
  }

  void visitLabel(const std::string &/* label */) override {
  }

  void visitPath(const std::string &path) override {
    auto new_path = root_dir_ + extractCorename(path);
    filesystem::create_directories(extractDir(new_path));
    os_ = std::ofstream(new_path);
  }

 private:
  // Instance variables
  std::string root_dir_;

  std::ofstream os_;
  unsigned int depth_;

  const std::string option_attribution_;
  const std::string option_middle_;
  const std::string option_end_;

  std::list<config::ModelConfigPtr> submodels_;
  std::list<config::FeatureFunctionLibraryConfigPtr> libraries_;

  // Concrete methods
  void print(const std::string &string) {
    os_ << '"' << string << '"';
  }

  void print(float num) {
    os_ << num;
  }

  void print(double num) {
    os_ << num;
  }

  void print(unsigned int num) {
    os_ << num;
  }

  void print(bool boolean) {
    os_ << std::boolalpha << boolean;
  }

  template<typename Return, typename... Params>
  auto print(const std::function<Return(Params...)> &/* function */) {
    os_ << "fun()";
  }

  template<typename Pair>
  auto print(const Pair &pair)
      -> std::enable_if_t<is_pair<Pair>::value> {
    print(pair.first);
    os_ << ": ";
    print(pair.second);
  }

  template<typename Iterable>
  auto print(const Iterable &iterable)
      -> std::enable_if_t<is_iterable<Iterable>::value> {
    open_iterable();
    for (auto it = std::begin(iterable); it != std::end(iterable); ++it) {
      indent();
      print(*it);
      os_ << (it == std::prev(std::end(iterable)) ? "" : ",") << "\n";
    }
    close_iterable();
  }

  void open_iterable() {
    os_ << "[ " << "\n";
    depth_++;
  }

  void close_iterable() {
    depth_--;
    indent();
    os_ << "]";
  }

  void indent() {
    std::fill_n(std::ostreambuf_iterator<char>(os_), 2*depth_, ' ');
  }

  void print(config::ModelConfigPtr config_ptr) {
    submodels_.push_back(config_ptr);
    os_ << "model(\"" << extractBasename(config_ptr->path()) << "\")";
  }

  void print(config::StateConfigPtr state_ptr) {
    os_ << "[ " << "\n";
    depth_++;
    state_ptr->accept(ModelConfigPrinter(os_, depth_, ": ", ",\n\n"));
    depth_--;
    indent();
    os_ << "]";
  }

  void print(config::DurationConfigPtr duration_ptr) {
    os_ << "(";
    duration_ptr->accept(ModelConfigPrinter(os_, depth_, "", ", ", ""));
    os_ << ")";
  }

  void print(config::FeatureFunctionLibraryConfigPtr config_ptr) {
    libraries_.push_back(config_ptr);
    os_ << "lib(\"" << extractBasename(config_ptr->path()) << "\")";
  }
};

}  // namespace lang

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                REGISTER VISITOR
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

namespace lang {

class ModelConfigRegister : public config::ModelConfigVisitor {
 public:
  // Constructors
  explicit ModelConfigRegister(chaiscript::ChaiScript &chai)
      : chai_(chai) {
  }

 protected:
  // Overriden functions
  void startVisit() override {
  }

  void endVisit() override {
  }

  void visitOption(config::option::Model &visited) override {
    chai_.add(chaiscript::var(&visited), tag_);
  }

  void visitOption(config::option::State &visited) override {
    chai_.add(chaiscript::var(&visited), tag_);
  }

  void visitOption(config::option::Duration &visited) override {
    chai_.add(chaiscript::var(&visited), tag_);
  }

  void visitOption(config::option::FeatureFunctionLibrary &visited) override {
    chai_.add(chaiscript::var(&visited), tag_);
  }

  void visitOption(config::option::Models &visited) override {
    chai_.add(chaiscript::var(&visited), tag_);
  }

  void visitOption(config::option::States &visited) override {
    chai_.add(chaiscript::var(&visited), tag_);
  }

  void visitOption(config::option::FeatureFunctionLibraries &visited) override {
    chai_.add(chaiscript::var(&visited), tag_);
  }

  void visitOption(config::option::Type &visited) override {
    chai_.add(chaiscript::var(&visited), tag_);
  }

  void visitOption(config::option::Size &visited) override {
    chai_.add(chaiscript::var(&visited), tag_);
  }

  void visitOption(config::option::Alphabet &visited) override {
    chai_.add(chaiscript::var(&visited), tag_);
  }

  void visitOption(config::option::Probability &visited) override {
    chai_.add(chaiscript::var(&visited), tag_);
  }

  void visitOption(config::option::Probabilities &visited) override {
    chai_.add(chaiscript::var(&visited), tag_);
  }

  void visitOption(config::option::FeatureFunctions &visited) override {
    chai_.add(chaiscript::var(&visited), tag_);
    chai_.add(chaiscript::fun([&visited] (
        const std::string &name, config::option::FeatureFunction fun) {
      visited.emplace(name, fun);
    }), "feature");
  }

  void visitTag(const std::string &tag, std::size_t /* count */,
                                        std::size_t /* max */) override {
    tag_ = tag;
  }

  void visitLabel(const std::string &/* label */) override {
  }

  void visitPath(const std::string &/* path */) override {
  }

 private:
  // Instance variables
  chaiscript::ChaiScript &chai_;
  std::string tag_;
};

}  // namespace lang

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                  INTERPRETER
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

namespace lang {

#define REGISTER_TYPE(type) \
  module->add(chaiscript::user_type<config::option::type>(), #type)

#define REGISTER_VECTOR(type) \
  do { \
    using chaiscript::vector_conversion; \
    using chaiscript::bootstrap::standard_library::vector_type; \
    module->add(vector_type<config::option::type>(#type)); \
    module->add(vector_conversion<config::option::type>()); \
  } while (false)

#define REGISTER_MAP(type) \
  do { \
    using chaiscript::map_conversion; \
    using chaiscript::bootstrap::standard_library::map_type; \
    module->add(map_type<config::option::type>(#type)); \
    module->add(map_conversion<config::option::type>()); \
  } while (false)

class Interpreter {
 public:
  // Inner struct
  struct Environment {
    config::ModelConfigPtr model_config_ptr;
    config::ConverterPtr converter_ptr;
  };

  // Concrete methods
  Environment evalModel(const std::string &filepath) {
    auto model_cfg = makeModelConfig(filepath);
    auto converter = makeConverver(model_cfg);
    return { model_cfg, converter };
  }

 private:
  // Concrete methods
  config::ModelConfigPtr makeModelConfig(const std::string &filepath) {
    auto model_type = findModelType(filepath);

    if (model_type == "GHMM") {
      return fillConfig<config::GHMMConfig>(filepath);
    } else if (model_type == "HMM") {
      return fillConfig<config::HMMConfig>(filepath);
    } else if (model_type == "LCCRF") {
      return fillConfig<config::LCCRFConfig>(filepath);
    } else if (model_type == "IID") {
      return fillConfig<config::IIDConfig>(filepath);
    } else if (model_type == "VLMC") {
      return fillConfig<config::VLMCConfig>(filepath);
    } else if (model_type == "IMC") {
      return fillConfig<config::IMCConfig>(filepath);
    } else if (model_type == "PeriodicIMC") {
      return fillConfig<config::PeriodicIMCConfig>(filepath);
    } else if (model_type == "SBSW") {
      return fillConfig<config::SBSWConfig>(filepath);
    } else if (model_type == "MultipleSequentialModels") {
      return fillConfig<config::MultipleSequentialModelsConfig>(filepath);
    } else if (model_type == "") {
      throw std::logic_error(
        filepath + ": Model type not specified!");
    } else {
      throw std::logic_error(
        filepath + ": Unknown model type \"" + model_type + "\"");
    }
  }

  config::ConverterPtr makeConverver(config::ModelConfigPtr model_cfg) {
    return std::make_shared<config::Converter>(
      std::get<decltype("observations"_t)>(*model_cfg.get()));
  }

  std::string findModelType(const std::string &filepath) {
    auto root = extractDir(filepath);

    std::vector<std::string> modulepaths;
    std::vector<std::string> usepaths { root };

    chaiscript::ChaiScript chai(modulepaths, usepaths);
    chai.add(makeInterpreterLibrary(root));

    auto cfg = std::make_shared<config::ModelConfig>(filepath);
    cfg->accept(ModelConfigRegister(chai));

    try { chai.eval_file(filepath); }
    catch (const std::exception &e) {
      // Explicitly ignore missing object exceptions
      if (!missingObjectException(e)) throw;
    }

    return std::get<decltype("model_type"_t)>(*cfg.get());
  }

  bool missingObjectException(const std::exception &e) {
    std::string exception(e.what());
    return exception.find("Can not find object:") != std::string::npos;
  }

  template<typename Config>
  std::shared_ptr<Config> fillConfig(const std::string &filepath) {
    auto root = extractDir(filepath);

    std::vector<std::string> modulepaths;
    std::vector<std::string> usepaths { root };

    chaiscript::ChaiScript chai(modulepaths, usepaths);
    chai.add(makeInterpreterLibrary(root));

    auto cfg = std::make_shared<Config>(filepath);
    cfg->accept(ModelConfigRegister(chai));

    chai.eval_file(filepath);

    return cfg;
  }

  chaiscript::ModulePtr makeInterpreterLibrary(const std::string &root) {
    static auto interpreter_library = std::make_shared<chaiscript::Module>();
    static bool initialized = false;

    if (!initialized) {
      registerTypes(interpreter_library, root);
      registerHelpers(interpreter_library, root);
      registerConstants(interpreter_library, root);
      registerAttributions(interpreter_library, root);
      registerConcatenations(interpreter_library, root);
      initialized = true;
    }

    return interpreter_library;
  }

  void registerTypes(chaiscript::ModulePtr &module,
                     const std::string &/* root */) {
    REGISTER_TYPE(Type);
    REGISTER_TYPE(Alphabet);
    REGISTER_TYPE(Size);
    REGISTER_TYPE(Probabilities);
    REGISTER_TYPE(Duration);
    REGISTER_TYPE(Model);
    REGISTER_TYPE(Models);
    REGISTER_TYPE(State);
    REGISTER_TYPE(States);
    REGISTER_TYPE(FeatureFunctions);
    REGISTER_TYPE(FeatureFunctionLibraries);

    REGISTER_VECTOR(Alphabet);
    REGISTER_VECTOR(Models);
    REGISTER_VECTOR(FeatureFunctionLibraries);

    REGISTER_MAP(Probabilities);
    REGISTER_MAP(States);
  }

  void registerHelpers(chaiscript::ModulePtr &module,
                       const std::string &root) {
    using chaiscript::fun;

    module->add(fun([this, root] (const std::string &file) {
      return this->makeModelConfig(root + file);
    }), "model");

    module->add(fun([this]() {
      auto duration_cfg = std::make_shared<config::GeometricDurationConfig>();
      return config::DurationConfigPtr(duration_cfg);
    }), "geometric");

    module->add(fun([this, root] (const std::string &file) {
      auto duration_cfg = std::make_shared<config::ExplicitDurationConfig>();
      std::get<decltype("model"_t)>(*duration_cfg.get())
        = this->makeModelConfig(root + file);
      return config::DurationConfigPtr(duration_cfg);
    }), "explicit");

    module->add(fun([this, root] (const std::string &file, unsigned int size) {
      auto duration_cfg = std::make_shared<config::ExplicitDurationConfig>();
      std::get<decltype("max_size"_t)>(*duration_cfg.get()) = size;
      std::get<decltype("model"_t)>(*duration_cfg.get())
        = this->makeModelConfig(root + file);
      return config::DurationConfigPtr(duration_cfg);
    }), "explicit");

    module->add(fun([this] (unsigned int size) {
      auto duration_cfg = std::make_shared<config::SignalDurationConfig>();
      std::get<decltype("size"_t)>(*duration_cfg.get()) = size;
      return config::DurationConfigPtr(duration_cfg);
    }), "fixed");

    module->add(fun([this] (unsigned int size) {
      auto duration_cfg = std::make_shared<config::MaxLenghtConfig>();
      std::get<decltype("size"_t)>(*duration_cfg.get()) = size;
      return config::DurationConfigPtr(duration_cfg);
    }), "max_lenght");

    module->add(fun([this, root] (const std::string &file) {
      using config::FeatureFunctionLibraryConfig;
      return this->fillConfig<FeatureFunctionLibraryConfig>(root + file);
    }), "lib");
  }

  void registerConstants(chaiscript::ModulePtr &module,
                         const std::string &/* root */) {
    using chaiscript::const_var;

    module->add_global_const(const_var(std::string("emission")), "emission");
    module->add_global_const(const_var(std::string("duration")), "duration");
  }

  void registerAttributions(chaiscript::ModulePtr &module,
                            const std::string &/* root */) {
    using chaiscript::fun;
    using chaiscript::boxed_cast;

    using chaiscript::Boxed_Value;
    using Map = std::map<std::string, Boxed_Value>;

    module->add(fun([] (config::option::States &conv, const Map &orig) {
      for (auto &pair : orig) {
        auto inner_orig
          = boxed_cast<std::map<std::string, Boxed_Value> &>(pair.second);

        conv[pair.first] = std::make_shared<config::StateConfig>();

        std::get<decltype("duration"_t)>(*conv[pair.first])
          = boxed_cast<config::DurationConfigPtr>(inner_orig["duration"]);

        std::get<decltype("emission"_t)>(*conv[pair.first])
          = boxed_cast<config::ModelConfigPtr>(inner_orig["emission"]);
      }
    }), "=");
  }

  void registerConcatenations(chaiscript::ModulePtr &module,
                              const std::string &/* root */) {
    using chaiscript::fun;

    module->add(fun([] (const std::string &lhs, const std::string &rhs) {
      return lhs + " | " + rhs;
    }), "|");

    module->add(fun([] (const std::string &lhs, const std::string &rhs) {
      return rhs + " | " + lhs;
    }), "->");
  }
};

}  // namespace lang


/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                      MAIN
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

int main(int argc, char **argv) {
  if (argc <= 1 || argc >= 5) {
    std::cerr << "USAGE: " << argv[0] << " model_config [dataset] [output_dir]"
              << std::endl;
    return EXIT_FAILURE;
  }

  lang::Interpreter interpreter;
  auto env = interpreter.evalModel(argv[1]);

  /*--------------------------------------------------------------------------*/
  /*                                CONVERTER                                 */
  /*--------------------------------------------------------------------------*/

  if (argc >= 3) {
    std::fstream dataset(argv[2]);

    std::string line;
    while (std::getline(dataset, line)) {
      std::cout << std::endl;

      std::cout << "Input: " << line << std::endl;

      std::cout << "Output: ";
      for (unsigned int n : env.converter_ptr->convert(line)) std::cout << n;
      std::cout << std::endl;
    }

    std::cout << std::endl;
  }

  /*--------------------------------------------------------------------------*/
  /*                           PRINTER / SERIALIZER                           */
  /*--------------------------------------------------------------------------*/

  switch (argc) {
    case 2: /* fall through */
    case 3: std::cout << *env.model_config_ptr; break;
    case 4: env.model_config_ptr->accept(lang::ModelConfigSerializer(argv[3]));
            break;
  }

  return EXIT_SUCCESS;
}
