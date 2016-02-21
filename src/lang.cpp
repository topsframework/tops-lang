// Standard headers
#include <map>
#include <tuple>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <iostream>
#include <exception>

#include <cassert>

// Internal headers
#include "ParameterPack.hpp"

// External headers
#include "chaiscript/chaiscript.hpp"
#include "chaiscript/dispatchkit/bootstrap_stl.hpp"

#include "named_types/named_tuple.hpp"
#include "named_types/rt_named_tag.hpp"
#include "named_types/literals/integral_string_literal.hpp"

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
                            BASIC CONFIG DECLARATION
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

class BasicConfigInterface;
template<typename Base, typename... Options> class BasicConfig;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                 CONFIG FACTORY
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

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

/*---------------------------------------------------------------------------*/

namespace config_option {

using type = std::string;
using alphabet = std::vector<std::string>;

}  // namespace config_option

/*---------------------------------------------------------------------------*/

using ModelConfig
  = config_with_options<
      config_option::type(decltype("model_type"_t)),
      config_option::alphabet(decltype("observations"_t))
    >::type;

using ModelConfigPtr = std::shared_ptr<ModelConfig>;

/*---------------------------------------------------------------------------*/

using DurationConfig
  = config_with_options<
      config_option::type(decltype("duration_type"_t))
    >::type;

using DurationConfigPtr = std::shared_ptr<DurationConfig>;

/*---------------------------------------------------------------------------*/

namespace config_option {

using model = ModelConfigPtr;
using duration = DurationConfigPtr;

}  // namespace config_option

/*---------------------------------------------------------------------------*/

using StateConfig
  = config_with_options<
      config_option::duration(decltype("duration"_t)),
      config_option::model(decltype("emission"_t))
    >::type;

using StateConfigPtr = std::shared_ptr<StateConfig>;

/*---------------------------------------------------------------------------*/

namespace config_option {

using probabilities = std::map<std::string, double>;

}  // namespace config_option

/*---------------------------------------------------------------------------*/

using IIDConfig
  = config_with_options<
      config_option::probabilities(decltype("emission_probabilities"_t))
    >::extending<ModelConfig>::type;

using IIDConfigPtr = std::shared_ptr<IIDConfig>;

/*---------------------------------------------------------------------------*/

namespace config_option {

using state = StateConfigPtr;
using states = std::map<std::string, StateConfigPtr>;

}  // namespace config_option

/*---------------------------------------------------------------------------*/

using GHMMConfig
  = config_with_options<
      config_option::probabilities(decltype("initial_probabilities"_t)),
      config_option::probabilities(decltype("transition_probabilities"_t)),
      config_option::states(decltype("states"_t))
    >::extending<ModelConfig>::type;

using GHMMConfigPtr = std::shared_ptr<GHMMConfig>;

/*---------------------------------------------------------------------------*/

namespace config_option {

using feature_function = std::function<
  double(unsigned int, unsigned int, std::vector<unsigned int>, unsigned int)>;
using feature_functions = std::map<std::string, feature_function>;

}  // namespace config_option

/*---------------------------------------------------------------------------*/

using LCCRFConfig
  = config_with_options<
      config_option::feature_functions(decltype("feature_functions"_t))
    >::extending<ModelConfig>::type;

using LCCRFConfigPtr = std::shared_ptr<LCCRFConfig>;

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                    VISITOR
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

class ConfigVisitor {
 public:
  // Concrete methods
  template<typename Base, typename... Options>
  void visit(std::shared_ptr<BasicConfig<Base, Options...>> config_ptr) {
    unsigned int count = 0;
    config_ptr->for_each([this, &count](const auto &tag, auto &value) {
      using Tag = std::remove_cv_t<std::remove_reference_t<decltype(tag)>>;
      using Value = std::remove_cv_t<std::remove_reference_t<decltype(value)>>;
      this->visit_tag(typename Tag::value_type().str(), count);
      this->visit(const_cast<Value&>(value));
      count++;
    });
  }

  // Virtual destructor
  virtual ~ConfigVisitor() = default;

