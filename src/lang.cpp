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
#include <memory>
#include <cstdlib>
#include <iostream>
#include <exception>

// Internal headers
#include "config/BasicConfig.hpp"
#include "config/ModelConfig.hpp"
#include "config/DiscreteConverter.hpp"
#include "config/StringLiteralSuffix.hpp"
#include "config/DecodableModelConfig.hpp"

#include "lang/Interpreter.hpp"
#include "lang/ModelConfigSerializer.hpp"

// External headers
#include "chaiscript/language/chaiscript_common.hpp"

// Using declarations
using config::operator ""_t;

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
  auto model_cfg = interpreter.evalModel(argv[1]);

  /*--------------------------------------------------------------------------*/
  /*                                CONVERTER                                 */
  /*--------------------------------------------------------------------------*/

  if (argc >= 3) {
    std::vector<config::ConverterPtr> converters {
      std::get<decltype("observations"_t)>(*model_cfg)->makeConverter() };

    auto decodable_model_cfg
      = std::dynamic_pointer_cast<config::DecodableModelConfig>(model_cfg);

    if (decodable_model_cfg) {
      auto &domains
        = std::get<decltype("other_observations"_t)>(*decodable_model_cfg);

      for (auto &domain : domains)
        converters.push_back(domain->makeConverter());

      converters.push_back(
        std::get<decltype("labels"_t)>(*decodable_model_cfg)->makeConverter());
    }

    std::fstream dataset(argv[2]);
    std::string line;

    // Header
    std::getline(dataset, line);
    std::cout << line << std::endl;

    // Data
    while (std::getline(dataset, line)) {
      std::stringstream ss(line);

      bool first = true;
      std::string input;
      for (const auto &converter : converters) {
        std::getline(ss, input, '\t');
        std::cout << (first ? '\0' : '\t') << converter->convert(input);
        first = false;
      }

      std::cout << std::endl;
    }

    std::cout << std::endl;
  }

  /*--------------------------------------------------------------------------*/
  /*                           PRINTER / SERIALIZER                           */
  /*--------------------------------------------------------------------------*/

  switch (argc) {
    case 2: /* fall through */
    case 3: model_cfg->accept(lang::ModelConfigSerializer{}); break;
    case 4: model_cfg->accept(lang::ModelConfigSerializer(argv[3])); break;
  }

  return EXIT_SUCCESS;
}
catch(chaiscript::exception::eval_error &e) {
  std::cerr << e.pretty_print() << std::endl;
}
catch(std::exception &e) {
  std::cerr << e.what() << std::endl;
}
