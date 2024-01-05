#ifndef PROTOCOL
#define PROTOCOL

#include <string>
#include <vector>

namespace Protocol {
  const char *serialize_message(std::string message);
  std::vector<std::string> deserialize_message(char message[]);
}

#endif // PROTOCOL