 protected:
  // Purely virtual methods
  virtual void visit(config_option::type &) = 0;
  virtual void visit(config_option::alphabet &) = 0;
  virtual void visit(config_option::probabilities &) = 0;

  virtual void visit(config_option::states &) = 0;
  virtual void visit(config_option::feature_functions &) = 0;

  virtual void visit_tag(const std::string &, unsigned int) = 0;
};

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                             BASIC CONFIG DEFINITION
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

class BasicConfigInterface
    : public std::enable_shared_from_this<BasicConfigInterface> {
 public:
  // Alias
  using Self = BasicConfigInterface;

  // Purely virtual methods
  virtual void accept(ConfigVisitor &/* visitor */) const = 0;
  virtual void accept(ConfigVisitor &&/* visitor */) const = 0;

  // Concrete methods
  template<typename Func>
  void for_each(Func&& /* func */) const {}

  template<typename... Args>
  void initialize(Args&&... /* args */) const {}

  template<class Tag>
  inline constexpr decltype(auto) get() const {};

  // Destructor
  virtual ~BasicConfigInterface() = default;
};

template<typename Base, typename... Options>
class BasicConfig : public Base {
 public:
  // Alias
  using Self = BasicConfig<Base, Options...>;
  using tuple_type = named_types::named_tuple<Options...>;

  // Constructors
  explicit BasicConfig() : attrs_() {}

  template<typename... Args>
  BasicConfig(Args&&... args) : attrs_() {
    this->initialize(std::forward<Args>(args)...);
  }

  // Overriden methods
  void accept(ConfigVisitor &visitor) const override {
    auto ptr = std::static_pointer_cast<Self>(
      const_cast<Self*>(this)->shared_from_this());
    visitor.visit(ptr);
  }

  void accept(ConfigVisitor &&visitor) const override {
    auto ptr = std::static_pointer_cast<Self>(
      const_cast<Self*>(this)->shared_from_this());
    visitor.visit(ptr);
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

    initialize_base(std::forward<Args>(args)...);
    initialize_tuple(std::forward<Args>(args)...);
  }

  template<class Tag> inline constexpr auto get() const &
      -> std::enable_if_t<!std::is_same<Base, BasicConfigInterface>::value,
                          const decltype(this->Base::template get<Tag>())> {
    return this->Base::template get<Tag>();
  };

  template<class Tag> inline constexpr decltype(auto) get() const & {
    return std::get<typename named_types::named_tag<Tag>::type>(attrs_);
  };

  template<class Tag> inline auto get() &
      -> std::enable_if_t<!std::is_same<Base, BasicConfigInterface>::value,
                          decltype(this->Base::template get<Tag>())> {
    return this->Base::template get<Tag>();
  };

  template<class Tag> inline decltype(auto) get() & {
    return std::get<typename named_types::named_tag<Tag>::type>(attrs_);
  };

  template<class Tag> inline auto get() &&
      -> std::enable_if_t<!std::is_same<Base, BasicConfigInterface>::value,
                          decltype(this->Base::template get<Tag>())> {
    return this->Base::template get<Tag>();
  };

  template<class Tag> inline decltype(auto) get() && {
    return
      std::get<typename named_types::named_tag<Tag>::type>(std::move(attrs_));
  };

 private:
  // Instance variables
  tuple_type attrs_;

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
        attrs_ = tuple_type(std::forward<decltype(types)>(types)...); },
      index_range<sizeof...(Args) - sizeof...(Options), sizeof...(Args)>(),
      std::forward<Args>(args)...);
  }
};

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                 GET OVERLOEADS
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

