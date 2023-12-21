# argumentum
Header-only implementation of a simple argument parser. Makes it easy to write user-friendly command-line interfaces with only one header include.


## Starter
With argumentum you are able to create an instance of an `Argumentum::Parser` by simply calling the constructor. The instance is exposing the method `run(argc, argv)` which then calls the handler instance with the parsed `Argumentum::ArgumentContext` instance.


```cpp
// ArgumentHandler.h
#include "Argumentum.h"

class ArgumentHandler: public Argumentum::ArgumentHandler {

public:

  ArgumentHandler() = default;
  ~ArgumentHandler() = default;

  int handle(Argumentum::ArgumentContext &context) override;

};

// ArgumentHandler.cpp
#include "ArgumentHandler.h"

int ArgumentHandler::handle(Argumentum::ArgumentContext &context) {
  // handler for the argument context
  return 0;
};

// main.cpp
#include "Argumentum.h"

int main(int argc, char* argv[]) {

   // logic which interprets the Argumentum::ArgumentContext
   ArgumentHandler argumentHandler;

   // argument parser for registering the options and setting some tool meta information
   Argumentum::Parser parser;
   parser.set_tool_name("ls");
   parser.set_description("list directories and files.");

   // registering the handler for the argument context
   parser.set_handler(argumentHandler);

   // registering the options
   parser.register_option({.value = 'l', 
                           .alternative_value = "list", 
                           .description = "description for the option 'l'", 
                           .option_type = Argumentum::OptionType::OPTIONAL, 
                           .has_argument = false});
   parser.register_option({.value = 'a',
                           .alternative_value = "all", 
                           .description = "description for the option 'a'", 
                           .option_type = Argumentum::OptionType::OPTIONAL, 
                           .has_argument = false});

   // running the instance - parsing the options and invoking the handler instance
   return parser.run(argc, argv);
};

```

Invoking the artefact with the argument `--help` is going to produce the following:

```
ls

list directories and files.

Usage: ls [-la] [operands...]

Options:
  -l, --list                    description for the option 'l'
  -a, --all                     description for the option 'a'
```

## API
### Argumentum::Parser
```
  register_option(const Option);             // registers an option
  set_handler(const ArgumentHandler);        // sets the handler which gets invoked after parsing
  set_description(const std::string);        // sets description for the --help option
  set_tool_name(const std::string);          // sets the title for the --help option
  override_help_command(const std::string);  // override the default --help option value

```

### Argumentum::Option
```
struct Option {
  char value;                                // option value to be machted e.g. 'f' for '-f'
  std::string alternative_value;             // option alternative value to be machted e.g. '--file'
  std::string description;                   // option description mainly used for the --help option
  OptionType option_type;                    // Argumentum::OptionType::MANDATORY | Argumentum::OptionType::OPTIONAL 
  bool has_argument;                         // bool defining if the option has an argument or not
  std::string option_description;            // description for the options argument, mainly used for the --help option
  bool argument_optional;                    // bool defining if the options argument is optional or not
};

```
### Argumentum::ArgumentContext
The argument context is the piece of information passed to the `Argumentum::ArgumentHandler` implementation. This context can be used as a reference on what arguments the user has passed into. 
```
struct Flag {
  char identifier;                           // option value matched e.g. 'f'
  std::string alternative_value;             // option alternative value matched e.g. '--file'
  std::string argument_value;                // option argument value
};

struct ArgumentContext {
  bool includes_flag(const std::string);     // method checking if a flag got set or not, checks the identifier and the alternative value
  Flag get_flag(const std::string);          // method for getting a flag, checks the identifier and the alternative value
};

```

## Error handling
If an option got not registered and cannot be matched then the parser throws an error during the call of the `run(arc, arv)`. The user gets notified with a message like the following

```
Option not recognized: x                    // when the option got not found (also in compound options)
```
