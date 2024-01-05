#include <vector>
#include "Socket.h"
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <chrono>
#include "Config.h"
#include "Protocol.h"
#include "Utils.h"
#include <cstring>

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

  void config(std::vector<std::string> parsed_message, int client_socket)
  {
    std::string command(parsed_message[4]);
    if(command.empty()) {
      Socket::send_message(client_socket, "ERR Unsupported CONFIG parameter");
      return;
    }
    if (command == "get")
    {
      std::string key(parsed_message[6]);
      std::string value = Config::get_config(key);

      if(value.empty()) {
        Socket::send_message(client_socket, "ERR Unsupported CONFIG parameter");
        return;
      }
      std::string message = "*2\r\n";
      Protocol::encode_message(message, key);
      Protocol::encode_message(message, value);
      std::string *message_encoded = new std::string(message);
      Socket::raw_send(client_socket, message_encoded->c_str(), strlen(message_encoded->c_str()));
      return;
    }
    else if (command == "set")
    {
      std::string parameter(parsed_message[6]);
      if (parameter == "timeout")
      {
        Socket::send_message(client_socket, "OK");
      }
      else
      {
        Socket::send_message(client_socket, "ERR Unsupported CONFIG parameter");
      }
    }
    else
    {
      Socket::send_message(client_socket, "ERR Unsupported CONFIG parameter");
    }
  }
} // namespace Commands
