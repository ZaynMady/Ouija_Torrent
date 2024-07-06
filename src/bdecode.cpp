#include "Ouija/bdecode.h"



std::string ouija::decode_str(std::vector<unsigned char> encoded_data, size_t & position)
{
    //initializing the variable which will hold the decoded string to return it to the user later
    std::string decoded_data; 
    //initializing the variable which will count the number of chars in the string
    std::string string_size;
    //colon position
    size_t colon;
    while(encoded_data[position] != ':')
    {
        string_size += encoded_data[position];
        position++;
    }
    //intializing the colon
    colon = position;
    //skip the : colon
    position++;

    //iterating through the rest of the string
    for(; position <= colon + std::stoi(string_size); position++)
    {
        decoded_data += encoded_data[position];
    }

    //returning data
    return decoded_data;
}

long ouija::decode_int(std::vector<unsigned char> encoded_data, size_t & position)
{
    //intializing the number variable which will hold the data and will be returned to the user
    std::string decoded_data;

    if(encoded_data[position] == 'i')
    {
        //skipping the I
        position++; 
    }

    while(encoded_data[position] != 'e')
    {
        decoded_data += encoded_data[position];
        position++;
    }

    //skipping the e
    position++;

    return std::stoi(decoded_data);
}

std::vector<unsigned char> ouija::decode_binary(std::vector<unsigned char> encoded_data, size_t & position)
{
        //initializing the variable which will hold the decoded string to return it to the user later
    std::vector<unsigned char> decoded_data; 
    //initializing the variable which will count the number of chars in the string
    std::string string_size;
    //colon position
    size_t colon;
    while(encoded_data[position] != ':')
    {
        string_size += encoded_data[position];
        position++;
    }
    //intializing the colon
    colon = position;
    //skip the : colon
    position++;

    //iterating through the rest of the string
    for(; position <= colon + std::stoi(string_size); position++)
    {
        decoded_data.push_back(encoded_data[position]);
    }

    //returning data
    return decoded_data;
}

std::map<std::string, bencodeelement> ouija::decode_dict(std::vector<unsigned char> &encoded_data, size_t & position)
{
    //map
    std::map<std::string, bencodeelement> decoded_element;
    //skip the d
    position++;

    //the key is always a string
    while(encoded_data[position] != 'e')
    {
    std::string key = ouija::decode_str(encoded_data, position);
    if (encoded_data[position] == 'i')
    {
        decoded_element[key] = ouija::decode_int(encoded_data, position);
    }
    else if (encoded_data[position] == 'd')
    {
        decoded_element[key] = ouija::decode_dict(encoded_data, position);
    }
    else if(encoded_data[position] == 'l')
    {
        decoded_element[key] = ouija::decode_list(encoded_data, position);
    }
    else if(isdigit(encoded_data[position]))
    {
        if(key == "pieces")
        {
            decoded_element[key] = ouija::decode_binary(encoded_data, position);
            
        }
        else
        {
            decoded_element[key] = ouija::decode_str(encoded_data, position);
        }
    }
    else
    {
        throw std::runtime_error("Incorrect Bencode Format");
    }
    }
    //skip the e
    position++;
    return decoded_element;
}

std::vector<bencodeelement> ouija::decode_list(std::vector<unsigned char> encoded_data, size_t &position)
{
    std::vector<bencodeelement> decoded_data;

    //skip the l
    position++;

    while(encoded_data[position] != 'e')
    {
        if (encoded_data[position] == 'i')
    {
        decoded_data.push_back(ouija::decode_int(encoded_data, position));
    }
    else if (encoded_data[position] == 'd')
    {
        decoded_data.push_back(ouija::decode_dict(encoded_data, position));
    }
    else if(encoded_data[position] == 'l')
    {
        decoded_data.push_back(ouija::decode_list(encoded_data, position));
    }
    else if(isdigit(encoded_data[position]))
    {
            decoded_data.push_back(ouija::decode_str(encoded_data, position));
        
    }
    else
    {
        throw std::runtime_error("Incorrect Bencode Format");
    }
    }
    
    //skip the e
    position++;


    return decoded_data;
}

#include <fstream>

std::map<std::string, bencodeelement> ouija::decode(std::string &file_path)
{
    std::map<std::string, bencodeelement> _returndict;
    size_t position = 0;

    std::ifstream File(file_path, std::ios::binary);
    if (!File)
    {
        throw std::runtime_error("Unable to Open File");
    }
    else
    {
            std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(File), {});
             _returndict = ouija::decode_dict(buffer, position);
    }

    return _returndict;
    File.close();
}

void ouija::write_info_file(std::map<std::string, bencodeelement> decoded_dict)
{
    //information file 

    std::map<std::string, long> files_and_sizes; 
    std::string file_name;
    int piece_length; 
    std::string pieces;
    std::string encoding;
    std::vector<std::string> files; 
    std::vector<std::string> alternative_trackers;
    std::string tracker;
    std::string comment;


    
    //OPERATING STUFF ON IT 
            
            if(decoded_dict.find("comment") != decoded_dict.end())
            {
                comment = decoded_dict["comment"].get_string();
            }
            if(decoded_dict.find("encoding") != decoded_dict.end())
            {
                encoding = decoded_dict["encoding"].get_string();
            }
            if(decoded_dict.find("announce") != decoded_dict.end())
            {
                tracker = decoded_dict["announce"].get_string();
            }
            if(decoded_dict.find("announce-list") != decoded_dict.end())
            {
            for (auto url : decoded_dict["announce-list"].get_list())
            {
                alternative_trackers.push_back(url.get_list()[0].get_string());
            }
            }
            auto info = decoded_dict["info"].get_dict();
            if(info.find("files") != info.end())
            {            auto size_and_path = info["files"].get_list();

            for (auto _ : size_and_path)
            {
                auto dict = _.get_dict();
                files.push_back(dict["path"].get_list()[0].get_string());
                files_and_sizes[dict["path"].get_list()[0].get_string()] = dict["length"].get_int();
            }}
            if(info.find("name") != info.end())
            {
                file_name = info["name"].get_string();
            }
            if(info.find("pieces") != info.end())
            {
                pieces = info["pieces"].get_string();
            }
            if(info.find("piece length") != info.end())
            {
                piece_length = info["piece"].get_int();
            }
    
    std::ofstream info_file(file_name + "-info.txt");

    info_file << "File Name: " << file_name << '\n';
    info_file << "comment: " << comment << std::endl;
    info_file << "Tracker: " << tracker << "\n";
    info_file << "List of Alternative Trackers \n {";

    for (auto elem: alternative_trackers)
    {
        info_file << elem << std::endl;
    }
    info_file << "}\n";

    info_file << "Paths and their respective Sizes: { \n";
    for (auto elem: files_and_sizes)
    {
        info_file << "Path: " << elem.first << std::endl;
        info_file << "Size: " << elem.second << std::endl;
        info_file << std::endl;
    }
    info_file << "}\n";

    info_file.close();
}