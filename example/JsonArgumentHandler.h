#include "Argumentum.h"

class JsonArgumentHandler : public Argumentum::ArgumentHandler {

private:
  typedef std::basic_stringstream<char32_t> u32sstream;
  typedef std::basic_ifstream<char32_t> u32filesstream;

  std::basic_string<char32_t> read_file(std::string file_path);

public:
  JsonArgumentHandler() = default;
  ~JsonArgumentHandler() = default;

  int handle(Argumentum::ArgumentContext &context);
};
