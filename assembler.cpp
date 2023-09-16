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

std::map<std::string, uint16_t> symbolTable;
uint16_t currentAddress = 0x1000;
uint16_t writeAddress = 0x0000;

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

void assemble(std::vector<char> &to_write, std::vector<std::string> valores, int index, std::string buf = "", uint16_t mode = 0)
{
    uint16_t start_pos;
    if (mode == (uint16_t)0)
        start_pos = writeAddress;
    else
        start_pos = currentAddress;
    if (commands.contains(valores[index]))
    {
        index++;
        if (valores[index].contains(','))
        {
            std::vector<std::string> val = tokenize(valores[index], ',');
            std::string command = std::format("{} {}", valores[index - 1], val[0]);
            if (val[1].back() == 'H')
            {
                val[1].pop_back();
                std::string half = val[1].substr(0, val[1].length() / 2);
                std::string otherHalf = val[1].substr(val[1].length() / 2);
                
                if (isNumber(half) && isNumber(otherHalf)) 
                {
                    unsigned int result = std::stoul(half, nullptr, 16);
                    unsigned int result2 = std::stoul(otherHalf, nullptr, 16);
                    
                    if (commands.contains(command) && (start_pos == to_write.size() || to_write.empty()))
                    {
                        to_write.push_back(commands[command]);
                        to_write.push_back(static_cast<char>(result));
                        to_write.push_back(static_cast<char>(result2));
                        start_pos += 3;
                    }
                    else if(commands.contains(command))
                    {
                        to_write[start_pos++] = commands[command];
                        to_write[start_pos++] = static_cast<char>(result);
                        to_write[start_pos++] = static_cast<char>(result2);
                    }
                } 
            }
        }
        else
        {
            std::string command = std::format("{} {}", valores[index - 1], valores[index]);
            if (commands.contains(command) && (start_pos == to_write.size() || to_write.empty()))
            {
                to_write.push_back(commands[command]);
            }
            else if(commands.contains(command))
            {
                to_write[start_pos] = commands[command];
                start_pos++;
            } 
        }
    }
    else if (buf.empty() != true && commands.contains(buf))
    {
        to_write.push_back(commands[buf]);
    }
    if (mode == 0)
        writeAddress = start_pos;
    else
        currentAddress = start_pos;
}

void processLabel(const std::string& label, std::vector<char> &to_write, std::vector<std::string> valores) 
{
    symbolTable.insert({label, currentAddress});
    if (to_write.size() < currentAddress)
    {
        int index = 1;
        to_write.resize(currentAddress);
        assemble(to_write, valores, index, "", 1);
        std::cout << to_write.size() << " " << index << currentAddress << std::endl;  
    }
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
        int index = 0;
        std::vector<std::string> valores = tokenize(buf);

        if (valores.size() < 2 && valores.size() != 0)
        {
            std::cout << "\x1B[91mError: no suficientes argumentos\033[0m\t\t" << std::endl;
            return -1;
        }
        if (valores.size() == 0)
            continue;
        if (valores[index].back() == ':')
        {
            processLabel(valores[index], to_write, valores);
            index++;
            continue;
        }
        assemble(to_write, valores, 0, buf);
    }
    std::ofstream outputFile("out.bin", std::ios::binary);
    for (unsigned int i = 0; i < to_write.size(); i++)
    {
        outputFile.write(&to_write[i], sizeof(to_write[i]));
    }
    outputFile.close();

    return 0;
}