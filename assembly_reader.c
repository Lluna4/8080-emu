#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int addr = 0;
    char *buf;
    int write_file = 0;
    FILE *file;

    if (argc > 1)
    {
        if (strcmp(argv[1], "-w") == 0)
            write_file = 1;
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
        printf("%02hhx ", buf[index]);
        index++;
        if (write_file != 0)
            fprintf(file, "%02hhx ", buf[index]);
    }  
}