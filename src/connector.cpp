#include "Ouija/connector.h"
#include "Ouija/bdecode.h"
#include <iostream>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp>


int ouija::initializeWinsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }
    return 0;
}

SOCKET ouija::createAndConnectSocket(const char *ip_address, int port_id)
{
    SOCKET ConnectSocket = INVALID_SOCKET;
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return INVALID_SOCKET;
    }

    struct sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(ip_address); 
    clientService.sin_port = htons(port_id);

    int result = connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService));
    if (result == SOCKET_ERROR) {
        std::cerr << "Unable to connect to server: " << WSAGetLastError() << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }

    return SOCKET(ConnectSocket);
}

// Function to send and recieve handshakes

bool ouija::sendhandshake(SOCKET socket, const std::string& infoHash, const std::string& peerId) {
    if (infoHash.size() != 20 || peerId.size() != 20) {
        std::cerr << "Incorrect sizes, info_hash and peer_id must be 20 bytes each." << std::endl;
        return false;
    }

    std::vector<char> handshake;
    handshake.push_back(HANDSHAKE_PSTRLEN);
    handshake.insert(handshake.end(), HANDSHAKE_PSTR, HANDSHAKE_PSTR + HANDSHAKE_PSTRLEN);
    handshake.insert(handshake.end(), HANDSHAKE_RESERVED, HANDSHAKE_RESERVED + 8);
    handshake.insert(handshake.end(), infoHash.begin(), infoHash.end());
    handshake.insert(handshake.end(), peerId.begin(), peerId.end());

    int result = send(socket, handshake.data(), handshake.size(), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        closesocket(socket);
        WSACleanup();
        return false;
    }

    std::cout << "Handshake sent.\n";
    return true;
}

bool ouija::receiveHandshake(SOCKET socket, std::string& infoHash, std::string& peerId) {
    const int handshakeSize = 1 + HANDSHAKE_PSTRLEN + 8 + 20 + 20;
    std::vector<char> buffer(handshakeSize);
    
    int result = recv(socket, buffer.data(), handshakeSize, 0);
    if (result == SOCKET_ERROR || result < handshakeSize) {
        std::cerr << "Receive failed or incomplete: " << WSAGetLastError() << std::endl;
        closesocket(socket);
        WSACleanup();
        return false;
    }

    std::cout << "Handshake received.\n";

    infoHash.assign(buffer.begin() + 28, buffer.begin() + 48);
    peerId.assign(buffer.begin() + 48, buffer.end());

    return true;
}


//write callback
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
// function to get peers needed from tracker URL
std::vector<std::map<std::string, int>> ouija::getPeersFromTracker(std::string tracker_url, std::string info_hash, std::string peer_id)
{
    CURL* curl;
    CURLcode res;
    std::vector <unsigned char> readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::string url = tracker_url + "?info_hash=" + curl_easy_escape(curl, info_hash.c_str(), info_hash.length())
                          + "&peer_id=" + curl_easy_escape(curl, peer_id.c_str(), peer_id.length())
                          + "&port=6881&uploaded=0&downloaded=0&left=0&compact=1&event=started";

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            return {};
        }
    }

    // Parse the bencoded response
    std::vector<std::pair<std::string, int>> peers;
    try {
        size_t position = 0;
        auto response = ouija::decode_dict(readBuffer, position);
        auto peer_list = response["peers"].get_list();
        for (const auto& peer : peer_list) {
            std::string ip = peer["ip"];
            int port = std::stoi(peer["port"]);
            peers.emplace_back(ip, port);
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse bencoded response: " << e.what() << std::endl;
        return {};
    }

        return std::vector<std::map<std::string, int>>();
}