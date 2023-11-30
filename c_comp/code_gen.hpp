#include <iostream>
#include <fstream>
#include "parser.hpp"

void code_gen(std::vector<struct func> parsed)
{
    std::ofstream file;
    char reg = 'D';
    char a[1024];

    file.open("output.asm");
    for(int i = 0; i < parsed.size(); i++)
    {
        for (int j = 0; j < parsed[0].variables.size(); j++)
        {
            sprintf(a, "%04X", atoi(parsed[i].variables[j].value.c_str()));
            file << "LXI " << reg << ","<< a << "H" << std::endl;
            reg += 2;
            if (reg == 'F')
                reg += 2;
        }
        sprintf(a, "%04hhX", parsed[i].return_thing.return_num);
        file << "LXI " << "B," << a << "H" << std::endl;
    }
    file.close();
}