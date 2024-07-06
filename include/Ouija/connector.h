#ifndef CONNECTOR_H
#define CONNECTOR_H

//Defining the necessary headers
#include <winsock2.h>
#include <ws2tcpip.h>
#include <map>
#include <vector>
#include <string>

//Defining The necessary GLOBAL VARIABLES

#define HANDSHAKE_PSTR "Bittorrent protocol"
#define HANDSHAKE_PSTRLEN 19
#define HANDSHAKE_RESERVED "0/0/0/0/0/0/0/0"




namespace ouija {
int initializeWinsock();

SOCKET createAndConnectSocket(const char* ip_address, int port_id);
std::vector<std::map<std::string, int>> getPeersFromTracker();
bool sendhandshake(SOCKET socket, const std::string &info_hash, const std::string &peer_id);
bool receiveHandshake(SOCKET socket, std::string& infoHash, std::string& peerId);
}


#endif //CONNECTOR_H

