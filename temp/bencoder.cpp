#include "Ouija/bencoder.h"
#include "Ouija/bencodeelement.h"
#include <string> 
#include <iostream> 
#include <vector> 
#include <map>
#include <variant> 
#include <typeinfo>
#include <fstream>
#include <sstream>

//decoding a string token in bencode
bencodeelement bencoder::bdecodestr(std::vector<unsigned char>& bencoded_data, size_t & position)
{
    size_t colon;
    std::string num_of_chars;
    //finding the colon 
    for(;bencoded_data[position] != ':'; position++)
    {
        num_of_chars += bencoded_data[position];
    }
    //skipping the :
    position++;
    //getting the number of chars; 
    int int_num_of_chars = std::stoi(num_of_chars);
    //ending position 
    size_t ending_position = position + int_num_of_chars;
    //temp str
    std::string temp_str;

    for (; position < ending_position; position++)
    {
        temp_str += bencoded_data[position];
    }
    
    bencodeelement decodedelement(temp_str);

    return decodedelement;
}
//decoding an int token in bencode
bencodeelement bencoder::bdecodeint(std::vector<unsigned char>& bencoded_data, size_t & position)
{
    //skip the i
    position++;
    //initializing a temp variable that holds the str integer
    std::string str_integer; 
    
    for(; bencoded_data[position] != 'e'; position++)
    {
        str_integer += bencoded_data[position];
    }

    position++; //skip the e
    bencodeelement decoded_element(std::stoi(str_integer));
    return decoded_element;
}
//decoding a list token in bencode
std::vector<bencodeelement> bencoder::bdecodelist(std::vector<unsigned char>& bencoded_data, size_t &  position)
{
    std::vector<bencodeelement> decoded_data;
    ++position; // Skip 'l'
    while (bencoded_data[position] != 'e') {
        if (isdigit(bencoded_data[position])) {
            bencodeelement decoded_element(bdecodestr(bencoded_data, position));
            decoded_data.push_back(decoded_element);
        } else if (bencoded_data[position] == 'i') {
            bencodeelement decoded_element(bdecodeint(bencoded_data, position));
            decoded_data.push_back(decoded_element);
        }
          else if (bencoded_data[position] == 'd')
          {
            bencodeelement decode_element(bdecodedict(bencoded_data, position));
            decoded_data.push_back(decode_element);
          }
          else if (bencoded_data[position] == 'l')
          {
            bencodeelement decode_element(bdecodelist(bencoded_data, position));
            decoded_data.push_back(decode_element);
          }
    }
    ++position; // Skip 'e'
    return decoded_data;
}
//decoding a map token in bencode
std::map<std::string, bencodeelement> bencoder::bdecodedict(std::vector<unsigned char>& bencoded_data, size_t &  position) {
    std::map <std::string, bencodeelement> decoded_dict;
    position++; //skip the d

    while(bencoded_data[position] != 'e')
    {
        std::string key = bdecodestr(bencoded_data, position).get_string();
        long pieces_length;

        //if it is the pieces dict
        if(key == "pieces")
        {
            pieces_length = decoded_dict["piece length"].get_int();
            bencodeelement decoded_element(bdecodebinary(bencoded_data, position, pieces_length));
            decoded_dict[key] = decoded_element;
        }
        //if it detected an int token
        if(bencoded_data[position] == 'i')
        {
            bencodeelement decoded_element(bdecodeint(bencoded_data, position));
            decoded_dict[key] = decoded_element;
        }
        //if it detected a string token
        else if (isdigit(bencoded_data[position]))
        {
            bencodeelement decoded_element(bdecodestr(bencoded_data, position));
            decoded_dict[key] = decoded_element ;
        }
        //if it is a list
        else if(bencoded_data[position] == 'l')
        {
            bencodeelement decoded_element(bdecodelist(bencoded_data, position)); 
            decoded_dict[key] = decoded_element;
        }
        else if (bencoded_data[position] == 'd')
        {
            bencodeelement decoded_element( bdecodedict(bencoded_data, position));
            decoded_dict[key] = decoded_element;
        }

    }

    position++; //skip the e
    return decoded_dict;
}
//decoding pieces token in bencode as a binary datatype
unsigned char * bencoder::bdecodebinary(std::vector<unsigned char>& bencoded_data, size_t & position, long pieces_length)
{
    unsigned char pieces[pieces_length];
    for (int i = 0; i < pieces_length; i++)
    {
        pieces[i] = bencoded_data[position];
        position++;
    }

    return pieces;

}


