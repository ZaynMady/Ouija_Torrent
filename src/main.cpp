#include "Ouija/bdecode.h"
#include <iostream>
#include <cstring>
#include <libtorrent/bencode.hpp>


int main(int argc, char * argv[])
{
    if(argc != 3)
    {
        std::cout << "Invalid number of arguments, please Enter in Command + File_Name\n";
    }
    else
    {
        if(std::strcmp(argv[1], "info") == 0)
        {
            //getting the file 
            std::string file_path = argv[2];
            auto dict = decode(file_path);
            write_info_file(dict);
        }
        else
        {
            std::cout << "Unexpected Command\n";
        }

    }


}

    

