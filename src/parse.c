#include "common.h"

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {

}

int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring) {

}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {

}

int output_file(int fd, struct dbheader_t *dbhdr/*, struct employee_t *employees*/) {
    if(fd < 0){
        printf("bad fd");
        return STATUS_ERROR;
    }

    dbhdr->magic = htonl(dbhdr->magic);
    dbhdr->filesize = htonl(dbhdr->filesize);
    dbhdr->count = htons(dbhdr->count);
    dbhdr->version = htons(dbhdr->version);

    lseek(fd, 0, SEEK_SET);
    write(fd, dbhdr, sizeof(struct dbheader_t));

    return STATUS_SUCCESS;

}	

int validate_db_header(int fd, struct dbheader_t **headerOut) {
    if(fd < 0){
        printf("got a bad fd\n");
        return STATUS_ERROR;
    }

    struct dbheader_t* header = calloc(1, sizeof(struct dbheader_t));
    if(!header){
        printf("Malloc failed to create dbheader\n");
        return STATUS_ERROR;
    }

    if(read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)){
        perror("read");
        free(header);
        return STATUS_ERROR;
    }

    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->magic = ntohl(header->magic);
    header->filesize = ntohl(header->filesize);

    if(header->magic != HEADER_MAGIC){
        printf("Magic number doesn't match\n");
        free(header);
        return STATUS_ERROR;
    }
    if(header->version != CURRENT_VERSION){
        printf("Improper header version\n");
        free(header);
        return STATUS_ERROR;
    }
    struct stat dbstat = {0};
    fstat(fd, &dbstat);
    if(header->filesize != dbstat.st_size){
        printf("Corrupted database\n");
        free(header);
        return STATUS_ERROR;
    }

    *headerOut = header;

    return STATUS_SUCCESS;
}

int create_db_header(int fd, struct dbheader_t **headerOut) {
	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if(!header){
        printf("malloc failed to create db header\n");
        return STATUS_ERROR;
    }
    header->version = CURRENT_VERSION;
    header->count = 0;
    header->magic = HEADER_MAGIC;        
    header->filesize = sizeof(struct dbheader_t);

    *headerOut = header;
    return STATUS_SUCCESS;    
}


