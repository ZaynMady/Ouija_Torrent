#include "Ouija/encoder.h"

std::vector<unsigned char> ouija::encode_str(std::string &decoded_data)

{
    std::vector<unsigned char> encoded_data;
    std::string string_size = std::to_string(decoded_data.size());
    for (int i = 0; i < string_size.size(); i++ )
    {
        encoded_data.push_back(string_size[i]);
    }
    encoded_data.push_back(':');
    //encoding the string itself 
    for (int i = 0; i < decoded_data.size(); i++)
    {
        encoded_data.push_back(decoded_data[i]);
    }
    return encoded_data;
}

std::vector<unsigned char> ouija::encode_int(int &decoded_data)
{
    std::vector<unsigned char> encoded_data; 

    encoded_data.push_back('i');

    for (auto _char : std::to_string(decoded_data))
    {
        encoded_data.push_back(_char);
    }

    encoded_data.push_back('e');
    return std::vector<unsigned char>(encoded_data);
}

std::vector<unsigned char> ouija::encode(std::map<std::string, bencodeelement> &decoded_data)
{
    std::vector<unsigned char> encoded_data; 
    

    return std::vector<unsigned char>(encoded_data);
}