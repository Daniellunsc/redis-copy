#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include <string>

namespace Utils {
  void debugRawMessage(char message[]);
  void debugParsedMessage(std::vector<std::string> parsed_message);
  std::string to_lower_case(std::string str);
}

#endif // UTILS_H
