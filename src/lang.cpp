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
#include <string>
#include <fstream>

// Internal headers
#include "lang/Interpreter.hpp"
#include "lang/ModelConfigSerializer.hpp"
#include "lang/DependencyTreeParser.hpp"

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 -------------------------------------------------------------------------------
                                  INTERPRETER
 -------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
*/

namespace lang {


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
    case 3: env.model_config_ptr->accept(lang::ModelConfigSerializer{});
            break;
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
