// Standard headers
#include <map>
#include <tuple>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <iostream>

// Internal headers
#include "ParameterPack.hpp"

// External headers
#include "named_types/named_tuple.hpp"
#include "named_types/rt_named_tag.hpp"
#include "named_types/literals/integral_string_literal.hpp"

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                FEATURE FUNCTION
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

using FeatureFunction = std::function<
  double(unsigned int, unsigned int, std::vector<unsigned int>, unsigned int)
>;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                    VISITOR
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

template<typename Base, typename... Options> class basic_config;
template<> class basic_config<void>;

using InterfaceConfig = basic_config<void>;
using InterfaceConfigPtr = std::shared_ptr<basic_config<void>>;

class ConfigVisitor {
 public:
  // Purely virtual methods
  virtual void visit(const std::string &) = 0;
  virtual void visit(const std::vector<std::string> &) = 0;
  virtual void visit(const std::vector<FeatureFunction> &) = 0;
  virtual void visit(const std::map<std::string, double> &) = 0;
  virtual void visit(const std::map<std::string, std::map<std::string, InterfaceConfigPtr>> &) = 0;

  virtual void visit_tag(const std::string &, unsigned int) = 0;

  // Concrete methods
  template<typename Base, typename... Options>
  void visit(std::shared_ptr<basic_config<Base, Options...>> config_ptr) {
    unsigned int count = 0;
    config_ptr->for_each([this, &count](const auto &tag, const auto &value) {
      using Tag = std::remove_cv_t<std::remove_reference_t<decltype(tag)>>;
      this->visit_tag(typename Tag::value_type().str(), count);
      this->visit(value);
      count++;
    });
  }

  // Virtual destructor
  virtual ~ConfigVisitor() = default;
};

template<>
void ConfigVisitor::visit(std::shared_ptr<basic_config<void>> /* config_ptr */) {}

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                  HELPER TRAITS
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

template<typename T>
struct delayed_true : public std::true_type {};

template<typename T>
struct delayed_false : public std::false_type {};

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                   BASIC CONFIG
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

template<typename Base, typename... Options>
class basic_config : public Base {
 public:
  // Alias
  using Self = basic_config<Base, Options...>;
  using tuple_type = named_types::named_tuple<Options...>;

  // Constructors
  explicit basic_config() : attrs_() {}

  template<typename... Args>
  basic_config(Args&&... args) : attrs_() {
    this->initialize(std::forward<Args>(args)...);
  }

  // Overriden methods
  void accept(ConfigVisitor &visitor) const override {
    visitor.visit(std::static_pointer_cast<Self>(
      const_cast<Self*>(this)->shared_from_this()));
  }

  // Concrete methods
  template<typename Func>
  void for_each(Func&& func) const {
    this->Base::for_each(func);
    named_types::for_each(func, attrs_);
  }

  template<typename... Args>
  void initialize(Args&&... args) {
    static_assert(sizeof...(Args) >= sizeof...(Options),
      "Must have least as many arguments as options in `initialize`");

    forward_subpack([this](auto&&... types) {
      this->Base::initialize(std::forward<decltype(types)>(types)...);
    },
    index_range<0, sizeof...(Args) - sizeof...(Options)>(),
    std::forward<Args>(args)...);

    forward_subpack([this](auto&&... types) {
      attrs_ = tuple_type(std::forward<decltype(types)>(types)...);
    },
    index_range<sizeof...(Args) - sizeof...(Options), sizeof...(Args)>(),
    std::forward<Args>(args)...);
  }

 private:
  tuple_type attrs_;
};

template<>
class basic_config<void>
    : public std::enable_shared_from_this<basic_config<void>> {
 public:
  // Alias
  using Self = basic_config<void>;

  // Purely virtual methods
  virtual void accept(ConfigVisitor &/* visitor */) const = 0;

  // Concrete methods
  template<typename Func>
  void for_each(Func&& /* func */) const {}

  template<typename... Args>
  void initialize(Args&&... /* args */) const {}

  // Destructor
  virtual ~basic_config<void>() = default;
};

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                 CONFIG FACTORY
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

template<typename... Options>
struct config_with_options {
  using type = basic_config<basic_config<void>, Options...>;

  template<typename T>
  struct extending {
    static_assert(delayed_false<T>::value, "Is not a configuration");
  };

  template<typename BaseBase, typename... BaseOptions>
  struct extending<basic_config<BaseBase, BaseOptions...>> {
    using type
      = basic_config<basic_config<BaseBase, BaseOptions...>, Options...>;
  };
};

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                    CONFIGS
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

template<typename T, T... chars>
constexpr decltype(auto) operator ""_t () {
  return named_types::named_tag<named_types::string_literal<T, chars...>>{};
}

#define CONFIG_OPTION(config_name, ...) \
  using config_name##_t = __VA_ARGS__; \
  using attr_##config_name = __VA_ARGS__(decltype(#config_name##_t))

/*---------------------------------------------------------------------------*/

CONFIG_OPTION(model_type, std::string);
CONFIG_OPTION(observations, std::vector<std::string>);

using ModelConfig
  = config_with_options<
      attr_model_type,
      attr_observations
    >::type;

/*---------------------------------------------------------------------------*/

CONFIG_OPTION(emission_probabilities, std::map<std::string, double>);

using IIDConfig
  = config_with_options<
      attr_emission_probabilities
    >::extending<ModelConfig>::type;

/*---------------------------------------------------------------------------*/

CONFIG_OPTION(initial_probabilities, std::map<std::string, double>);
CONFIG_OPTION(transition_probabilities, std::map<std::string, double>);
CONFIG_OPTION(states,
  std::map<std::string, std::map<std::string, InterfaceConfigPtr>>);

