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
#include <memory>
#include <string>
#include <vector>
#include <stack>
#include <cstdlib>
#include <iomanip>
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
#include "StringLiteralSuffix.hpp"

#include "filesystem/Filesystem.hpp"

#include "model/Symbol.hpp"
#include "model/Sequence.hpp"

#include "config/Options.hpp"
#include "config/DependencyTreeConfig.hpp"
#include "config/FeatureFunctionLibraryConfig.hpp"

#include "config/ModelConfig.hpp"
#include "config/HMMConfig.hpp"
#include "config/IIDConfig.hpp"
#include "config/IMCConfig.hpp"
#include "config/MDDConfig.hpp"
#include "config/MSMConfig.hpp"
#include "config/GHMMConfig.hpp"
#include "config/SBSWConfig.hpp"
#include "config/VLMCConfig.hpp"
#include "config/LCCRFConfig.hpp"
#include "config/PeriodicIMCConfig.hpp"

#include "config/StateConfig.hpp"

#include "config/DurationConfig.hpp"
#include "config/FixedDurationConfig.hpp"
#include "config/ExplicitDurationConfig.hpp"
#include "config/GeometricDurationConfig.hpp"
#include "config/MaxLengthDurationConfig.hpp"

#include "config/ModelConfigVisitor.hpp"

#include "config/BasicConfig.hpp"

#include "config/Converter.hpp"

#include "lang/Util.hpp"
#include "lang/FilePrinter.hpp"
#include "lang/SingleFilePrinter.hpp"
#include "lang/MultipleFilePrinter.hpp"
#include "lang/ModelConfigSerializer.hpp"

// External headers
#include "chaiscript/chaiscript.hpp"
#include "chaiscript/dispatchkit/bootstrap_stl.hpp"

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 ------------------------------------------------------------------------------
                               SERIALIZER VISITOR
 ------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
*/

namespace lang {

ModelConfigSerializer::ModelConfigSerializer(std::ostream &os)
    : printer_(std::make_shared<SingleFilePrinter>(
          std::shared_ptr<std::ostream>(&os, [] (void *) {}))) {
}

ModelConfigSerializer::ModelConfigSerializer(const std::string &root_dir)
    : printer_(std::make_shared<MultipleFilePrinter>(root_dir)) {
}

}  // namespace lang

