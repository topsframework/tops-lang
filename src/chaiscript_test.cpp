// Standard headers
#include <map>
#include <string>
#include <cstdlib>
#include <iostream>

// External headers
#include <chaiscript/chaiscript.hpp>

struct HMMConfig {
  std::string model_name;
  std::vector<std::string> state_names;
  std::vector<std::string> observation_symbols;

  std::map<std::string, double> transitions;
  std::map<std::string, double> emission_probabilities;
  std::map<std::string, double> initial_probabilities;
};

std::ostream &operator<<(std::ostream &os, const HMMConfig &cfg) {
  os << cfg.model_name << std::endl;
  for (const auto &state : cfg.state_names)
    os << state << std::endl;
  for (const auto &symbol : cfg.observation_symbols)
    os << symbol << std::endl;
  for (const auto &trans : cfg.transitions)
    os << trans.first << ": " << trans.second << std::endl;
  for (const auto &em : cfg.emission_probabilities)
    os << em.first << ": " << em.second << std::endl;
  for (const auto &em : cfg.initial_probabilities)
    os << em.first << ": " << em.second << std::endl;
  return os;
}

int main(int argc, char **argv) {

  if (argc != 2) {
    std::cerr << "USAGE: " << argv[0] << " hmm_script_name" << std::endl;
    return EXIT_FAILURE;
  }

  chaiscript::ChaiScript chai;
  using namespace chaiscript;

  // Add overload of operator '=' to convert vector of Boxed_Value in std::string
  chai.add(fun([] (std::vector<std::string> &conv,
                   const std::vector<Boxed_Value> &orig) {
    for (const auto &bv : orig) conv.emplace_back(boxed_cast<std::string>(bv));
  }), "=");

  // Add overload of operator '=' to convert maps with Boxed_Value in Probability
  chai.add(fun([] (std::map<std::string, double> &conv,
                   const std::map<std::string, Boxed_Value> &orig) {
    for (const auto &pair : orig) conv[pair.first] = boxed_cast<double>(pair.second);
  }), "=");

  // Add overload of operator '|' to concatenate strings
  chai.add(fun([] (const std::string &lhs, const std::string &rhs) {
    return lhs + " | " + rhs;
  }), "|");

  HMMConfig hmm_cfg;
  chai.add(var(&hmm_cfg.model_name), "model_name");
  chai.add(var(&hmm_cfg.state_names), "state_names");
  chai.add(var(&hmm_cfg.transitions), "transitions");
  chai.add(var(&hmm_cfg.observation_symbols), "observation_symbols");
  chai.add(var(&hmm_cfg.initial_probabilities), "initial_probabilities");
  chai.add(var(&hmm_cfg.emission_probabilities), "emission_probabilities");

  chai.eval_file(argv[1]);

  std::cout << hmm_cfg;

  return EXIT_SUCCESS;
}
