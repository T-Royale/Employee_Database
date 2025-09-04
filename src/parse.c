#include "common.h"

int delete_employee(short *count, struct employee_t *employees, int delete_id, int fd){
    if(delete_id >= *count || delete_id < 0) {
        return STATUS_ERROR;
    }

    long total_size = (long)sizeof(struct dbheader_t) +
                   (long)*count * (long)sizeof(struct employee_t);

    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) { perror("lseek"); return STATUS_ERROR; }
    if (ftruncate(fd, total_size) == -1) { perror("ftruncate"); return STATUS_ERROR; }

    int tail = *count - delete_id - 1;

    if(tail > 0){
        memmove(&employees[delete_id], &employees[delete_id+1], (size_t)tail * sizeof(employees[0]));
    }

    (*count)--;

    return STATUS_SUCCESS;
}

int list_employees(struct dbheader_t *dbhdr, struct employee_t* employees) {
	if(!dbhdr || !employees) return STATUS_ERROR;
    for(int i = 0; i < dbhdr->count; i++){
		printf("Employee %d\n", i);
		printf("\tName: %s\n", employees[i].name);
		printf("\tAddress: %s\n", employees[i].address);
		printf("\tHours: %d\n", employees[i].hours);
	}
    return STATUS_SUCCESS;
}

int add_employee(struct dbheader_t *dbhdr, struct employee_t **employees, char *addstring) {
    if(!dbhdr || !employees || !addstring)
        return STATUS_ERROR;

    int count = dbhdr->count + 1;

    char name[256] = {0}, addr[256] = {0};
    int hours = 0;
    if (sscanf(addstring, "%255[^,],%255[^,],%d", name, addr, &hours) != 3)
        return STATUS_ERROR;

    struct employee_t *tmp = (count > 1) 
        ? realloc(*employees, count * sizeof(struct employee_t))
        : calloc(1, sizeof(struct employee_t));
    if(!tmp) return STATUS_ERROR;
    *employees = tmp;
    
    memset(&(*employees)[dbhdr->count], 0, sizeof(**employees));

    strncpy((*employees)[dbhdr->count].name, name, sizeof((*employees)[dbhdr->count].name) - 1);
    (*employees)[dbhdr->count].name[sizeof((*employees)[dbhdr->count].name) - 1] = '\0';

    strncpy((*employees)[dbhdr->count].address, addr, sizeof((*employees)[dbhdr->count].address) - 1);
    (*employees)[dbhdr->count].address[sizeof((*employees)[dbhdr->count].address) - 1] = '\0';

    (*employees)[dbhdr->count].hours = hours;

    dbhdr->count = count;
    return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {
    if(fd < 0){
        printf("bad fd on read_employees\n");
        return STATUS_ERROR;
    }
    int count = dbhdr->count;

    struct employee_t* employees = calloc(count, sizeof(struct employee_t));
    if(!employees){
        printf("calloc failed\n");
        return STATUS_ERROR;
    }

    read(fd, employees, count * sizeof(struct employee_t));

    for(int i = 0; i < count; i++){
        employees[i].hours = ntohl(employees[i].hours);
    }

    *employeesOut = employees;
    return STATUS_SUCCESS;
}

int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {
    if(fd < 0){
        printf("bad fd on output_file\n");
        return STATUS_ERROR;
    }

    int realCount = dbhdr->count;

    dbhdr->magic = htonl(dbhdr->magic);
    dbhdr->filesize = htonl(sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realCount));
    dbhdr->count = htons(dbhdr->count);
    dbhdr->version = htons(dbhdr->version);

    lseek(fd, 0, SEEK_SET);
    write(fd, dbhdr, sizeof(struct dbheader_t));

    for(int i = 0; i < realCount; i++){
        employees[i].hours = htonl(employees[i].hours);
        write(fd, &employees[i], sizeof(struct employee_t));
    }

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

int create_db_header(struct dbheader_t **headerOut) {
    if(headerOut == NULL){
        printf("Tried to create db of a NULL pointer");
        return STATUS_ERROR;
    }
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


