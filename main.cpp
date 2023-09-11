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
        INS_INX_B = 0x03,
        INS_LXI_D = 0x11,
        INS_STAX_D = 0x12,
        INS_INX_D = 0x13,
        INS_LXI_H = 0x21,
        INS_INX_H = 0x23,
        INS_LXI_SP = 0x31,
        INS_INX_SP = 0x33;

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
                
                case INS_INX_B:
                    if (C == 0xFF)
                    {
                        C = 0x00;
                        if (B == 0xFF)
                            B = 0x00;
                        else
                            B++;
                    }
                    else 
                        C++;
                    cycles -= 5;
                    break;
                
                case INS_INX_D:
                    if (E == 0xFF)
                    {
                        E = 0x00;
                        if (D == 0xFF)
                            D = 0x00;
                        else
                            D++;
                    }
                    else 
                        E++;
                    cycles -= 5;
                    break;

                case INS_INX_H:
                    if (L == 0xFF)
                    {
                        L = 0x00;
                        if (H == 0xFF)
                            H = 0x00;
                        else
                            H++;
                    }
                    else
                        L++;
                    cycles -= 5;
                    break;

                case INS_INX_SP:
                    if (SP == 0xFFFF)
                        SP = 0x0000;
                    else
                        SP++;
                    cycles -= 5;
                    break;

                case INS_STAX_D:
                    u16L = E;
                    u16H = D;
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
    cpu.B = 0xFE;
    cpu.C = 0x01;


    mem.memory[0] = 0x01;
    mem.memory[1] = 0xEF;
    mem.memory[2] = 0x14;// LXI B EF20H
    mem.memory[3] = 0x11;
    mem.memory[4] = 0xFF;
    mem.memory[5] = 0xAA;// LXI D FFAAH 
    mem.memory[6] = 0x21;
    mem.memory[7] = 0xBC;
    mem.memory[8] = 0xFF;// LXI H BCFFH
    mem.memory[9] = 0x31;
    mem.memory[10] = 0xFF;
    mem.memory[11] = 0xFF;// LXI SP 31CCH
    mem.memory[12] = 0x03;// INX B
    mem.memory[13] = 0x13;//INX D
    mem.memory[14] = 0x23;//INX H
    mem.memory[15] = 0x33;//INX SP
    cpu.execute(mem, 50);

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
    
    return 0;
}