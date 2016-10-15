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

#ifndef LANG_DEPENDENCY_TREE_PARSER_
#define LANG_DEPENDENCY_TREE_PARSER_

// Standard headers
#include <string>
#include <vector>

// Internal headers
#include "config/ModelConfig.hpp"
#include "config/DependencyTreeConfig.hpp"

namespace lang {

// Forward declarations
class Interpreter;

/**
 * @class DependencyTreeParser
 * @brief Parser to dependency tree serialized file
 */
class DependencyTreeParser {
 public:
  // Constructors
  DependencyTreeParser(Interpreter* interpreter,
                       std::string root_dir,
                       std::string filename,
                       std::vector<std::string> content);

  // Concrete methods
  config::DependencyTreeConfigPtr parse();

 private:
  // Concrete methods
  void parseNode(std::string line);

  config::ModelConfigPtr makeModelConfig(std::string filepath);

  void resetEdgeIndex();
  unsigned int nextEdgeIndex();

  void parseLevel();
  void parseChild();
  void parseLastChild();

  int parseSpaces();
  std::string parseId();
  std::string parseNumber();
  std::string parseString();

  void consume();
  void consume(char c);

  char next();
  char next(int n);

  // Instance variables
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

#endif  // LANG_DEPENDENCY_TREE_PARSER_
