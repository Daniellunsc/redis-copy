#include <vector>
#include "Commands.h"
#include "Socket.h"

namespace CommandHandler
{
  void handle(std::string command, std::vector<std::string> output, int client_socket)
  {
    if (command == "ping")
    {
      Commands::ping(output, client_socket);
    }
    else if (command == "echo")
    {
      Commands::echo(output, client_socket);
    }
    else if (command == "set")
    {
      Commands::set(output, client_socket);
    }
    else if (command == "get")
    {
      Commands::get(output, client_socket);
    } else if (command == "config") {
      Commands::config(output, client_socket);
    }
    else
    {
      Socket::send_message(client_socket, "COMMAND NOT FOUND");
    }
  }
}
