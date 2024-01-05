#include "Utils.h"
#include <iostream>
#include <algorithm>

namespace Utils
{
  void debugRawMessage(char message[])
  {
    std::cout << "New message\n"
              << std::endl;
    std::cout << message << std::endl;
    std::cout << "------------------------" << std::endl;
  }

  void debugParsedMessage(std::vector<std::string> parsed_message)
  {
    std::string command(parsed_message[2]);
    std::string value(parsed_message[4]);
    std::cout << "command: " << std::endl;
    std::cout << command << std::endl;
    std::cout << "value: " << std::endl;
    std::cout << value << std::endl;
    std::cout << "extra args: " << std::endl;
    for (int i = 6; i <= parsed_message.size(); i += 2)
    {
      std::cout << "index: " << i << " " << parsed_message[i] << std::endl;
    }
  }

  std::string to_lower_case(std::string str)
  {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
  }
}
