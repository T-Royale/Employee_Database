#include "common.h"

void print_usage(char *argv[]) {
    printf("Usage: %s -n -f <path to db>\n", argv[0]);
    printf("\t -n - create new database file\n");
    printf("\t -a \"name,addr,hours\" - add employee\n");
    printf("\t -f - (required) path to database file\n");
}

int main(int argc, char *argv[]) { 
	bool newFile = false;
    char* filePath = NULL;
    char* addString = NULL;

    int dbfd = -1;
    struct dbheader_t* header = NULL;

    struct employee_t* employees = NULL;

    char c;
    while((c = getopt(argc, argv, "nf:a:")) != -1){
        switch(c){
            case 'n':
                newFile = true;
                break;
            case 'f':
                filePath = optarg;
                break;
            case 'a':
                addString = optarg;
                break;
            case '?':
                printf("unknown option -%c\n", c);
        }
    }

    if(filePath == NULL){
        printf("Database file path not specified\n");
        print_usage(argv);
        return 0;
    }

    if(newFile){
        dbfd = create_db_file(filePath);
        if(dbfd == STATUS_ERROR){
            printf("Unable to create database file\n");
            return -1;
        }
        if(create_db_header(&header) == STATUS_ERROR){
            printf("Failed to create database header\n");
            return -1;
        }
    } else{
        dbfd = open_db_file(filePath);
        if(dbfd == STATUS_ERROR){
            printf("Unable to open database file\n");
            return -1;
        }
        if(validate_db_header(dbfd, &header) == STATUS_ERROR){
            printf("failed to create header\n");
            return -1;
        }
    }

    if(read_employees(dbfd, header, &employees) != STATUS_SUCCESS){
        printf("failed to read employees\n");
        return 0;
    }

    if(addString){
        if(add_employee(header, &employees, addString) == STATUS_ERROR){
            printf("Failed to add employee\n");
            return -1;
        }
    }

    output_file(dbfd, header, employees);

    return 0;
}