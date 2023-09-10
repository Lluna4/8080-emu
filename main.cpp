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

    byte fetch(u32 *cycles, MEM& memory)
    {
        byte data = memory[PC];
        PC++;
        *cycles--;
        return data;
    }

    static constexpr byte
        INS_LXI = 0x01;

    void execute(MEM& memory, u32 cycles)
    {
        while (cycles > 0)
        {
            byte instruction = fetch(&cycles, memory);
            cycles--;

            switch (instruction)
            {
                case INS_LXI:
                    byte B = fetch(&cycles, memory);
                    cycles--;
                    byte u16L = fetch(&cycles, memory);
                    cycles--;
                    byte u16H = fetch(&cycles, memory);
                    cycles--;

                    switch (B)
                    {
                        case 0x00:
                            B = u16L;
                            cycles--;
                            C = u16H;
                            cycles--;
                            break;
                        
                        case 0x01:
                            D = u16L;
                            cycles--;
                            E = u16H;
                            cycles--;
                            break;

                        case 0x10:
                            H = u16L;
                            cycles--;
                            L = u16H;
                            cycles--;
                            break;
                        //case 0x11 for sp reg

                        default:
                            break;
                    }
                    break;
            }
            break;
        }
    }
};


int main()
{
    MEM mem;
    CPU cpu;

    cpu.reset();
    mem.init();

    mem.memory[0] = 0x01;
    mem.memory[1] = 0x01;
    mem.memory[2] = 'A';
    mem.memory[3] = 'B';
    cpu.execute(mem, 10);
    
    return 0;
}