//THE BENCODER WORKS FOR BENCODING DATA BACK TO A .TORRENT FORMAT
std::string bencoder::bencodingstr(std::string decoded_data)
{
    std::string bencoded_data; 
    bencoded_data += std::to_string(decoded_data.size()) + ":" + decoded_data;
    return bencoded_data;
}

std::string bencoder::bencodingint(int decoded_data)
{
    std::string bencoded_data;
    bencoded_data += "i" + std::to_string(decoded_data) + "e";
    return bencoded_data;
}

std::string bencoder::bencodinglist(std::vector<bencodeelement> decoded_data)
{
    std::string bencoded_data;
    bencoded_data += "l";

    for(auto elem: decoded_data)
    {
        if(elem.is_int())
        {
            bencoded_data += bencodingint(elem.get_int());
        }
        if(elem.is_str())
        {
            bencoded_data += bencodingstr(elem.get_string());
        }
        if(elem.is_list())
        {
            bencoded_data += bencodinglist(elem.get_list());
        }
        if(elem.is_dict())
        {
            bencoded_data += bencodingdict(elem.get_dict());
        }
    }

    bencoded_data += "e";

    return bencoded_data;
}

std::string bencoder::bencodingdict(std::map<std::string, bencodeelement> decoded_data)
{
    std::string bencoded_data;
    bencoded_data += 'd';

    for(auto elem: decoded_data)
    {
        bencoded_data += bencodingstr(elem.first);
        if(elem.second.is_int())
        {
            bencoded_data += bencodingint(elem.second.get_int());
        }
        if(elem.second.is_str())
        {
            bencoded_data += bencodingstr(elem.second.get_string());
        }
        if(elem.second.is_list())
        {
            bencoded_data += bencodinglist(elem.second.get_list());
        }
        if(elem.second.is_dict())
        {
            bencoded_data += bencodingdict(elem.second.get_dict());
        }
    }



    bencoded_data += 'e';
    return bencoded_data;
}


//decoding a string and returning a bunch of elements
std::vector <bencodeelement> bencoder::decode(std::vector<unsigned char> & bencoded_data){
    size_t position = 0; 
    std::vector<bencodeelement> decoded_data;
    while (position < bencoded_data.size())
    {
    if(bencoded_data[position] == 'i')
    {
        bencodeelement decodedelement(bdecodeint(bencoded_data, position));
        decoded_data.push_back(decodedelement);
    }
    else if (bencoded_data[position] == 'l')
    {
        bencodeelement decodedelement(bdecodelist(bencoded_data, position));
        decoded_data.push_back(decodedelement);
    }
    else if (isdigit(bencoded_data[position]))
    {
        bencodeelement decodedelement(bdecodestr(bencoded_data, position));
        decoded_data.push_back(decodedelement);
    }
    else if (bencoded_data[position] == 'd')
    {
        bencodeelement decodedelement(bdecodedict(bencoded_data, position));
        decoded_data.push_back(decodedelement);
    }
    }

    return decoded_data;

}
//turning a torrent file into a string character
std::vector<unsigned char> bencoder::torrent_to_string(std::string & filepath)
{
    std::ifstream File(filepath, std::ios::binary);
    if (!File)
    {
        throw std::runtime_error("Unable to Open File");
    }
    else
    {
        //getting the size of the file
        std::streamsize size = File.tellg();
        File.seekg(0, std::ios::beg);

        //storing it into a vector of unsigned char
        std::vector <unsigned char> buffer(size);
        if(!File.read(reinterpret_cast<char *>(buffer.data()), size))
        {
            std::cerr << "Failed to read File: " << filepath << std::endl;
            File.close();
        }

            File.close();
            return buffer;
    }
}
//creating a text file containing all meta data
void bencoder::create_info_file(std::string & filepath)
{
    auto bencodeddata = torrent_to_string(filepath);

    //decoding the data into a list of decoded bencodeelements 
    std::vector<bencodeelement> decoded_data = decode(bencodeddata);



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
    for (auto elem: decoded_data)
    {
        if (elem.is_dict())
        {
            auto decoded_dict = elem.get_dict();
            
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
        }

    }

    std::ofstream info_file(file_name + ".txt");

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

}

