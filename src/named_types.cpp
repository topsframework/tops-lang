// Standard headers
#include <map>
#include <tuple>
#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <iostream>

// External headers
#include <named_types/named_tuple.hpp>
#include <named_types/rt_named_tag.hpp>
#include <named_types/literals/integral_string_literal.hpp>

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                  CONVERSIONS
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

namespace detail {

// To allow ADL with custom begin/end and custom to_string
using std::begin;
using std::end;
using std::to_string;

// is_iterable

template<typename T>
auto is_iterable_impl(void *)
-> decltype (
    begin(std::declval<T&>()) != end(std::declval<T&>()), // begin/end and operator !=
    ++std::declval<decltype(begin(std::declval<T&>()))&>(), // operator ++
    *begin(std::declval<T&>()), // operator*
    std::true_type{});

template<typename T>
std::false_type is_iterable_impl(...);

// is_convertible_to_string

template<typename T>
auto is_convertible_to_string_impl(void *)
-> decltype (
    to_string(std::declval<T&>()), // to_string
    std::true_type{});

template<typename T>
std::false_type is_convertible_to_string_impl(...);

// is_pair

template<typename>
struct is_pair_impl : std::false_type {};

template<typename F, typename S>
struct is_pair_impl<std::pair<F, S>> : std::true_type {};

// is_named_tuple

template<typename>
struct is_named_tuple_impl : std::false_type {};

template<typename... Ts>
struct is_named_tuple_impl<named_types::named_tuple<Ts...>> : std::true_type {};

}

template <typename T>
using is_iterable = decltype(detail::is_iterable_impl<T>(nullptr));

template <typename T>
using is_convertible_to_string
  = decltype(detail::is_convertible_to_string_impl<T>(nullptr));

template <typename T>
using is_pair = detail::is_pair_impl<T>;

template <typename T>
using is_named_tuple = detail::is_named_tuple_impl<T>;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                    PRINTER
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

class Printer {
 public:
  Printer(std::ostream &os) : os(os) {
  }

  template<typename T>
  std::ostream &to_ostream(std::ostream &os, const T &value) {
    printf(value);
    return os;
  }

 private:
  std::ostream &os;
  unsigned int depth_ = 0;

  void printf(const std::string &string) {
    printr('"'); printr(string); printr('"');
  }

  template<typename T>
  auto printf(const T &value)
      -> std::enable_if_t<std::is_function<T>::value> {
    printr("function");
  }

  template<typename T>
  auto printf(const T &value)
      -> std::enable_if_t<is_convertible_to_string<T>::value> {
    printr(std::to_string(value));
  }

  template<typename Pair>
  auto printf(const Pair &pair)
      -> std::enable_if_t<is_pair<Pair>::value> {
    printf(pair.first);
    printr(": ");
    printf(pair.second);
  }

  template<typename Iterable>
  auto printf(const Iterable &iterable)
      -> std::enable_if_t<
           is_iterable<Iterable>::value
           && !std::is_convertible<Iterable, std::string>::value> {
    open_iterable();
    for (auto it = std::begin(iterable); it != std::end(iterable); ++it) {
      indent();
      printf(*it);
      printr(it == std::prev(std::end(iterable)) ? "" : ",");
      newline();
    }
    close_iterable();
  }

  template<typename Tuple>
  auto printf(const Tuple &tuple)
      -> std::enable_if_t<is_named_tuple<Tuple>::value> {
    for_each([this](const auto &tag, const auto &value) {
      using Tag = std::remove_cv_t<std::remove_reference_t<decltype(tag)>>;
      if (Tuple::template tag_index<Tag>::value > 0) newline();
      indent();
      printr(typename Tag::value_type().str());
      printr(" = ");
      printf(value);
    }, tuple);
  }

  void open_iterable() {
    printr("[ ");
    newline();
    depth_++;
  }

  void close_iterable() {
    depth_--;
    indent();
    printr("]");
  }

  void indent() {
    std::fill_n(std::ostreambuf_iterator<char>(os), 2*depth_, ' ');
  }

  void newline() {
    os << std::endl;
  }

  template<typename T>
  void printr(const T &value) {
    os << value;
  }
};

template<typename... Ts>
std::ostream &operator<<(
    std::ostream &os, const named_types::named_tuple<Ts...> &tuple) {
  return Printer(os).to_ostream(os, tuple);
}

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                     OTHER
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

namespace {

template<typename T, T... chars>
constexpr decltype(auto) operator ""_t () {
  return named_types::named_tag<named_types::string_literal<T, chars...>>{};
}

}  // namespace

template<typename T>
struct delayed_true : public std::true_type {};

template<typename T>
struct delayed_false : public std::false_type {};

template<typename Base, typename... Options>
class basic_config : public named_types::named_tuple<Options...> {
  using named_types::named_tuple<Options...>::named_tuple;

  template<typename NewBase, typename B, typename... Ts, std::size_t... Is>
  auto upcast_impl(const basic_config<B, Ts...>& cfg,
                   std::index_sequence<Is...>) {
    using base_config
      = basic_config<NewBase, std::tuple_element_t<Is, Ts>...>;
    return base_config(std::get<Is>(cfg)...);
  }

