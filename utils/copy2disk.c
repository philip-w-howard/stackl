#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    int offset;
    int status;
    FILE *input;
    int disk;

    if (argc < 3)
    {
        printf("copy2disk <offset> <file>\n");
        exit(-1);
    }

    offset = atoi(argv[1]);
    if (offset < 0)
    {
        printf("copy2disk <offset> <file>\n");
        exit(-1);
    }

    input = fopen(argv[2], "r");
    if (input == NULL)
    {
        printf("Unable to open %s\n", argv[2]);
        exit(-2);
    }

    disk  = open("stackl.disk", O_RDWR);
    if (disk < 0)
    {
        printf("Unable to open disk\n");
        exit(-2);
    }

    int size;
    size = fseek(input, 0, SEEK_END);
    if (size != 0)
    {
        printf("File IO failure\n");
        exit (-3);
    }

    size = ftell(input);
    void *buff = malloc(size);
    if (buff == NULL)
    {
        printf("Buffer allocation failed\n");
        exit(-3);
    }

    rewind(input);
    lseek(disk, offset, SEEK_SET);

    status = fread(buff, size, 1, input);
    if (status != 1)
    {
        printf("File read failed\n");
        exit(-5);
    }

    status = write(disk, buff, size);
    if (status != size)
    {
        printf("File write failed\n");
        exit(-5);
    }

    fclose(input);
    close(disk);

    return 0;
}
