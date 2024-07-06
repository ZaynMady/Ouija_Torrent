#include "Ouija/bdecode.h"
#include <iostream>
#include <cstring>
#include "Ouija/encoder.h"
#include "Ouija/encrypter.h"
#include <fstream>
#include <winsock.h>

//defining client name and version
#define CLIENT_NAME "Ouija"
#define CLIENT_VERSION "0.01"


int main(int argc, char * argv[])
{
    if(argc != 3)
    {
        std::cout << "Invalid number of arguments, please Enter in Command + File_Name\n";
    }
    else
    {
        if(std::strcmp(argv[1], "decode") == 0)
        {
            //getting the file 
            std::string file_path = argv[2];
            auto dict = ouija::decode(file_path);
            ouija::write_info_file(dict);
        }
        else if(std::strcmp(argv[1], "compute_hash") == 0)
        {
            std::string filepath = argv[2];
            auto decoded_data = ouija::decode(filepath);
            auto info_dict = decoded_data["info"].get_dict();
            auto info_hash = encrpyt_info_hash(info_dict);
            std::cout << info_hash;
        }
        else
        {
            std::cout << "Unexpected Command\n";
        }
    }


}

    

