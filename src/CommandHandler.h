#include <vector>
#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

namespace CommandHandler
{
  void handle(std::string command, std::vector<std::string> output, int client_socket);
}

#endif
