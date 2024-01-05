#include "Socket.h"
#include <sys/socket.h>
#include <iostream>
#include "Protocol.h"
#include "Utils.h"
#include "CommandHandler.h"
#include <cstring>

namespace Socket
{
  int send_message(int client_socket, std::string message)
  {
    const char *message_serialized = Protocol::serialize_message(message);
    if (raw_send(client_socket, message_serialized, strlen(message_serialized)) == -1)
    {
      std::cerr << "Error on send";
      return -1;
    }
    return 0;
  }

  int parse_message(int client_socket, char message[])
  {
    std::vector<std::string> output = Protocol::deserialize_message(message);
    std::string command(Utils::to_lower_case(output[2]));
    CommandHandler::handle(command, output, client_socket);
    return 0;
  }

  int raw_send(int client_socket, const void *message, size_t message_size) {
    return send(client_socket, message, message_size, 0);
  }
} // namespace Socket
