#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define MAX_FILENAME_LENGTH 256

int main(int argc, char *argv[])
{
    int addr = 0;
    char *buf;
    int write_file = 0;
    int silent = 0;
    char output_name[MAX_FILENAME_LENGTH] = "output.txt";
    FILE *file;

    if (argc > 1)
    {
        if (access(argv[1], F_OK) != 0)
        {
            printf("\x1B[91mError: The file doesn't exist\033[0m\t\t\n");
            return -1;
        }
        for (int i = 2; i < argc; i++)
        {
            if (strcmp(argv[i], "-h") == 0)
            {
                printf("Options:\n\t-w\n\t\tWrite output to a txt file (output.txt)\n\n\t-s\n\t\tSilent mode: There's no output in console\n\n");
                return 0;
            }
            if (strcmp(argv[i], "-w") == 0)
                write_file = 1;
            else if (strcmp(argv[i], "-s") == 0)
                silent = 1;
            else if (strcmp(argv[i], "-w") == 0)
                write_file = 1;
            else if (strcmp(argv[i], "-o") == 0)
            {
                if (argc < i + 2)
                {
                    printf("\x1B[91mError: Not enough arguments for output\033[0m\t\t\n");
                    return -1;
                }
                if (access(argv[i+1], F_OK) != 0)
                {
                    strncpy(output_name, argv[i+1], strlen(argv[i+1]) + 1);
                    output_name[MAX_FILENAME_LENGTH - 1] = '\0';
                    printf("(%s), (%s), (%i)", argv[i + 1], output_name, i);
                }
                else
                {
                    printf("\x1B[91mError: File already exists\033[0m\t\t\n");
                    return -1;
                }
            }
        }
    }
    else
    {
        printf("\x1B[91mError: Not enough arguments\033[0m\t\t\n");
        return -1;
    }

    FILE* f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    printf("%lu\n", file_size);
    buf = (char *)calloc(file_size, sizeof(char));
    rewind(f);
    fread(buf, sizeof(char), file_size, f);

    size_t index = 0;
    if (write_file != 0)
        file = fopen(output_name, "w");

    while (index < file_size)
    {
        if (silent == 0)
            if (index % 0x0010 == 0)
                printf("\n0x%04lx %02hhx ", index, buf[index]);
            else
                printf("%02hhx ", buf[index]);
        index++;
        if (write_file != 0)
            if (index % 0x0010 == 0)
                fprintf(file,"\n0x%04lx %02hhx ", index, buf[index]);
            else
                fprintf(file,"%02hhx ", buf[index]);
    }  
}