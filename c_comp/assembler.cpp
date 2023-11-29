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
#include "tokenize.hpp"

std::map<std::string, char> commands = {{"LXI", 0x01}, {"LXI B", 0x01}, {"STAX B", 0x02}, {"INX B", 0x03}, {"INR", 0x04}, {"INR B", 0x04}, {"LXI D", 0x11}, {"STAX D", 0x12}, {"INX D", 0x13},
{"INR D", 0x14} ,{"LXI H", 0x21}, {"INX H", 0x23}, {"INR H", 0x24},{"LXI SP", 0x31}, {"INX SP", 0x33}, {"INR M", 0x34}};

std::map<std::string, uint16_t> symbolTable;
uint16_t currentAddress = 0x1000;
uint16_t writeAddress = 0x0000;
bool label = false;

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
                start_pos++;
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
        if (commands.contains(buf) && (start_pos == to_write.size() || to_write.empty()))
        {
            to_write.push_back(commands[buf]);
            start_pos++;
        }
        else if(commands.contains(buf))
        {
            to_write[start_pos++] = commands[buf];
        }
    }
    if (mode == 0)
        writeAddress = start_pos;
    else
        currentAddress = start_pos;
}

void processLabel(const std::string& label, std::vector<char> &to_write, std::vector<std::string> valores, std::string buf) 
{
    symbolTable.insert({label, currentAddress});
    if (to_write.size() < currentAddress)
    {
        int index = 1;
        to_write.resize(currentAddress);
        assemble(to_write, valores, index, tokenize(buf, ':')[1], 1);
        std::cout << to_write.size() << " " << index << currentAddress << std::endl;  
    }
}

int start_assemble(int argc, char *argv[])
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
            processLabel(valores[index], to_write, valores, buf);
            index++;
            label = true;
            continue;
        }
        if ((buf[0] == ' ' || buf[0] == '\t') && label == true)
        {
            size_t in = buf.find_first_not_of(" \t\n\r\f\v");
            if (in != std::string::npos)
                buf = buf.substr(in);
            assemble(to_write, valores, index, buf, 1);
            continue;
        }
        assemble(to_write, valores, 0, buf);
        label = false;
    }
    std::ofstream outputFile("out.bin", std::ios::binary);
    for (unsigned int i = 0; i < to_write.size(); i++)
    {
        outputFile.write(&to_write[i], sizeof(to_write[i]));
    }
    outputFile.close();
    std::cout << "Bytes written: " << currentAddress << std::endl;

    return 0;
}