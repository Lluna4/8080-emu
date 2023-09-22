#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int addr = 0;
    char *buf;
    int write_file = 0;
    int silent = 0;
    FILE *file;

    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], "-w") == 0)
                write_file = 1;
            else if (strcmp(argv[i], "-s") == 0)
                silent = 1;
            else if (strcmp(argv[i], "-h") == 0)
            {
                printf("Options:\n\t-w\n\t\tWrite output to a txt file (output.txt)\n\n\t-s\n\t\tSilent mode: There's no output in console\n\n");
                return 0;
            }
        }
    }

    FILE* f = fopen("out.bin", "rb");
    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    printf("%lu\n", file_size);
    buf = (char *)calloc(file_size, sizeof(char));
    rewind(f);
    fread(buf, sizeof(char), file_size, f);

    size_t index = 0;
    if (write_file != 0)
        file = fopen("output.txt", "w");

    while (index < file_size)
    {
        if (silent == 0)
            printf("%02hhx ", buf[index]);
        index++;
        if (write_file != 0)
            fprintf(file, "%02hhx ", buf[index]);
    }  
}