using GHMMConfig
  = config_with_options<
      attr_initial_probabilities,
      attr_transition_probabilities,
      attr_states
    >::extending<ModelConfig>::type;

/*---------------------------------------------------------------------------*/

CONFIG_OPTION(feature_functions, std::vector<FeatureFunction>);

using LCCRFConfig
  = config_with_options<
      attr_feature_functions
    >::extending<ModelConfig>::type;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                  PRINTER HELPERS
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

// is_pair

template<typename>
struct is_pair_impl : std::false_type {};

template<typename F, typename S>
struct is_pair_impl<std::pair<F, S>> : std::true_type {};

}

template <typename T>
using is_iterable = decltype(detail::is_iterable_impl<T>(nullptr));

template <typename T>
using is_pair = detail::is_pair_impl<T>;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                 PRINTER VISITOR
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

class PrinterConfigVisitor : public ConfigVisitor {
 public:
  // Constructors
  PrinterConfigVisitor(std::ostream &os) : os_(os) {
  }

  // Overriden functions
  void visit(const std::string &visited) override {
    printf(visited);
  }

  void visit(const std::vector<std::string> &visited) override {
    printf(visited);
  }

  void visit(const std::vector<FeatureFunction> &visited) override {
    printf(visited);
  }

  void visit(const std::map<std::string, double> &visited) override {
    printf(visited);
  }

  void visit(const std::map<std::string,
               std::map<std::string, InterfaceConfigPtr>> &visited) override {
    printf(visited);
  }

  void visit_tag(const std::string &tag, unsigned int count) override {
    if (count > 0) os_ << "\n";
    indent();
    os_ << tag << " = ";
  }

 private:
  // Instance variables
  std::ostream &os_;
  unsigned int depth_ = 0;

  // Concrete methods
  void printf(const std::string &string) {
    os_ << '"' << string << '"';
  }

  auto printf(float num) {
    os_ << num;
  }

  auto printf(double num) {
    os_ << num;
  }

  template<typename Return, typename... Params>
  auto printf(const std::function<Return(Params...)> &/* function */) {
    os_ << "function";
  }

  template<typename Pair>
  auto printf(const Pair &pair)
      -> std::enable_if_t<is_pair<Pair>::value> {
    printf(pair.first);
    os_ << ": ";
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
      os_ << (it == std::prev(std::end(iterable)) ? "" : ",") << "\n";
    }
    close_iterable();
  }

  auto printf(InterfaceConfigPtr config_ptr) {
    os_ << "{ " << "\n";
    depth_++;
    config_ptr->accept(*this);
    os_ << "\n";
    depth_--;
    indent();
    os_ << "}";
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
};

template<typename... Ts>
std::ostream &operator<<(std::ostream &os, const basic_config<Ts...> &config) {
  auto visitor = std::make_shared<PrinterConfigVisitor>(os);
  config.accept(*std::static_pointer_cast<ConfigVisitor>(visitor));
  return os;
}

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                      MAIN
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

int main() {
  auto model_config = std::make_shared<ModelConfig>(
    model_type_t{"IID"},
    observations_t{ "A", "T", "C", "G" }
  );

  std::cout << "===========================================" << std::endl;
  std::cout << *model_config << std::endl;
  std::cout << "===========================================" << std::endl;

  auto iid_config = std::make_shared<IIDConfig>(
    model_type_t{ "IID" },
    observations_t{ "A", "T", "C", "G" },
    emission_probabilities_t{
      {"A", 0.25}, {"T", 0.25}, {"C", 0.25}, {"G", 0.25}
    }
  );

  std::cout << "===========================================" << std::endl;
  std::cout << *iid_config << std::endl;
  std::cout << "===========================================" << std::endl;

  auto ghmm_config = std::make_shared<GHMMConfig>(
    model_type_t{ "GHMM" },
    observations_t{ "1", "2", "3", "4", "5", "6" },
    initial_probabilities_t{
      { "Fair", 0.5 },
      { "Loaded", 0.5 }
    },
    transition_probabilities_t{
      { "Loaded | Fair", 0.1 },
      { "Fair | Fair", 0.9 },
      { "Fair | Loaded", 0.1 },
      { "Loaded | Loaded", 0.9 }
    },
    states_t{
      {
        "Fair",
        {
          { "duration", iid_config },
          { "emission", iid_config }
        }
      },
      {
        "Loaded",
        {
          { "duration", iid_config },
          { "emission", iid_config }
        }
      }
    }
  );

  std::cout << "===========================================" << std::endl;
  std::cout << *ghmm_config << std::endl;
  std::cout << "===========================================" << std::endl;

  auto lccrf_config = std::make_shared<LCCRFConfig>(
    model_type_t{"LCCRF"},
    observations_t{ "1", "2", "3", "4", "5", "6" },
    feature_functions_t{
      [](unsigned int prev, unsigned int curr,
         std::vector<unsigned int>, unsigned int) {
        if (prev == 1 and curr == 0) return 0.1; // Loaded → Fair
        else return 0.0;
      },
      [](unsigned int prev, unsigned int curr,
         std::vector<unsigned int>, unsigned int) {
        if (prev == 0 and curr == 0) return 0.9; // Fair → Fair
        else return 0.0;
      },
      [](unsigned int prev, unsigned int curr,
         std::vector<unsigned int>, unsigned int) {
        if (prev == 0 and curr == 1) return 0.1; // Fair → Loaded
        else return 0.0;
      },
      [](unsigned int prev, unsigned int curr,
         std::vector<unsigned int>, unsigned int) {
        if (prev == 1 and curr == 1) return 0.9; // Loaded → Loaded
        else return 0.0;
      }
    }
  );

  std::cout << "===========================================" << std::endl;
  std::cout << *lccrf_config << std::endl;
  std::cout << "===========================================" << std::endl;

  return 0;
}
