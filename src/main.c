#include "common.h"

void print_usage(char *argv[]) {
    printf("Usage: %s -n -f <path to db>\n", argv[0]);
    printf("\t -n - create new database file\n");
    printf("\t -f - (required) path to database file\n");
}

int main(int argc, char *argv[]) { 
	bool newFile = false;
    char* filePath = NULL;

    int dbfd = -1;

    char c;
    while((c = getopt(argc, argv, "nf:")) != -1){
        switch(c){
            case 'n':
                newFile = true;
                break;
            case 'f':
                filePath = optarg;
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
    } else{
        dbfd = open_db_file(filePath);
        if(dbfd == STATUS_ERROR){
            printf("Unable to open database file\n");
            return -1;
        }
    }

    return 0;
}
