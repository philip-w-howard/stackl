#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    int size;
    int start_offset;
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

    strcpy(buff, "");
    while ((strcmp(buff, "begindata\n") != 0) && 
           (fgets(buff, size, input) != NULL))
    {}

    start_offset = ftell(input);
    size -= start_offset;

    status = fread(buff, size, 1, input);
    if (status != 1)
    {
        printf("File read failed\n");
        exit(-5);
    }

    lseek(disk, offset, SEEK_SET);
    status = write(disk, buff, size);
    if (status != size)
    {
        printf("File write failed\n");
        exit(-5);
    }

    printf("Write %d bytes starting at offset %d\n", size, offset);
    fclose(input);
    close(disk);

    return 0;
}
