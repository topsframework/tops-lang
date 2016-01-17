// Standard headers
#include <map>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <iostream>

// External headers
#include "chaiscript/chaiscript.hpp"
#include "chaiscript/dispatchkit/bootstrap_stl.hpp"

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                    PRINTER
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

template<typename T, typename U>
std::ostream &operator<<(std::ostream &os, const std::map<T, U> &map);

template<typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vector);

template<typename T, typename U>
std::ostream &operator<<(std::ostream &os, const std::pair<T, U> &pair) {
  os << pair.first << ": " << pair.second;
  return os;
}

class PrinterBase {
 protected:
  static unsigned int depth_;
};

unsigned int PrinterBase::depth_ = 0;

template<typename Iterable>
class Printer : public PrinterBase {
 public:
  static std::ostream &to_ostream(std::ostream &os, const Iterable &iterable) {
    os << "[ " << std::endl;

    depth_++;

    auto it = std::begin(iterable);
    while (std::next(it) != std::end(iterable)) {
      std::fill_n(std::ostream_iterator<char>(os), 2*depth_, ' ');
      os << *it << ", " << std::endl;
      ++it;
    }
    std::fill_n(std::ostream_iterator<char>(os), 2*depth_, ' ');
    os << *it << std::endl;

    depth_--;

    std::fill_n(std::ostream_iterator<char>(os), 2*depth_, ' ');
    os << "]";

    return os;
  }
};

template<typename Iterable>
inline std::ostream &to_ostream(std::ostream &os, const Iterable &iterable) {
  return Printer<Iterable>::to_ostream(os, iterable);
}

template<typename T, typename U>
std::ostream &operator<<(std::ostream &os, const std::map<T, U> &map) {
  return to_ostream(os, map);
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vector) {
  return to_ostream(os, vector);
}

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                                    CONFIG
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

struct HMMConfig {
  std::string model_type;
  std::vector<std::string> observations;

  std::map<std::string, double> initial_probabilities;
  std::map<std::string, double> transition_probabilities;
  std::map<std::string, std::map<std::string, std::string>> states;
};

std::ostream &operator<<(std::ostream &os, const HMMConfig &cfg) {
  os << "model_type = "               << cfg.model_type               << std::endl;
  os << "observations = "             << cfg.observations             << std::endl;
  os << "initial_probabilities = "    << cfg.initial_probabilities    << std::endl;
  os << "transition_probabilities = " << cfg.transition_probabilities << std::endl;
  os << "states = "                   << cfg.states                   << std::endl;
  return os;
}

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

  chaiscript::ChaiScript chai;
  using namespace chaiscript;

  /*-------------------------------------------------------------------------*/
  /*                                OVERLOADS                                */
  /*-------------------------------------------------------------------------*/

  // Add overload of operator '=' to make variable attribution
  chai.add(fun([&chai] (std::vector<std::string> &conv,
                        const std::vector<Boxed_Value> &orig) {
    for (const auto &bv : orig)
      conv.emplace_back(chai.boxed_cast<std::string>(bv));
  }), "=");

  // Add overload of operator '=' to make variable attribution
  chai.add(fun([&chai] (std::map<std::string, double> &conv,
                        const std::map<std::string, Boxed_Value> &orig) {
    for (const auto &pair : orig)
      conv[pair.first] = chai.boxed_cast<double>(pair.second);
  }), "=");

  // Add overload of operator '=' to make variable attribution
  chai.add(fun([&chai] (std::map<std::string, std::map<std::string, std::string>> &conv,
                        const std::map<std::string, Boxed_Value> &orig) {
    for (auto &pair : orig) {
      for (auto &deep_pair
          : chai.boxed_cast<std::map<std::string, Boxed_Value> &>(pair.second)) {
        conv[pair.first][deep_pair.first]
          = chai.boxed_cast<std::string>(deep_pair.second);
      }
    }
  }), "=");

  // Add overload of operator '|' to concatenate strings
  chai.add(fun([] (const std::string &lhs, const std::string &rhs) {
    return lhs + " | " + rhs;
  }), "|");

  /*-------------------------------------------------------------------------*/
  /*                                REGISTER                                 */
  /*-------------------------------------------------------------------------*/

  HMMConfig hmm_cfg;
  chai.add(var(&hmm_cfg.model_type), "model_type");
  chai.add(var(&hmm_cfg.states), "states");
  chai.add(var(&hmm_cfg.observations), "observations");
  chai.add(var(&hmm_cfg.initial_probabilities), "initial_probabilities");
  chai.add(var(&hmm_cfg.transition_probabilities), "transition_probabilities");

  chai.eval_file(argv[1]);

  std::cout << hmm_cfg;

  return EXIT_SUCCESS;
}
