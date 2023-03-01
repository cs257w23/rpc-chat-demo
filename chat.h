#ifndef CHAT_H
#define CHAT_H

#include <sstream>
#include <vector>

#include "rpc/client.h"
#include "rpc/server.h"

namespace chat {

// Structs used for RPC calls cannot have methods.
struct Socket {
  std::string ip;
  uint16_t port;
  MSGPACK_DEFINE_ARRAY(ip, port)
};

// Goal is to prevent access outside Node to anything directly RPC related.
struct Connection {
  Connection(Socket neighbor, rpc::client *client)
      : neighbor(neighbor), client(client) {}
  Socket neighbor;
  rpc::client *client;
};

class Node {
 public:
  // Constructors and destructors
  Node(Socket socket);
  ~Node();

  // Setters
  void setName(const std::string &name);

  // Getters
  std::vector<Socket> neighbors();


  // RPC calls
  void connect(std::string ip, uint16_t port);
  void send(const std::string &message);

  // Handlers. Signatures of these functions must match
  // server bindings.
  void handle_connection(const std::string ip, const uint16_t port);
  void handle_message(const std::string &sender, const Socket &node,
                      const std::string &message);
  std::vector<Socket> handle_neighbors();

 private:
  std::string name;
  Socket socket;
  rpc::server srv;

  // Everyone we are connected to.
  std::vector<Connection> connections;
};

}  // namespace chat

#endif