  template<typename NewBase, std::size_t Size, typename B, typename... Ts>
  auto upcast(const basic_config<B, Ts...>& cfg) {
    static_assert(Size < sizeof...(Ts),
      "Subtuple must have less parameters than original tuple");
    return upcast_impl<NewBase>(cfg, std::make_index_sequence<Size>());
  }

 public:
  template<typename BaseBase, typename... BaseOptions>
  explicit operator basic_config<BaseBase, BaseOptions...>() {
    static_assert(
      std::is_same<Base, basic_config<BaseBase, BaseOptions...>>::value,
      "Cast is allowed only to base config type");
    return upcast_impl<BaseBase, sizeof...(BaseOptions)>(*this);
  }
};

template<typename... Options>
struct config_with_options {
  using type = basic_config<void, Options...>;// named_types::named_tuple<Options...>;

  template<typename T>
  struct extending {
    static_assert(delayed_false<T>::value, "Is not a configuration");
  };

  template<typename BaseBase, typename... BaseOptions>
  struct extending<basic_config<BaseBase, BaseOptions...>> {
    using type = basic_config<
                   basic_config<BaseBase, BaseOptions...>,
                   BaseOptions..., Options...>;
  };
};

#define CONFIG_OPTION(name) using name = decltype(#name##_t)

CONFIG_OPTION(model_type);
CONFIG_OPTION(observations);
CONFIG_OPTION(emission_probabilities);
CONFIG_OPTION(initial_probabilities);
CONFIG_OPTION(transition_probabilities);
CONFIG_OPTION(states);

int main() {
  using ModelConfig
    = config_with_options<
        std::string(model_type),
        std::vector<std::string>(observations)
      >::type;

  ModelConfig model_config {
    "IID",
    { "A", "T", "C", "G" }
  };

  std::cout << "===========================================" << std::endl;
  std::cout << model_config << std::endl;
  std::cout << "===========================================" << std::endl;

  using IIDConfig
    = config_with_options<
        std::map<std::string, double>(emission_probabilities)
      >::extending<ModelConfig>::type;

  IIDConfig iid_config {
    "IID",
    { "A", "T", "C", "G" },
    { {"A", 0.25}, {"T", 0.25}, {"C", 0.25}, {"G", 0.25} }
  };

  std::cout << "===========================================" << std::endl;
  std::cout << iid_config << std::endl;
  std::cout << "===========================================" << std::endl;

  using GHMMConfig
    = config_with_options<
        std::map<std::string, double>(initial_probabilities),
        std::map<std::string, double>(transition_probabilities),
        std::map<std::string, std::map<std::string, std::string>>(states)
      >::extending<ModelConfig>::type;

  GHMMConfig ghmm_config {
    "GHMM",
    { "1", "2", "3", "4", "5", "6" },
    {
      { "Fair", 0.5 },
      { "Loaded", 0.5 }
    },
    {
      { "Loaded | Fair", 0.1 },
      { "Fair | Fair", 0.9 },
      { "Fair | Loaded", 0.1 },
      { "Loaded | Loaded", 0.9 }
    },
    {
      {
        "Fair",
        {
          { "duration", "geometric" },
          { "emission", "explicit" }
        }
      },
      {
        "Loaded",
        {
          { "duration", "geometric" },
          { "emission", "explicit" }
        }
      }
    }
  };

  std::cout << "===========================================" << std::endl;
  std::cout << ghmm_config << std::endl;
  std::cout << "===========================================" << std::endl;

  auto upcasted_ghmm_config = static_cast<ModelConfig>(ghmm_config);
  std::cout << upcasted_ghmm_config << std::endl;

  using LCCRFConfig
    = config_with_options<
        std::vector<
          std::function<double(unsigned int, unsigned int, std::vector<unsigned int>, unsigned int)>
        >(decltype("features"_t))
      >::extending<ModelConfig>::type;

  LCCRFConfig lccrf_config {
    "LCCRF",
    { "1", "2", "3", "4", "5", "6" },
    {
      [](unsigned int prev, unsigned int curr, std::vector<unsigned int>, unsigned int) {
        if (prev == 1 and curr == 0) return 0.1; // Loaded → Fair
        else return 0.0;
      },
      [](unsigned int prev, unsigned int curr, std::vector<unsigned int>, unsigned int) {
        if (prev == 0 and curr == 0) return 0.9; // Fair → Fair
        else return 0.0;
      },
      [](unsigned int prev, unsigned int curr, std::vector<unsigned int>, unsigned int) {
        if (prev == 0 and curr == 1) return 0.1; // Fair → Loaded
        else return 0.0;
      },
      [](unsigned int prev, unsigned int curr, std::vector<unsigned int>, unsigned int) {
        if (prev == 1 and curr == 1) return 0.9; // Loaded → Loaded
        else return 0.0;
      }
    }
  };

  // std::cout << "===========================================" << std::endl;
  // std::cout << lccrf_config << std::endl;
  // std::cout << "===========================================" << std::endl;

  return 0;
}
