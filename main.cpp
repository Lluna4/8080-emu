#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <filesystem>
#include "minios.cpp"

int main(int argc, char *argv[])
{
    MEM mem;
    CPU cpu;

    cpu.reset();

    /*if (argc > 1)
    {
        if (std::filesystem::exists(argv[1]) == true)
            load(argv[1], mem);
        else
        {
            std::cout << "\x1B[91mError: el archivo no existe\033[0m\t\t" << std::endl;
            return -1;
        }
    }
    cpu.execute(mem, 0);

    std::cout << "Registry breakdown: " << std::endl;
    std::cout << "Accumulator: " << (int)cpu.A << std::endl;
    std::cout << "B: " << (int)cpu.B << std::endl;
    std::cout << "C: " << (int)cpu.C << std::endl;
    std::cout << "D: " << (int)cpu.D << std::endl;
    std::cout << "E: " << (int)cpu.E << std::endl;
    std::cout << "H: " << (int)cpu.H << std::endl;
    std::cout << "L: " << (int)cpu.L << std::endl;
    std::cout << "PC: " << cpu.PC << std::endl;
    std::cout << "SP: " << cpu.SP << std::endl;
    std::cout << "Flags: " << std::endl;
    std::cout << "Sign: " << (int)cpu.sign << std::endl;
    std::cout << "Zero: " << (int)cpu.zero << std::endl;
    std::cout << "Parity: " << (int)cpu.parity << std::endl;
    std::cout << "Carry: " << (int)cpu.carry << std::endl;
    std::cout << "aux: " << (int)cpu.aux << std::endl;*/
    init_os(cpu, mem);
    std::cout << "Registry breakdown: " << std::endl;
    std::cout << "Accumulator: " << (int)cpu.A << std::endl;
    std::cout << "B: " << (int)cpu.B << std::endl;
    std::cout << "C: " << (int)cpu.C << std::endl;
    std::cout << "D: " << (int)cpu.D << std::endl;
    std::cout << "E: " << (int)cpu.E << std::endl;
    std::cout << "H: " << (int)cpu.H << std::endl;
    std::cout << "L: " << (int)cpu.L << std::endl;
    std::cout << "PC: " << cpu.PC << std::endl;
    std::cout << "SP: " << cpu.SP << std::endl;
    std::cout << "Flags: " << std::endl;
    std::cout << "Sign: " << (int)cpu.sign << std::endl;
    std::cout << "Zero: " << (int)cpu.zero << std::endl;
    std::cout << "Parity: " << (int)cpu.parity << std::endl;
    std::cout << "Carry: " << (int)cpu.carry << std::endl;
    std::cout << "aux: " << (int)cpu.aux << std::endl;
    
    return 0;
}