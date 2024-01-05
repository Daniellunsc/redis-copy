#include <vector>
#include "Socket.h"
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <chrono>

namespace Commands
{
  std::unordered_map<std::string, std::string> database;
  std::unordered_map<std::string, std::chrono::high_resolution_clock::duration> database_expirations;
  // MAYBE IT'S WORTH TO CREATE A DATABASE LAYER LATER
  void set_database_expiration(std::string key, std::chrono::high_resolution_clock::duration expiration)
  {
    std::cout << "Setting expiration: " << key << std::endl;
    auto expiry_time = std::chrono::high_resolution_clock::now().time_since_epoch() + expiration;
    database_expirations.insert_or_assign(key, expiry_time);
  }

  void ping(std::vector<std::string> parsed_message, int client_socket)
  {
    std::string message("PONG");
    Socket::send_message(client_socket, message);
  }

  void echo(std::vector<std::string> parsed_message, int client_socket)
  {
    std::string message(parsed_message[4]);
    Socket::send_message(client_socket, message);
  }

  void set(std::vector<std::string> parsed_message, int client_socket)
  {
    std::string key(parsed_message[4]);
    std::string value(parsed_message[6]);
    if (value.empty() || value == "")
    {
      Socket::send_message(client_socket, "INVALID VALUE");
      return;
    }
    if (parsed_message[8] == "px" && !parsed_message[10].empty())
    {
      int expiration_ms = std::stoi(parsed_message[10]);
      set_database_expiration(key, std::chrono::milliseconds(expiration_ms));
      std::cout << "Expiration set: " << key << std::endl;
    }
    database.insert_or_assign(key, value);
    Socket::send_message(client_socket, "OK");
  }

  void get(std::vector<std::string> parsed_message, int client_socket)
  {
    std::string key(parsed_message[4]);
    std::string value = database[key];
    if (!value.empty())
    {
      auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
      if (database_expirations.contains(key) && (now >= database_expirations.at(key)))
      {
        database.erase(key);
        database_expirations.erase(key);
        std::cout << "Key expired: " << key << std::endl;
        Socket::raw_send(client_socket, "$-1\r\n", 5);
        return;
      }
      Socket::send_message(client_socket, value);
      return;
    }
    Socket::raw_send(client_socket, "$-1\r\n", 5);
  }
} // namespace Commands
