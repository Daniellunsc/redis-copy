#include <vector>
#ifndef COMMANDS_H
#define COMMANDS_H

namespace Commands{
  void set_database_expiration(std::string key, std::chrono::high_resolution_clock::duration expiration);
  void echo(std::vector<std::string> parsed_message, int client_socket);
  void ping(std::vector<std::string> parsed_message, int client_socket);
  void set(std::vector<std::string> parsed_message, int client_socket);
  void get(std::vector<std::string> parsed_message, int client_socket);
}

#endif
