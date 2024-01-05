#ifndef SOCKET_H
#define SOCKET_H

#include <string>

namespace Socket {
  int send_message(int client_socket, std::string message);
  int parse_message(int client_socket, char message[]);
  int raw_send(int, const void *, size_t);
}

#endif // SOCKET_H
