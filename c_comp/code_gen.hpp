#include <iostream>
#include <fstream>
#include "parser.hpp"

void code_gen(std::vector<struct func> parsed)
{
    std::ofstream file;
    char a[1024];

    file.open("output.asm");
    for(int i = 0; i < parsed.size(); i++)
    {
        sprintf(a, "%04hhX", parsed[i].return_thing.return_num);
        file << "LXI " << "B," << a << "H" << std::endl;
    }
    file.close();
}