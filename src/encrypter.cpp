#include "Ouija/encrypter.h"
#include "Ouija/encoder.h"
#include <openssl/sha.h>
#include <fstream>
#include <iomanip>


std::string encrpyt_info_hash(std::map<std::string, bencodeelement> info_dict)
{
    std::vector <unsigned char> serialized_info = ouija::encode_dict(info_dict);
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(serialized_info.data()), serialized_info.size(), hash);
    
    std::stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}