namespace std {

template<typename Tag, typename Base, typename... Options>
decltype(auto) get(BasicConfig<Base, Options...> const &input) {
  return input.template get<Tag>();
};

template<typename Tag, typename Base, typename... Options>
decltype(auto) get(BasicConfig<Base, Options...> &input) {
  return input.template get<Tag>();
};

template<typename Tag, typename Base, typename... Options>
decltype(auto) get(BasicConfig<Base, Options...> &&input) {
  return move(input).template get<Tag>();
};

}  // namespace std

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
  PrinterConfigVisitor(std::ostream &os, unsigned int depth = 0)
      : os_(os), depth_(depth), initial_depth_(depth) {
  }

 protected:
  // Overriden functions
  void visit(config_option::type &visited) override {
    printf(visited);
    separate_if_end_of_section();
  }

  void visit(config_option::alphabet &visited) override {
    printf(visited);
    separate_if_end_of_section();
  }

  void visit(config_option::probabilities &visited) override {
    printf(visited);
    separate_if_end_of_section();
  }

  void visit(config_option::states &visited) override {
    printf(visited);
    separate_if_end_of_section();
  }

  void visit(config_option::feature_functions &visited) override {
    printf(visited);
    separate_if_end_of_section();
  }

  void visit_tag(const std::string &tag, unsigned int count) override {
    if (count > 0) os_ << "\n";
    indent();
    os_ << tag << " = ";
  }

 private:
  // Instance variables
  std::ostream &os_;
  unsigned int depth_;

  const unsigned int initial_depth_;

  // Concrete methods
  void separate_if_end_of_section() {
    if (depth_ == initial_depth_) os_ << "\n";
  }

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
      -> std::enable_if_t<is_iterable<Iterable>::value> {
    open_iterable();
    for (auto it = std::begin(iterable); it != std::end(iterable); ++it) {
      indent();
      printf(*it);
      os_ << (it == std::prev(std::end(iterable)) ? "" : ",") << "\n";
    }
    close_iterable();
  }

  void printf(StateConfigPtr state_ptr) {
    os_ << "[ " << "\n";
    depth_++;
    indent();
    os_ << "duration = ";
    printf(std::get<decltype("duration"_t)>(*state_ptr));
    os_ << "," << std::endl;
    indent();
    os_ << "emission = ";
    printf(std::get<decltype("emission"_t)>(*state_ptr));
    depth_--;
    indent();
    os_ << "\n";
    indent();
    os_ << "]";
  }

  void printf(DurationConfigPtr duration_ptr) {
    printf(std::get<decltype("duration_type"_t)>(*duration_ptr));
  }

  void printf(ModelConfigPtr config_ptr) {
    os_ << "{ " << "\n";
    depth_++;
    config_ptr->accept(PrinterConfigVisitor(os_, depth_));
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
std::ostream &operator<<(std::ostream &os, const BasicConfig<Ts...> &config) {
  auto visitor = std::make_shared<PrinterConfigVisitor>(os);
  config.accept(*std::static_pointer_cast<ConfigVisitor>(visitor));
  return os;
}

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                REGISTER VISITOR
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

class RegisterConfigVisitor : public ConfigVisitor {
 public:
  // Constructors
  RegisterConfigVisitor(chaiscript::ChaiScript &chai) : chai_(chai) {
  }

 protected:
  // Overriden functions
  void visit(config_option::type &visited) override {
    chai_.add(chaiscript::var(&visited), tag_);
  }

  void visit(config_option::alphabet &visited) override {
    chai_.add(chaiscript::var(&visited), tag_);
  }

  void visit(config_option::probabilities &visited) override {
    chai_.add(chaiscript::var(&visited), tag_);
  }

  void visit(config_option::states &visited) override {
    chai_.add(chaiscript::var(&visited), tag_);
  }

  void visit(config_option::feature_functions &visited) override {
    chai_.add(chaiscript::var(&visited), tag_);
  }

  void visit_tag(const std::string &tag, unsigned int count) override {
    tag_ = tag;
  }

 private:
  // Instance variables
  chaiscript::ChaiScript &chai_;
  std::string tag_;
};

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                  INTERPRETER
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

class Interpreter {
 public:
  // Concrete methods
  ModelConfigPtr eval_file(const std::string &path) {
    auto found = path.find_last_of("/\\");

    File file {
      path.substr(0, found + 1), // path
      path.substr(found + 1)     // name
    };

    return eval_file(file);
  }

 private:
  // Inner struct
  struct File {
    std::string path;
    std::string name;
  };

  // Concrete methods
  ModelConfigPtr eval_file(const File &file) {
    auto model_type = find_model_type(file);

    if (model_type == "GHMM") {
      return fill_config<GHMMConfig>(file);
    } else if (model_type == "IID") {
      return fill_config<IIDConfig>(file);
    } else {
      throw std::logic_error("Unknown model \"" + model_type + "\"");
    }
  }

  template<typename Config>
  std::shared_ptr<Config> fill_config(const File &file) {
    chaiscript::ChaiScript chai;
    register_helpers(chai, file);
    register_constants(chai, file);
    register_attributions(chai, file);
    register_concatenations(chai, file);

    auto cfg = std::make_shared<Config>();
    cfg->accept(RegisterConfigVisitor(chai));

    // Explicitly ignore all errors (will be handled later)
    try { chai.eval_file(file.path + file.name); } catch (...) {}

    return cfg;
  }

  std::string find_model_type(const File &file) {
    auto model_cfg = fill_config<ModelConfig>(file);
    return std::get<decltype("model_type"_t)>(*model_cfg.get());
  }

  void register_helpers(chaiscript::ChaiScript &chai,
                        const File &main) {
    using namespace chaiscript;

    chai.add(fun([&main] (const std::string &file) {
      return main.path + file;
    }), "model");

    chai.add(fun([]() {
      return std::string("geometric");
    }), "geometric");

    chai.add(fun([] (unsigned int size) {
      return std::string("fixed");
    }), "fixed");
  }

  void register_constants(chaiscript::ChaiScript &chai,
                          const File &main) {
    using namespace chaiscript;
    chai.add(var(std::string("duration")), "duration");
    chai.add(var(std::string("emission")), "emission");
  }

  void register_attributions(chaiscript::ChaiScript &chai,
                             const File &/* main */) {
    using namespace chaiscript;

    chai.add(fun([this, &chai] (
        std::vector<std::string> &conv, const std::vector<Boxed_Value> &orig) {
      for (const auto &bv : orig)
        conv.emplace_back(chai.boxed_cast<std::string>(bv));
    }), "=");

    chai.add(fun([this, &chai] (
        std::map<std::string, double> &conv,
        const std::map<std::string, Boxed_Value> &orig) {
      for (const auto &pair : orig)
        conv[pair.first] = chai.boxed_cast<double>(pair.second);
    }), "=");

    chai.add(fun([this, &chai] (
        std::map<std::string, StateConfigPtr> &conv,
        const std::map<std::string, Boxed_Value> &orig) {
      for (auto &pair : orig) {
        auto inner_orig
          = chai.boxed_cast<std::map<std::string, Boxed_Value> &>(pair.second);

        conv[pair.first] = std::make_shared<StateConfig>();

        std::get<decltype("duration"_t)>(*conv[pair.first])
          = std::make_shared<DurationConfig>();
        std::get<decltype("duration_type"_t)>(
          *std::get<decltype("duration"_t)>(*conv[pair.first]))
            = chai.boxed_cast<std::string>(inner_orig["duration"]);

        auto filename = chai.boxed_cast<std::string>(inner_orig["emission"]);
        std::get<decltype("emission"_t)>(*conv[pair.first])
          = this->eval_file(filename);
      }
    }), "=");
  }

  void register_concatenations(chaiscript::ChaiScript &chai,
                               const File &/* main */) {
    using namespace chaiscript;

    chai.add(fun([] (const std::string &lhs, const std::string &rhs) {
      return lhs + " | " + rhs;
    }), "|");

    chai.add(fun([] (const std::string &lhs, const std::string &rhs) {
      return rhs + " | " + lhs;
    }), "->");
  }
};

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                     MAIN
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

int main(int argc, char **argv) {

  if (argc != 2) {
    std::cerr << "USAGE: " << argv[0] << " hmm_script_name" << std::endl;
    return EXIT_FAILURE;
  }

  /*-------------------------------------------------------------------------*/
  /*                                REGISTER                                 */
  /*-------------------------------------------------------------------------*/

  Interpreter interpreter;
  auto model_architecture_ptr = interpreter.eval_file(argv[1]);
  std::cout << *model_architecture_ptr;

  return EXIT_SUCCESS;
}
