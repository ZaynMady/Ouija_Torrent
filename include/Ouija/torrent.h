#ifndef TORRENT_H
#define TORRENT_H


namespace ouija
{
    #include <string>
    #include <map>
    #include <vector>

    class torrent{
private:
    std::string info_hash;
    std::string peer_id;
    std::string tracker_url;
    std::vector<std::map<std::string, int>> peers;

public:
    torrent(std::string &filepath, std::string client_name, std::string client_version);




    };
}




#endif //TORRENT_H