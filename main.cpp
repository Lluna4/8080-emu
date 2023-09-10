#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using byte = unsigned char;
using word = unsigned short;

using u32 = unsigned int;

struct MEM
{
    static constexpr u32 MAX_MEM = 1024 * 64;
    byte memory[MAX_MEM];

    void init()
    {
        for (u32 i = 0; i < MAX_MEM; i++)
        {
            memory[i] = 0;
        }
    }

    byte operator[](u32 address) const
    {
        return memory[address];
    }
};

struct CPU
{
    //Program counter and stack pointer
    word PC;
    word SP;

    //registers
    byte A, B, C, D, E, H, L;

    //Flag
    byte F; 

    void reset()
    {
        PC = 0;
        //SP = 0xFFFF;
    }

    byte fetch(MEM& memory)
    {
        byte data = memory[PC];
        PC++;
        return data;
    }

    static constexpr byte
        INS_LXI_B = 0x01,
        INS_STAX_B = 0x02,
        INS_LXI_D = 0x11,
        INS_LXI_H = 0x21,
        INS_LXI_SP = 0x31;

    void execute(MEM& memory, u32 cycles)
    {
        while (cycles > 0)
        {
            byte instruction = fetch(memory);
            byte u16L;
            byte u16H;

            switch (instruction)
            {
                case INS_LXI_B:
                    u16L = fetch(memory);
                    u16H = fetch(memory);
                    B = u16L;
                    C = u16H;
                    cycles -= 10;
                    break;
                
                case INS_STAX_B:
                    u16L = C;
                    u16H = B;
                    memory.memory[(u16H << 8) | u16L] = A;
                    cycles -= 7;
                    break;

                case INS_LXI_D:
                    u16L = fetch(memory);
                    u16H = fetch(memory);
                    D = u16L;
                    E = u16H;
                    cycles -= 10;
                    break;
                
                case INS_LXI_H:
                    u16L = fetch(memory);
                    u16H = fetch(memory);
                    H = u16L;
                    L = u16H;
                    cycles -= 10;
                    break;
                case INS_LXI_SP:
                    u16L = fetch(memory);
                    u16H = fetch(memory);
                    SP = (u16H << 8) | u16L; 
                    cycles -= 10;
                    break;
                
            }
        }
    }
};


int main()
{
    MEM mem;
    CPU cpu;

    cpu.reset();
    mem.init();
    cpu.A = 'A';
    cpu.B = 0x1;
    cpu.C = 0x11;


    mem.memory[0] = 0x02;
    cpu.execute(mem, 7);

    std::cout << "Registry breakdown: " << std::endl;
    std::cout << "Accumulator: " << cpu.A << std::endl;
    std::cout << "B: " << cpu.B << std::endl;
    std::cout << "C: " << cpu.C << std::endl;
    std::cout << "D: " << cpu.D << std::endl;
    std::cout << "E: " << cpu.E << std::endl;
    std::cout << "H: " << cpu.H << std::endl;
    std::cout << "L: " << cpu.L << std::endl;
    std::cout << "PC: " << cpu.PC << std::endl;
    std::cout << "SP: " << cpu.SP << std::endl;
    
    return 0;
}