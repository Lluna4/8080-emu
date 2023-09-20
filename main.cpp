#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <filesystem>

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
    bool sign : 1, zero : 1, parity : 1, carry : 1, aux : 1;

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
        INS_INR_B = 0x04,
        INS_LXI_D = 0x11,
        INS_STAX_D = 0x12,
        INS_INX_D = 0x13,
        INS_INR_D = 0x14,
        INS_LXI_H = 0x21,
        INS_INX_H = 0x23,
        INS_INR_H = 0x24,
        INS_LXI_SP = 0x31,
        INS_INX_SP = 0x33,
        INS_INR_M = 0x34;

    void execute(MEM& memory, u32 cycles)
    {
        bool exe = true;
        while (exe == true)
        {
            byte instruction = fetch(memory);
            int index = 0;
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
                
                case INS_INR_B:
                    if (B == 0xFF)
                    {
                        B = 0x00;
                        carry = 1;
                    }
                    else
                        B++;
                    
                    zero = (B == 0);
                    parity = ((B % 2) == 0);
                
                case INS_INR_D:
                    if (D == 0xFF)
                    {
                        D = 0x00;
                        carry = 1;
                    }
                    else
                        D++;
                    zero = (D == 0);
                    parity = ((D % 2) == 0);

                case INS_INR_H:
                    if (H == 0xFF)
                    {
                        H = 0x00;
                        carry = 1;
                    }
                    else
                        H++;
                    zero = (H == 0);
                    parity = ((H % 2) == 0);
                
                case INS_INR_M:
                    while (true)
                    {
                        if (memory[index] == 0xFF)
                        {
                            index++;
                            memory.memory[index] = 0x00;
                            carry = 1;
                        }
                        else
                        {
                            memory.memory[index]++;
                            break;
                        } 
                    }
                    zero = (memory[index] == 0);
                    parity = ((memory[index] % 2) == 0);

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
                
                case 0:
                    exe = false;
                    break;

                default:
                    break;
            }
        }
    }
};

void load(char *argv, MEM& memory)
{
    int addr = 0;
    FILE* f = fopen(argv, "rb");
    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    rewind(f);
    fread(&memory.memory[addr], sizeof(uint8_t), file_size, f);
}

int main(int argc, char *argv[])
{
    MEM mem;
    CPU cpu;

    cpu.reset();

    if (argc > 1)
    {
        if (std::filesystem::exists(argv[1]) == true)
            load(argv[1], mem);
        else
        {
            std::cout << "\x1B[91mError: el archivo no existe\033[0m\t\t" << std::endl;
            return -1;
        }
    }

    cpu.A = 0x03;

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
    
    return 0;
}