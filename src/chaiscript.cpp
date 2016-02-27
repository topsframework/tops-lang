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
#include <string>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <utility>
#include <iostream>

// External headers
#include "chaiscript/chaiscript.hpp"
#include "chaiscript/dispatchkit/bootstrap_stl.hpp"

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                    PRINTER
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
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
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                     CONFIG
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

/*----------------------------------------------------------------------------*/
/*                                   Model                                    */
/*----------------------------------------------------------------------------*/

struct ModelConfig {
  std::string model_type;
  std::vector<std::string> observations;
};

void register_params(ModelConfig &cfg, chaiscript::ChaiScript &chai) {
  using chaiscript::var;
  chai.add(var(&cfg.model_type), "model_type");
  chai.add(var(&cfg.observations), "observations");
}

std::ostream &operator<<(std::ostream &os, const ModelConfig &cfg) {
  os << "model_type = " << cfg.model_type << std::endl;
  os << "observations = " << cfg.observations << std::endl;
  return os;
}

/*----------------------------------------------------------------------------*/
/*                                    IID                                     */
/*----------------------------------------------------------------------------*/

struct IIDConfig : public ModelConfig {
  std::map<std::string, double> emission_probabilities;
};

void register_params(IIDConfig &cfg, chaiscript::ChaiScript &chai) {
  using chaiscript::var;
  register_params(static_cast<ModelConfig &>(cfg), chai);
  chai.add(var(&cfg.emission_probabilities), "emission_probabilities");
}

std::ostream &operator<<(std::ostream &os, const IIDConfig &cfg) {
  os << static_cast<const ModelConfig &>(cfg);
  os << "emission_probabilities = " << cfg.emission_probabilities << std::endl;
  return os;
}

/*----------------------------------------------------------------------------*/
/*                                    GHMM                                    */
/*----------------------------------------------------------------------------*/

struct GHMMConfig : public ModelConfig {
  std::map<std::string, double> initial_probabilities;
  std::map<std::string, double> transition_probabilities;
  std::map<std::string, std::map<std::string, std::string>> states;
};

void register_params(GHMMConfig &cfg, chaiscript::ChaiScript &chai) {
  using chaiscript::var;
  register_params(static_cast<ModelConfig &>(cfg), chai);
  chai.add(var(&cfg.initial_probabilities), "initial_probabilities");
  chai.add(var(&cfg.transition_probabilities), "transition_probabilities");
  chai.add(var(&cfg.states), "states");
}

std::ostream &operator<<(std::ostream &os, const GHMMConfig &cfg) {
  os << static_cast<const ModelConfig &>(cfg);
  os << "initial_probabilities = " << cfg.initial_probabilities << std::endl;
  os << "transition_probabilities = " << cfg.transition_probabilities
     << std::endl;
  os << "states = " << cfg.states << std::endl;
  return os;
}

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                    HELPERS
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

std::string model(const std::string &file) {
  return "explicit";
}

std::string geometric() {
  return "geometric";
}

std::string fixed(unsigned int size) {
  return "fixed";
}

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                   INTERPRETER
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

class Interpreter {
 public:
  // Constructors
  Interpreter() : chai_{} {
    using chaiscript::fun;
    using chaiscript::Boxed_Value;

    // Add helpers
    chai_.add(fun(&model), "model");
    chai_.add(fun(&geometric), "geometric");
    chai_.add(fun(&fixed), "fixed");

    // Add overload of operator '=' to make variable attribution
    chai_.add(fun([this] (std::vector<std::string> &conv,
                          const std::vector<Boxed_Value> &orig) {
      for (const auto &bv : orig)
        conv.emplace_back(chai_.boxed_cast<std::string>(bv));
    }), "=");

    // Add overload of operator '=' to make variable attribution
    chai_.add(fun([this] (std::map<std::string, double> &conv,
                          const std::map<std::string, Boxed_Value> &orig) {
      for (const auto &pair : orig)
        conv[pair.first] = chai_.boxed_cast<double>(pair.second);
    }), "=");

    // Add overload of operator '=' to make variable attribution
    chai_.add(fun([this] (
        std::map<std::string, std::map<std::string, std::string>> &conv,
        const std::map<std::string, Boxed_Value> &orig) {
      for (auto &pair : orig) {
        for (auto &deep_pair : chai_.boxed_cast<
            std::map<std::string, Boxed_Value> &>(pair.second)) {
          conv[pair.first][deep_pair.first]
            = chai_.boxed_cast<std::string>(deep_pair.second);
        }
      }
    }), "=");

    // Add overload of operator '|' to concatenate strings
    chai_.add(fun([] (const std::string &lhs, const std::string &rhs) {
      return lhs + " | " + rhs;
    }), "|");
  }

  // Concrete methods
  void eval_file(const std::string &file) {
    auto model_type = find_model_type(file);

    if (model_type == "GHMM") {
      GHMMConfig ghmm_cfg;
      register_params(ghmm_cfg, chai_);
      chai_.eval_file(file);
      std::cout << ghmm_cfg;
    } else if (model_type == "IID") {
      GHMMConfig iid_cfg;
      register_params(iid_cfg, chai_);
      chai_.eval_file(file);
      std::cout << iid_cfg;
    } else {
      std::cerr << "Unknown model" << std::endl;
    }
  }

 private:
  // Instance variables
  chaiscript::ChaiScript chai_;

  // Concrete methods
  std::string find_model_type(const std::string &file) {
    ModelConfig model_cfg;
    register_params(model_cfg, chai_);

    try { chai_.eval_file(file); } catch (...) {}

    return model_cfg.model_type;
  }
};

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                      MAIN
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "USAGE: " << argv[0] << " hmm_script_name" << std::endl;
    return EXIT_FAILURE;
  }

  /*--------------------------------------------------------------------------*/
  /*                                REGISTER                                  */
  /*--------------------------------------------------------------------------*/

  Interpreter interpreter;
  interpreter.eval_file(argv[1]);

  return EXIT_SUCCESS;
}
