#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "sstream"
#include <algorithm>
#include <unordered_map>
#include <chrono>


const int MSG_SIZE_MAX = 1024;

std::unordered_map<std::string, std::string> database;
std::unordered_map<std::string, std::chrono::high_resolution_clock::duration> database_expirations;

void set_database_expiration(std::string key, std::chrono::high_resolution_clock::duration expiration) {
  std::cout << "Setting expiration: " << key << std::endl;
  auto expiry_time = std::chrono::high_resolution_clock::now().time_since_epoch() + expiration;
  database_expirations.insert_or_assign(key, expiry_time);
}

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

const char *serialize_message(std::string message)
{
  std::string *serialized_message = new std::string("+" + message + "\r\n");
  return serialized_message->c_str();
}

int send_message(std::string message, int client_socket)
{
  const char *message_serialized = serialize_message(message);
  if (send(client_socket, message_serialized, strlen(message_serialized), 0) == -1)
  {
    std::cerr << "Error on send";
    return -1;
  }
  return 0;
}

void ping_command(std::vector<std::string> parsed_message, int client_socket)
{
  std::string message("PONG");
  send_message(message, client_socket);
}

void echo_command(std::vector<std::string> parsed_message, int client_socket)
{
  std::string message(parsed_message[4]);
  send_message(message, client_socket);
}

void set_command(std::vector<std::string> parsed_message, int client_socket)
{
  std::string key(parsed_message[4]);
  std::string value(parsed_message[6]);
  if(value.empty() || value == "") {
    send_message("INVALID VALUE", client_socket);
    return;
  }
  if(parsed_message[8] == "px" && !parsed_message[10].empty()) {
    int expiration_ms = std::stoi(parsed_message[10]);
    set_database_expiration(key, std::chrono::milliseconds(expiration_ms));
    std::cout << "Expiration set: " << key << std::endl; 
  }
  database.insert_or_assign(key, value);
  send_message("OK", client_socket);
}

void get_command(std::vector<std::string> parsed_message, int client_socket) {
  std::string key(parsed_message[4]);
  std::string value = database[key];
  if(!value.empty()) {
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    if(database_expirations.contains(key) && (now >= database_expirations.at(key))) {
      database.erase(key);
      database_expirations.erase(key);
      std::cout << "Key expired: " << key << std::endl;
      send(client_socket, "$-1\r\n", 5, 0);
      return;
    }
    send_message(value, client_socket);
    return;
  }
  send(client_socket, "$-1\r\n", 5, 0);
}

int parse_message(ssize_t bytes_received, char message[], int client_socket)
{
  // debugRawMessage(message);
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
  debugParsedMessage(output);
  std::string command(to_lower_case(output[2]));
  if (command == "ping")
  {
    output.clear();
    ping_command(output, client_socket);
  }
  else if (command == "echo")
  {
    echo_command(output, client_socket);
  }
  else if (command == "set")
  {
    set_command(output, client_socket);
  }
  else if (command == "get")
  {
    get_command(output, client_socket);
  }
  else
  {
    send_message("Command not found", client_socket);
  }
  return 0;
}

void listen_to_client(int socket_fd)
{
  std::cout << "Client connected\n";
  bool isConnected = true;
  while (isConnected)
  {
    char msg[MSG_SIZE_MAX];
    ssize_t bytes_read = recv(socket_fd, msg, MSG_SIZE_MAX, 0);

    if (bytes_read < 0)
    {
      std::cout << "Client disconneted";
      isConnected = false;
      break;
    }
    else if (bytes_read == 0)
    {
      std::cout << "Client disconnected" << std::endl;
      isConnected = false;
      break;
    }

    if (parse_message(bytes_read, msg, socket_fd) != 0)
    {
      std::cerr << "Error on send";
      break;
    }
    bzero(msg, MSG_SIZE_MAX);
  }
  close(socket_fd);
}

int main(int argc, char **argv)
{
  // You can use print statements as follows for debugging, they'll be visible when running tests.
  std::cout << "Logs from your program will appear here!\n";

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
  {
    std::cerr << "Failed to create server socket\n";
    return 1;
  }

  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0)
  {
    std::cerr << "setsockopt failed\n";
    return 1;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(6379);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
  {
    std::cerr << "Failed to bind to port 6379\n";
    return 1;
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0)
  {
    std::cerr << "listen failed\n";
    return 1;
  }

  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);

  std::cout << "Waiting for a client to connect...\n";

  int new_socket_fd;
  while ((new_socket_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len)) != -1)
  {
    std::thread t(listen_to_client, new_socket_fd);
    t.detach();
  }

  close(server_fd);

  return 0;
}
