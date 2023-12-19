#include "Argumentum.h"
#include "Jayson.h"
#include "JsonArgumentHandler.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

int main(int argc, char **argv) {

  auto jsonArgumentHandler = new JsonArgumentHandler();

  Argumentum::Parser parser;
  parser.set_tool_name("Json");
  parser.set_description("RFC8259 compliant JSON parser tool.");
  parser.set_handler(jsonArgumentHandler);

  parser.register_option({
      .value = 'd',
      .alternative_value = "dump",
      .description = "prints the JSON tree.",
      .option_type = Argumentum::OptionType::OPTIONAL,
      .has_argument = false,
  });
  parser.register_option(
      {.value = 'f',
       .alternative_value = "find",
       .description =
           "finds a json node by a given key and type e.g. --find name:string. "
           "Possible types are: string,bool,array,object & number.",
       .option_type = Argumentum::OptionType::OPTIONAL,
       .has_argument = true,
       .option_description = "key:type"});
  parser.register_option({
      .alternative_value = "print-node",
      .description = "print node found by find option.",
      .option_type = Argumentum::OptionType::OPTIONAL,
      .has_argument = false,
  });
  parser.register_option({
      .alternative_value = "print-value",
      .description = "print node value found by find option.",
      .option_type = Argumentum::OptionType::OPTIONAL,
      .has_argument = false,
  });
  parser.register_option({
      .value = 't',
      .alternative_value = "trace",
      .description = "logs out all possible information.",
      .option_type = Argumentum::OptionType::OPTIONAL,
      .has_argument = false,
  });

  return parser.run(argc, argv);
}
