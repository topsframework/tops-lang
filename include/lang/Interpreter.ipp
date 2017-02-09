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
#include <memory>
#include <vector>
#include <string>

// Internal headers
#include "lang/Util.hpp"
#include "lang/ModelConfigRegister.hpp"

#include "config/BasicConfig.hpp"

namespace lang {

/*----------------------------------------------------------------------------*/
/*                              CONCRETE METHODS                              */
/*----------------------------------------------------------------------------*/

template<typename Config, typename Option>
decltype(auto) Interpreter::getConfigOption(const std::string &filepath) {
  auto root_dir = extractDir(filepath);

  std::vector<std::string> modulepaths;
  std::vector<std::string> usepaths { root_dir };

  chaiscript::ChaiScript chai(modulepaths, usepaths);
  chai.add(makeInterpreterLibrary(filepath));

  auto cfg = Config::make(filepath);
  cfg->accept(ModelConfigRegister(chai));

  try { chai.eval_file(filepath); }
  catch (const std::exception &e) {
    // Explicitly ignore missing object exceptions
    if (!missingObjectException(e)) throw;
  }

  return std::get<Option>(*cfg.get());
}

/*----------------------------------------------------------------------------*/

template<typename Config>
std::shared_ptr<Config> Interpreter::fillConfig(const std::string &filepath) {
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

/*----------------------------------------------------------------------------*/

}  // namespace lang
