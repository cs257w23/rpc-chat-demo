#include "chat.h"

#include <iostream>
#include <string>
#include <vector>

#include "rpc/client.h"
#include "rpc/server.h"

using namespace std;

chat::Node::Node(const Socket socket)
    : socket(socket), srv(socket.ip, socket.port) {
  cout << "Listening on " << socket.ip << ":" << socket.port << endl;

  // Bindings
  srv.bind("connect",
           [&](std::string ip, uint16_t port) { handle_connection(ip, port); });
  srv.bind("msg",
           [&](std::string sender, chat::Socket node, std::string message) {
             handle_message(sender, node, message);
           });
  srv.bind("neighbors", [&]() { return neighbors(); });

  // non-blocking call, handlers execute on one of the workers
  srv.async_run(8);
}

// Parameters are IP and port to use for connect-back.
void chat::Node::handle_connection(const std::string ip, const uint16_t port) {
  cout << "incoming connection from " << ip << ":" << port << endl;

  connect(ip, port);
}

void chat::Node::connect(std::string ip, uint16_t port) {
  cout << "connect to " << ip << ":" << port << endl;

  bool found = false;
  for (const Connection &conn : connections) {
    if (conn.neighbor.ip == ip && conn.neighbor.port == port) {
      cout << "already connected" << endl;
      return;
    } 
  }

  rpc::client *c = new rpc::client(ip, port);

  // Uses default copy constructor.
  Socket sock;
  sock.ip = ip;
  sock.port = port;
  Connection conn(sock, c);
  connections.push_back(conn);
  c->call("connect", this->socket.ip, this->socket.port);

  std::vector<chat::Socket> network = c->call("neighbors").as<std::vector<chat::Socket>>();

  for (auto &s : network) {
    if (s.ip == this->socket.ip && s.port == this->socket.port) {
      continue;
    }

    connect(s.ip, s.port);
  }
}

vector<chat::Socket> chat::Node::neighbors() {
  std::vector<chat::Connection> conns;
  std::vector<chat::Socket> ret;

  for (auto it = connections.begin(); it != connections.end(); ++it) {
    rpc::client::connection_state cs = it->client->get_connection_state();
    if (cs == rpc::client::connection_state::connected) {
      conns.push_back(*it);
      ret.push_back(it->neighbor);
    } else {
      delete it->client;
    }
  }

  this->connections = conns;
  return ret;
}

void chat::Node::send(const std::string &message) {
  for (const auto &conn : connections) {
    conn.client->send("msg", name, socket, message);
  }
}

void chat::Node::setName(const std::string &name) { this->name = name; }

void chat::Node::handle_message(const std::string &sender,
                                const chat::Socket &node,
                                const std::string &message) {
  cout << sender << "(" << node.ip << ":" << node.port << ")"
       << ": " << message << endl;
}

chat::Node::~Node() {
  for (auto conn : connections) {
    delete conn.client;
  }
}