template<typename... Ts>
std::ostream &operator<<(std::ostream &os,
                         const config::BasicConfig<Ts...> &config) {
  config.accept(lang::ModelConfigSerializer{});
  return os;
}

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

  void visitOption(config::option::DependencyTrees &visited) override {
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

  void visitOption(config::option::DependencyTree &visited) override {
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
                            DEPENDENCY TREE PARSER
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

namespace lang {

class Interpreter;

class DependencyTreeParser {
 public:
  // Constructors
  DependencyTreeParser(Interpreter* interpreter,
                       std::string root_dir,
                       std::string filename,
                       std::vector<std::string> content)
      : interpreter_(interpreter),
        root_dir_(root_dir),
        filename_(filename),
        content_(content),
        line_(0), column_(1) {
  }

  // Concrete methods
  config::DependencyTreeConfigPtr parse() {
    for (auto line : content_) {
      line_++;
      column_ = 1;
      parseNode(line);
    }

    edges_.insert(edges_.begin(), 0);

    std::stack<unsigned int> stack_edges;
    std::stack<config::DependencyTreeConfigPtr> stack_nodes;
    for (unsigned int i = 0; i < edges_.size(); i++) {
      if (stack_edges.empty()) {
        stack_edges.push(edges_[i]);
        stack_nodes.push(nodes_[i]);
      } else if (stack_edges.top() < edges_[i]) {
        stack_edges.push(edges_[i]);
        stack_nodes.push(nodes_[i]);
      } else if (stack_edges.top() == edges_[i]) {
        auto node1 = stack_nodes.top();
        auto node2 = nodes_[i];
        stack_edges.pop();
        stack_nodes.pop();
        stack_nodes.top()->children().push_back(node1);
        stack_nodes.top()->children().push_back(node2);
      } else {
        auto node = stack_nodes.top();
        stack_nodes.pop();
        stack_edges.pop();
        stack_nodes.top()->children().push_back(node);
        i--;
      }
    }

    while (stack_nodes.size() > 1) {
      auto node = stack_nodes.top();
      stack_nodes.pop();
      stack_nodes.top()->children().push_back(node);
    }
    return stack_nodes.top();
  }

 private:
  // Concrete methods
  void parseNode(std::string line) {
    it_ = line.begin();

    parseSpaces();

    if (next() != '(') {
      parseLevel();
    }

    consume('(');
    auto id = parseId();
    consume(')');
    consume(' ');
    consume('m');
    consume('o');
    consume('d');
    consume('e');
    consume('l');
    consume('(');
    consume('"');

    auto filepath = parseString();

    auto tree = config::DependencyTreeConfig::make(root_dir_ + filename_);
    std::get<decltype("position"_t)>(*tree) = id;
    std::get<decltype("configuration"_t)>(*tree)
      = makeModelConfig(root_dir_ + filepath);
    nodes_.push_back(tree);

    consume('"');
    consume(')');
  }

  config::ModelConfigPtr makeModelConfig(std::string filepath);

  void resetEdgeIndex() {
    reset_edge_index_ = true;
  }

  unsigned int nextEdgeIndex() {
    edge_index_++;

    if (reset_edge_index_) {
      edge_index_ = 0;
      reset_edge_index_ = false;
    }

    while (edge_index_ < leaves_.size()) {
      if (!leaves_[edge_index_])
        return edge_index_;
      edge_index_++;
    }
    return edge_index_;
  }

  void parseLevel() {
    parseSpaces();
    resetEdgeIndex();
    while (next() == '|' && next(2) == ' ') {
      if (edges_[nextEdgeIndex()] != column_) {
        std::cerr << "Parse error at (" << line_ << ", " << column_
                  << "): Wrong edge position" << std::endl;
        exit(0);
      }
      consume('|');
      parseSpaces();
    }
    edges_.push_back(column_);
    if (next() == '|')
      parseChild();
    else if (next() == '`')
      parseLastChild();
  }

  void parseChild() {
    leaves_.push_back(false);
    consume('|');
    consume('-');
    parseSpaces();
  }

  void parseLastChild() {
    leaves_.push_back(true);
    consume('`');
    consume('-');
    parseSpaces();
  }

  int parseSpaces() {
    int s = 0;
    while (next() == ' ') {
      s++;
      consume();
    }
    return s;
  }

  std::string parseId() {
    if (next() == '*') {
      consume();
      return "*";
    } else if (std::isdigit(next())) {
      return parseNumber();
    }

    std::cerr << "Parse error at (" << line_ << ", " << column_
              << "): Node ID should be a number or '*'" << std::endl;
    exit(0);
  }

  std::string parseNumber() {
    std::string num = "";
    while (std::isdigit(next())) {
      num += std::string(1, next());
      consume();
    }
    return num;
  }

  std::string parseString() {
    std::string str = "";
    while (next() != '"' && next() != '\n') {
      str += std::string(1, next());
      consume();
    }
    return str;
  }

  void consume() {
    column_++;
    it_++;
  }

  void consume(char c) {
    if (c == next()) {
      column_++;
      it_++;
    } else {
      std::cerr << "Parse error at (" << line_ << ", "
                << column_ << ") :" << std::endl;
      std::cerr << "  Expecting: " << c << std::endl;
      std::cerr << "  Got: " << next() <<std::endl;
      exit(0);
    }
  }

  char next() {
    return *it_;
  }

  char next(int n) {
    return *(it_ + n - 1);
  }

  Interpreter* interpreter_;

  std::string root_dir_;
  std::string filename_;

  std::vector<std::string> content_;
  std::string::iterator it_;

  std::vector<unsigned int> edges_;
  std::vector<bool> leaves_;
  std::vector<config::DependencyTreeConfigPtr> nodes_;

  unsigned int line_;
  unsigned int column_;
  unsigned int edge_index_;

  bool reset_edge_index_;
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
    checkExtension(filepath);

    auto model_cfg = makeModelConfig(filepath);
    auto converter = makeConverver(model_cfg);
    return { model_cfg, converter };
  }

 private:
  // Concrete methods
  void checkExtension(const std::string &filepath) {
    auto suffix = extractSuffix(filepath);

    if (suffix != "tops") {
      std::ostringstream error_message;
      error_message << "Unknown extension ." << suffix;
      throw std::invalid_argument(error_message.str());
    }
  }

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
    } else if (model_type == "MSM") {
      return fillConfig<config::MSMConfig>(filepath);
    } else if (model_type == "MDD") {
      return fillConfig<config::MDDConfig>(filepath);
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
    auto root_dir = extractDir(filepath);

    std::vector<std::string> modulepaths;
    std::vector<std::string> usepaths { root_dir };

    chaiscript::ChaiScript chai(modulepaths, usepaths);
    chai.add(makeInterpreterLibrary(filepath));

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
    auto root_dir = extractDir(filepath);

    std::vector<std::string> modulepaths;
    std::vector<std::string> usepaths { root_dir };

    chaiscript::ChaiScript chai(modulepaths, usepaths);
    chai.add(makeInterpreterLibrary(filepath));

    auto cfg = std::make_shared<Config>(filepath);
    cfg->accept(ModelConfigRegister(chai));

    chai.eval_file(filepath);

    return cfg;
  }

  chaiscript::ModulePtr makeInterpreterLibrary(const std::string &filepath) {
    static auto interpreter_library = std::make_shared<chaiscript::Module>();
    static bool initialized = false;

    if (!initialized) {
      registerTypes(interpreter_library, filepath);
      registerHelpers(interpreter_library, filepath);
      registerConstants(interpreter_library, filepath);
      registerAttributions(interpreter_library, filepath);
      registerConcatenations(interpreter_library, filepath);
      initialized = true;
    }

    return interpreter_library;
  }

  void registerTypes(chaiscript::ModulePtr &module,
                     const std::string &/* filepath */) {
    REGISTER_TYPE(Type);
    REGISTER_TYPE(Alphabet);
    REGISTER_TYPE(Size);
    REGISTER_TYPE(Probabilities);
    REGISTER_TYPE(Duration);
    REGISTER_TYPE(Model);
    REGISTER_TYPE(Models);
    REGISTER_TYPE(State);
    REGISTER_TYPE(States);
    REGISTER_TYPE(DependencyTree);
    REGISTER_TYPE(DependencyTrees);
    REGISTER_TYPE(FeatureFunctions);
    REGISTER_TYPE(FeatureFunctionLibraries);

    REGISTER_VECTOR(Alphabet);
    REGISTER_VECTOR(Models);
    REGISTER_VECTOR(DependencyTrees);
    REGISTER_VECTOR(FeatureFunctionLibraries);

    REGISTER_MAP(Probabilities);
    REGISTER_MAP(States);
  }

  void registerHelpers(chaiscript::ModulePtr &module,
                       const std::string &filepath) {
    using chaiscript::fun;

    using config::FixedDurationConfig;
    using config::ExplicitDurationConfig;
    using config::GeometricDurationConfig;
    using config::MaxLengthDurationConfig;
    using config::FeatureFunctionLibraryConfig;
    using config::DependencyTreeConfig;

    module->add(fun([this, filepath] (const std::string &file) {
      auto root_dir = extractDir(filepath);
      return this->makeModelConfig(root_dir + file);
    }), "model");

    module->add(fun([this, filepath]() {
      auto duration_ptr = GeometricDurationConfig::make(filepath, "geometric");
      return config::DurationConfigPtr(duration_ptr);
    }), "geometric");

    module->add(fun([this, filepath] (const std::string &file) {
      auto duration_ptr = ExplicitDurationConfig::make(filepath, "explicit");
      std::get<decltype("model"_t)>(*duration_ptr.get())
        = this->makeModelConfig(extractDir(filepath) + file);
      return config::DurationConfigPtr(duration_ptr);
    }), "explicit");

    module->add(fun([this, filepath] (const std::string &file,
                                      unsigned int size) {
      auto duration_ptr = ExplicitDurationConfig::make(filepath, "explicit");
      std::get<decltype("max_size"_t)>(*duration_ptr.get()) = size;
      std::get<decltype("model"_t)>(*duration_ptr.get())
        = this->makeModelConfig(extractDir(filepath) + file);
      return config::DurationConfigPtr(duration_ptr);
    }), "explicit");

    module->add(fun([this, filepath] (config::ModelConfigPtr model_ptr,
                                      unsigned int size) {
      auto duration_ptr = ExplicitDurationConfig::make(filepath, "explicit");
      std::get<decltype("max_size"_t)>(*duration_ptr.get()) = size;
      std::get<decltype("model"_t)>(*duration_ptr.get()) = model_ptr;
      return config::DurationConfigPtr(duration_ptr);
    }), "explicit");

    module->add(fun([this, filepath] (unsigned int size) {
      auto duration_ptr = FixedDurationConfig::make(filepath, "fixed");
      std::get<decltype("size"_t)>(*duration_ptr.get()) = size;
      return config::DurationConfigPtr(duration_ptr);
    }), "fixed");

    module->add(fun([this, filepath] (unsigned int size) {
      auto duration_ptr = MaxLengthDurationConfig::make(filepath, "max_length");
      std::get<decltype("size"_t)>(*duration_ptr.get()) = size;
      return config::DurationConfigPtr(duration_ptr);
    }), "max_length");

    module->add(fun([this, filepath] (const std::string &file) {
      auto root_dir = extractDir(filepath);
      return this->fillConfig<FeatureFunctionLibraryConfig>(root_dir + file);
    }), "lib");

    module->add(fun([this, filepath] (const std::string &file) {
      auto root_dir = extractDir(filepath);

      std::ifstream src(root_dir + file);

      std::string line;
      std::vector<std::string> content;
      while (std::getline(src, line)) {
        content.push_back(line);
      }

      DependencyTreeParser parser(this, root_dir, file, content);
      return parser.parse();
    }), "tree");
  }

  void registerConstants(chaiscript::ModulePtr &module,
                         const std::string &/* filepath */) {
    using chaiscript::const_var;

    module->add_global_const(const_var(std::string("emission")), "emission");
    module->add_global_const(const_var(std::string("duration")), "duration");
  }

  void registerAttributions(chaiscript::ModulePtr &module,
                            const std::string &filepath) {
    using chaiscript::fun;
    using chaiscript::boxed_cast;

    using chaiscript::Boxed_Value;
    using Map = std::map<std::string, Boxed_Value>;

    module->add(fun([filepath] (config::option::States &conv, const Map &orig) {
      for (auto &pair : orig) {
        auto inner_orig
          = boxed_cast<std::map<std::string, Boxed_Value> &>(pair.second);

        conv[pair.first] = std::make_shared<config::StateConfig>(filepath);

        std::get<decltype("duration"_t)>(*conv[pair.first])
          = boxed_cast<config::DurationConfigPtr>(inner_orig["duration"]);

        std::get<decltype("emission"_t)>(*conv[pair.first])
          = boxed_cast<config::ModelConfigPtr>(inner_orig["emission"]);
      }
    }), "=");
  }

  void registerConcatenations(chaiscript::ModulePtr &module,
                              const std::string &/* filepath */) {
    using chaiscript::fun;

    module->add(fun([] (const std::string &lhs, const std::string &rhs) {
      return lhs + " | " + rhs;
    }), "|");

    module->add(fun([] (const std::string &lhs, const std::string &rhs) {
      return rhs + " | " + lhs;
    }), "->");
  }
};

// Implementation of DependencyTreeParser::makeModelConfig()
// to solve cyclic dependency with Interpreter
config::ModelConfigPtr
DependencyTreeParser::makeModelConfig(std::string filepath) {
  return interpreter_->evalModel(filepath).model_config_ptr;
}

}  // namespace lang

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                      MAIN
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

int main(int argc, char **argv) try {
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
catch(chaiscript::exception::eval_error &e) {
  std::cerr << e.pretty_print() << std::endl;
}
catch(std::exception &e) {
  std::cerr << e.what() << std::endl;
}
