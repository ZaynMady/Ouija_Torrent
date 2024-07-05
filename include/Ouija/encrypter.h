#ifndef ENCRYPTER_H
#define ENCRYPTER_H


//Calculating the Info Hash using the OpenSSL library
#include <openssl/sha.h>
#include <vector>
#include <map> 
#include "bencodeelement.h"
#include <string> 

std::string encrpyt_info_hash(std::map<std::string, bencodeelement> info_dict);





#endif //ENCRYPTER_H