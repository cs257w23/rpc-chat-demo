#include "chat.h"
#include <iostream>
#include <string>

using namespace std;

void cmd(string, chat::Node&);

int main(int argc, char *argv[]) {
    int port {8080};
    string ip {"127.0.0.1"};
    if (argc > 2) {
        ip = argv[1];
        port = stoi(argv[2]);
    }

    chat::Socket sock;
    sock.ip = ip;
    sock.port = port;
    chat::Node n(sock);

    // Main input loop.
    string input;
    while (getline(cin, input)) {
        cmd(input, n);
    }

    return 0;
}

void cmd(string input, chat::Node &node) {
    istringstream stream(input);
    string command;
    stream >> command; 
    if (command == "/connect") {
        string ip;
        string port;
        stream >> ip;
        stream >> port;
        node.connect(ip, stoi(port));
    } else if (command == "/list") {
        string thing_to_list;
        stream >> thing_to_list;
        if (thing_to_list == "neighbors") {
            cout << "Neighbors: " << "\n";
            for (const chat::Socket &neighbor : node.neighbors()) {
                cout << neighbor.ip << ":" << neighbor.port << endl;
            }
        }
    } else if (command == "/identify") {
        string name;
        stream >> name;
        node.setName(name);
    } 
    else {
        node.send(command);
    }
}
