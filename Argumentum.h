/*
 * Copyright (c) 2023, ke0na
 *
 * Version: 1.0.0
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <algorithm>
#include <iostream>
#include <iterator>
#include <ostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef ARGUMENTUM_ARGUMENTUM_H
#define ARGUMENTUM_ARGUMENTUM_H

namespace Argumentum {

enum OptionType { MANDATORY, OPTIONAL };

struct Option {
  char value;
  std::string alternative_value;
  std::string description;
  OptionType option_type;
  bool has_argument;
  std::string option_description;
  bool argument_optional;

  bool is_valid() { return value != '\0' || !alternative_value.empty(); };
};

struct ArgumentContext {

  struct Flag {
    char identifier;
    std::string alternative_value;
    std::string argument_value;
  };

  std::vector<Flag> flags{};
  std::vector<std::string> operands{};

  bool includes_flag(const std::string flag_to_check) {
    auto match = std::find_if(
        flags.begin(), flags.end(), [&flag_to_check](const Flag flag) {
          return !std::string{flag.identifier}.compare(flag_to_check) ||
                 !std::string{flag.alternative_value}.compare(flag_to_check);
        });

    return match != flags.end();
  }

  Flag get_flag(const std::string flag_to_check) {
    auto match = std::find_if(
        flags.begin(), flags.end(), [&flag_to_check](const Flag flag) {
          return !std::string{flag.identifier}.compare(flag_to_check) ||
                 !std::string{flag.alternative_value}.compare(flag_to_check);
        });

    return *match;
  }

  friend std::ostream &operator<<(std::ostream &stream,
                                  ArgumentContext context) {

    stream << "Argumentum::ArgumentContext {" << std::endl;

    stream << "  flags: [" << std::endl;
    for (const auto flag : context.flags) {

      stream << "    ";
      if (flag.identifier != '\0')
        stream << "-" << flag.identifier;
      if (flag.identifier != '\0' && !flag.alternative_value.empty())
        stream << "|";
      if (!flag.alternative_value.empty())
        stream << "--" << flag.alternative_value;
      if (!flag.argument_value.empty())
        stream << " : " << flag.argument_value;

      stream << std::endl;
    }
    stream << "  ]" << std::endl;

    stream << "  operands: [ ";
    for (auto operand : context.operands)
      stream << operand << " ";

    stream << "]" << std::endl;
    stream << "}" << std::endl;
    return stream;
  }
};

class ArgumentHandler {

public:
  ArgumentHandler() = default;
  virtual int handle(Argumentum::ArgumentContext &context) = 0;
};

class Parser {

private:
  std::string m_tool_name;
  std::string m_description;
  std::string m_help_command{"--help"};
  ArgumentHandler *m_handler;

  std::vector<Option> m_options{};

  int print_help(int argc, char *argv[]) {

    std::cout << m_tool_name << std::endl
              << std::endl
              << m_description << std::endl
              << std::endl;

    std::cout << "Usage: " << std::regex_replace(argv[0], std::regex("./"), "")
              << " ";

    std::vector<Option> flags_optional{};
    std::copy_if(m_options.begin(), m_options.end(),
                 std::back_inserter(flags_optional), [](Option &option) {
                   return option.value &&
                          option.option_type == OptionType::OPTIONAL &&
                          !option.has_argument;
                 });

    std::string flags_optional_usage;
    for (auto flag : flags_optional)
      flags_optional_usage += flag.value;

    std::cout << "[-" << flags_optional_usage << "]";

    std::vector<Option> flags_optional_with_argument{};
    std::copy_if(m_options.begin(), m_options.end(),
                 std::back_inserter(flags_optional_with_argument),
                 [](Option &option) {
                   return option.value &&
                          option.option_type == OptionType::OPTIONAL &&
                          option.has_argument;
                 });

    for (auto flag : flags_optional_with_argument)
      std::cout << "[-" << flag.value << " " << flag.option_description << "]";

    std::vector<Option> flags_mandatory_with_argument{};
    std::copy_if(m_options.begin(), m_options.end(),
                 std::back_inserter(flags_mandatory_with_argument),
                 [](Option &option) {
                   return option.value &&
                          option.option_type == OptionType::MANDATORY &&
                          option.has_argument;
                 });

    std::cout << " ";
    for (auto flag : flags_mandatory_with_argument)
      std::cout << "-" << flag.value << " " << flag.option_description;

    std::cout << "[operands...]";

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "Options:" << std::endl;

    auto get_indents = [](int indent) {
      std::string indent_string;
      for (int i = 0; i < indent; i++)
        indent_string += " ";

      return indent_string;
    };

    auto get_alternative_value = [](Option &option) -> std::string {
      if (!option.alternative_value.empty() && option.value)
        return ", --" + option.alternative_value;

      if (!option.alternative_value.empty())
        return "--" + option.alternative_value;

      return "";
    };

    auto get_value = [](Option &option) -> std::string {
      return option.value ? "-" + std::string{option.value} : "";
    };

    for (auto option : m_options) {

      std::cout << get_indents(2);

      auto alternative_value = get_alternative_value(option);
      auto option_value = get_value(option);
      auto option_pre_face = option_value + alternative_value;
      if (!option.value)
        option_pre_face = get_indents(4) + option_pre_face;

      std::cout << option_pre_face;

      auto length_gap = 30 - option_pre_face.size();
      std::cout << get_indents(length_gap);
      std::cout << option.description << std::endl;
    }

    return 0;
  }

  Option find_option_by_value(char &option_value) {
    auto matched_option =
        std::find_if(m_options.begin(), m_options.end(),
                     [&option_value](Option &registered_option) {
                       return registered_option.value == option_value;
                     });
    if (matched_option != m_options.end()) {
      return *matched_option;
    }
    return {};
  };

  Option
  find_option_by_alternative_value(std::string &option_alternative_value) {
    auto matched_option =
        std::find_if(m_options.begin(), m_options.end(),
                     [&option_alternative_value](Option &registered_option) {
                       return !registered_option.alternative_value.compare(
                           option_alternative_value);
                     });
    if (matched_option != m_options.end()) {
      return *matched_option;
    }
    return {};
  };

public:
  Parser() = default;

  void register_option(const Option &option) { m_options.push_back(option); };
  void set_handler(ArgumentHandler *handler) { m_handler = handler; };
  void set_description(const std::string &description) {
    m_description = description;
  };
  void set_tool_name(const std::string &tool_name) { m_tool_name = tool_name; };
  void override_help_command(const std::string &help_command) {
    m_help_command = help_command;
  };

  int handle_error(std::string errorenous_flag) {
    std::cout << "Option not recognized: ";
    std::cout << errorenous_flag << std::endl;
    return 1;
  };

  int run(int argc, char *argv[]) {

    if (argc >= 2 && !std::string{argv[1]}.compare(m_help_command)) {
      print_help(argc, argv);
      return 0;
    }

    ArgumentContext context;
    int last_matched_option = 0;

    for (int index = 0; index < argc; index++) {

      auto argument = argv[index];

      if (argument && argument[0] == '-') {

        auto next_char = argument[1];
        auto next_next_char = argument[2];
        std::string alternative_value;
        auto is_alternative_value_flag = next_char == '-';
        if (is_alternative_value_flag) {
          alternative_value = std::string{argument}.substr(2, strlen(argument));
        } else {
          auto argument_without_dash =
              std::string{argument}.substr(1, strlen(argument));
          for (int index = 0; index < argument_without_dash.length(); index++) {
            auto actual_char = argument_without_dash[index];
            auto matched_option = find_option_by_value(actual_char);
            if (matched_option.is_valid() &&
                matched_option.has_argument == false) {
              context.flags.push_back(
                  {.identifier = matched_option.value,
                   .alternative_value = matched_option.alternative_value});
            } else {
              return handle_error(std::string{actual_char});
            }
          }
          last_matched_option = index;
          continue;
        }

        auto matched_option =
            !alternative_value.empty()
                ? find_option_by_alternative_value(alternative_value)
                : find_option_by_value(next_char);
        if (matched_option.is_valid()) {
          if (matched_option.has_argument) {
            auto argument_value = argv[index + 1];
            context.flags.push_back(
                {.identifier = matched_option.value,
                 .alternative_value = matched_option.alternative_value,
                 .argument_value = argument_value});
            index++;
          } else {
            context.flags.push_back(
                {.identifier = matched_option.value,
                 .alternative_value = matched_option.alternative_value});
          }
          last_matched_option = index;
          continue;
        }

        if (!alternative_value.empty())
          return handle_error("--" + alternative_value);

        return handle_error("-" + std::string{next_char});
      }
    }

    for (int operands_index = last_matched_option + 1; operands_index < argc;
         operands_index++) {
      auto operand = argv[operands_index];
      context.operands.push_back(operand);
    }

    return m_handler->handle(context);
  };
};

} // namespace Argumentum

#endif // ARGUMENTUM_ARGUMENTUM_H
