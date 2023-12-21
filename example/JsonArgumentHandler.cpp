#include "JsonArgumentHandler.h"
#include "Argumentum.h"
#include "Jayson.h"
#include <fstream>

std::basic_string<char32_t>
JsonArgumentHandler::read_file(std::string file_path) {
  u32filesstream input_file(file_path);
  u32sstream buffer;

  if (!input_file.good())
    throw std::invalid_argument("File could not be read.");

  buffer << input_file.rdbuf();
  return buffer.str();
};

int JsonArgumentHandler::handle(Argumentum::ArgumentContext &context) {

  if (context.operands.empty()) {
    std::cout << "No file provided" << std::endl;
    return 1;
  }

  if (context.operands.size() > 1) {
    std::cout << "Please provide only one file." << std::endl;
    return 1;
  }

  try {
    auto file_content = read_file(context.operands.at(0));

    Jayson::Parser parser{file_content};
    auto result = parser.parse();

    if (context.includes_flag("dump") || context.includes_flag("d"))
      std::cout << result;

    if (context.includes_flag("f")) {

      auto flag = context.get_flag("f");
      auto flag_argument = flag.argument_value;
      std::stringstream flag_argument_stream{flag_argument};
      std::vector<std::string> tokens{};

      std::string token{};
      while (std::getline(flag_argument_stream, token, ':'))
        tokens.push_back(token);

      if (tokens.size() != 2)
        throw std::invalid_argument(
            "-f argument provided is not correct. Please provide a value in "
            "this format -> key:type");

      auto key = tokens[0];
      auto type = tokens[1];

      auto node_is_of_type = [](std::string type,
                                std::string type_to_check) -> bool {
        return !std::string{type}.compare(type_to_check);
      };

      if (node_is_of_type(type, "string")) {

        auto string_node_optional = result->find_string(key);
        if (string_node_optional.is_empty())
          return 1;

        auto string_node = *string_node_optional.get_value();

        if (context.includes_flag("print-node"))
          std::cout << string_node << std::endl;

        if (context.includes_flag("print-value"))
          std::cout << string_node.get_value() << std::endl;
      }
    }
  } catch (const std::invalid_argument &error) {
    if (context.includes_flag("trace"))
      std::cerr << error.what() << std::endl;
    return 1;
  }

  return 0;
};
