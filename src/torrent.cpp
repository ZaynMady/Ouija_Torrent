#include "Ouija/torrent.h"


namespace ouija{

    #include "Ouija/bdecode.h"
    #include "Ouija/encrypter.h"

    torrent::torrent(std::string &file_path, std::string client_name, std::string client_version)
    {
        auto dict = ouija::decode(file_path);
        tracker_url = dict["announce"].get_string();
        info_hash = encrpyt_info_hash(dict["info"].get_dict());
        peer_id = generate_peerid(client_name, client_name);
    }
}