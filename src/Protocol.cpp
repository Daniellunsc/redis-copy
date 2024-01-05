#include "Protocol.h"

namespace Protocol
{
  const char *serialize_message(std::string message)
  {
    std::string *serialized_message = new std::string("+" + message + "\r\n");
    return serialized_message->c_str();
  }

  std::string encode_message(std::string &message, std::string &str_to_encode)
  {
    message += "$" + std::to_string(str_to_encode.length()) + "\r\n" + str_to_encode + "\r\n";
    return message;
  }
  
  std::vector<std::string> deserialize_message(char message[])
  {
    std::string line = "", delimiter = "\r\n";
    int index = 0;
    std::string input(message);
    std::vector<std::string> output;
    while (index < input.length())
    {
      if (input.substr(index, 2) == delimiter)
      {
        output.push_back(line);
        line = "";
        index += 2;
      }
      line += input[index];
      index++;
    }
    return output;
  }
}
