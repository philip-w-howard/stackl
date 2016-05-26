#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define BLOCK_SIZE 1024

int main(int argc, char **argv)
{
    int size;
    int status;
    int disk;
    int ii;

    if (argc < 2)
    {
        printf("makedisk <size in blocks>\n");
        exit(-1);
    }

    size = atoi(argv[1]);
    if (size < 1)
    {
        printf("makedisk <size in blocks>\n");
        exit(-1);
    }

    disk  = open("stackl.disk", O_WRONLY | O_CREAT | O_TRUNC, 
            S_IRUSR | S_IWUSR);

    if (disk < 0)
    {
        printf("Unable to open disk\n");
        exit(-2);
    }

    void *buff = calloc(BLOCK_SIZE, 1);
    if (buff == NULL)
    {
        printf("Buffer allocation failed\n");
        exit(-3);
    }

    for (ii=0; ii<size; ii++)
    {
        status = write(disk, buff, BLOCK_SIZE);
        if (status != BLOCK_SIZE)
        {
            printf("File write failed\n");
            exit(-5);
        }
    }

    close(disk);

    return 0;
}
