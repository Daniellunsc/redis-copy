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

const int MSG_SIZE_MAX = 1024;

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
      exit(1);
    } else if (bytes_read == 0) {
      isConnected = false;
      break;
    }

    const char *message = "+PONG\r\n";
    if (send(socket_fd, message, strlen(message), 0) == -1)
    {
      std::cerr << "Error on send";
      isConnected = false;
      exit(1);
    }
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
  while((new_socket_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len)) != -1 ) {
    std::thread t(listen_to_client, new_socket_fd);
    t.detach();
  }

  close(server_fd);

  return 0;
}
