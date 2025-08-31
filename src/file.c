#include "common.h"

int create_db_file(char *filename) {
    int fd;
    if((fd = open(filename, O_RDONLY)) != -1){
        close(fd);
        printf("The file %s already exists\n", filename);
        return STATUS_ERROR;
    }
    fd = open(filename, O_RDWR | O_CREAT, 0644);
    if(fd == -1) {
        perror("open");
        return STATUS_ERROR;
    }
    return fd;
}

int open_db_file(char *filename) {
    int fd;
    if((fd = open(filename, O_RDWR)) == -1){
        close(fd);
        perror("open");
        return STATUS_ERROR;
    }
}


