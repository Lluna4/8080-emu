#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <map>
#include <format>
#include <iomanip>

std::map<std::string, char> commands = {{"LXI", 0x01}, {"LXI B", 0x01}, {"STAX B", 0x02}, {"INX B", 0x03}, {"LXI D", 0x11}, {"STAX D", 0x12}, {"INX D", 0x13},
{"LXI H", 0x21}, {"INX H", 0x23},{"LXI SP", 0x31}, {"INX SP", 0x33}};

bool isNumber(std::string a)
{
    for (int i = 0; i < a.length(); i++)
    {
        if (isxdigit(a[i]) == 0)
            return false;
    }
    return true;
}

std::vector<std::string> tokenize(std::string result, char separator = ' ')
{
    std::vector<std::string> tokens;
    int index = 0;
    int a = 0;
    bool inside = false;
    size_t last = 0;
    while (result[index] == ' ' || result[index] == separator)
        index++;
    if (index > -1)
        index--;
    if (index >= 0)
    {
        a = index;
        last = index + 1;
    }
    while (result[a])
    {
        index++;
        a = index;
        if (result[index] == separator || result[index] == '"' || result[index] == '\0')
        {
            if ((result[index] == separator || result[index] == '\0') && inside == false)
            {
                char* buffer = (char*)calloc(static_cast<size_t>(index - last) + 1, sizeof(char));
                if (!buffer)
                    return {};
                memcpy(buffer, &result.c_str()[last], index - last);
                std::string ret = buffer;
                if (ret.size() == 0)
                    break;
                tokens.push_back(ret);
                //last = index;
                while (result[index] == ' ' || result[index] == separator)
                    index++;
                if (result[index] == '"')
                    index--;
                last = index;
                free(buffer);
            }
            else if (result[index] == '"' && inside == false)
            {
                last = index;
                inside = true;
            }
            else if (result[index] == '"' && inside == true)
            {
                last++;
                char* buffer = (char*)calloc(static_cast<size_t>(index - last) + 1, sizeof(char));
                if (!buffer)
                    return {};
                memcpy(buffer, &result.c_str()[last], index - last);
                std::string ret = buffer;
                if (ret.size() == 0)
                    break;
                tokens.push_back(ret);
                index++;
                if (result[index] != '\0')
                {
                    while (result[index] == ' ' || result[index] == separator)
                        index++;
                    if (result[index] == '"')
                        index--;
                    last = index;
                }
                else
                    break;
                free(buffer);
                inside = false;
            }
            else if (result[index] == '\0' && inside == true)
            {
                char* buffer = (char*)calloc(static_cast<size_t>(index - last) + 1, sizeof(char));
                if (!buffer)
                    return {};
                memcpy(buffer, &result.c_str()[last], index - last);
                std::string ret = buffer;
                if (ret.size() == 0)
                    break;
                tokens.push_back(ret);
                free(buffer);
                inside = false;
            }
        }
    }
    return tokens;
}


int main(int argc, char *argv[])
{
    std::vector<char> to_write;

    if (argc < 2)
    {
        std::cout << "\x1B[91mError: no se han proporcionado argumentos suficientes\033[0m\t\t" << std::endl;
        return -1;
    }
    if (std::filesystem::exists(argv[1]) == false)
    {
        std::cout << "\x1B[91mError: el archivo no existe\033[0m\t\t" << std::endl;
        return -1;
    }

    std::string buf;
    std::ifstream infile(argv[1]);
    std::string head;
    while (std::getline(infile, buf))
    {
        std::vector<std::string> valores = tokenize(buf);

        if (valores.size() < 2)
        {
            std::cout << "\x1B[91mError: no suficientes argumentos\033[0m\t\t" << std::endl;
            return -1;
        }

        if (commands.contains(valores[0]))
        {
            std::string comm;
            if (commands[valores[0]] == 0x01)
                comm = std::format("{} {}", valores[0], valores[1]);
            else
                comm = valores[0];
            
            if (commands.contains(comm))
            {
                to_write.push_back(commands[comm]);
            }
            if (valores.size() > 2)
            {
                if (valores[2].back() == 'H')
                {
                    valores[2].pop_back();
                    std::string half = valores[2].substr(0, valores[2].length() / 2);
                    std::string otherHalf = valores[2].substr(valores[2].length() / 2);
                    
                    if (isNumber(half) && isNumber(otherHalf)) 
                    {
                        unsigned int result = std::stoul(half, nullptr, 16);
                        unsigned int result2 = std::stoul(otherHalf, nullptr, 16);
                        
                        to_write.push_back(static_cast<char>(result));
                        to_write.push_back(static_cast<char>(result2));
                    } 
                }
            }
        }
        else if (commands.contains(buf))
        {
            to_write.push_back(commands[buf]);
        }
    }
    std::ofstream outputFile("out.bin", std::ios::binary);
    for (unsigned int i = 0; i < to_write.size(); i++)
    {
        outputFile.write(&to_write[i], sizeof(to_write[i]));
    }
    outputFile.close();

    return 0;
}