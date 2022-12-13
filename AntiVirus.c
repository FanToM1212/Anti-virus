#include <stdio.h>
#include "dirent.h"
#include <string.h>
#include <stdlib.h>

#pragma warning(disable : 4996)
#define MAX 50
#define FIRST_PART 4
#define LAST_PART 5
#define Infected 0
#define SAFE 1

int virusCheck(char* path, char* virusPath);
int fastVirusCheck(char* path, char* virusPath);

int main(int argc, char* argv[]){
    if (argc != 3){
        printf("how to run - 'antiVirus.exe', Dirctory path, virus path\n");
        return 1;
    }

    FILE* log = NULL;
    char path[255] = { 0 }; //path of current file
    char pathOfLog[MAX] = { 0 };  //path of the log
    char whatScan[MAX] = { 0 };  //what kind of scan
    char choose = ' ';
    char names[20][MAX] = { 0 };
    int n = 0;
    strcat(path, argv[1]);
    DIR* directory = 0;
    struct dirent* dir = 0;

    directory = opendir(argv[1]);
    int bool = 0;
    printf("wellcome to my anti-virus\n");
    printf("\nFolder to scan: %s\n", argv[1]);
    printf("Virus signature: %s\n", argv[2]);
    printf("\npress 0 for a normal scan or any other key fo quick scan: ");
    choose = getchar();
    printf("procces may take a few minutes\n");

    if (choose == '0'){
        strcat(pathOfLog, argv[1]);
        strcat(pathOfLog, "/slowScanLog.txt");
        strcpy(whatScan, "Normal scan");
        log = fopen(pathOfLog, "w");
    }
    else{
        strcat(pathOfLog, argv[1]);
        strcat(pathOfLog, "/fastScanLog.txt");
        strcpy(whatScan, "Quick Scan");
        log = fopen(pathOfLog, "w");
    }
    
    fprintf(log, "\nFolder to scan:\n%s\n", argv[1]);
    fprintf(log, "Virus signature:\n%s\n", argv[2]);
    
    if (directory == NULL){
        fputs("Error opening file\nhow to run - file.exe Dirctory path virus path (for fast scan do 1 other for slow)\n", log);
        printf("Error opening file\n");
        printf("how to run - file.exe Dirctory path virus path (for fast scan do 1 other for slow)\n");
        return 1;
    }

    fprintf(log, "\nScanning option:\n%s\n\n", whatScan);
    fputs("results:\n", log);
    printf("\nscanning:\n");

    while ((dir = readdir(directory)) != NULL){
        if (strcmp(dir->directory_name, ".") && strcmp(dir->directory_name, "..")){
            strcpy(names[n], dir->directory_name);
            n++;
        }
    }

    char temp[30];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (strcmp(names[j], names[j + 1]) > 0) {
                strcpy(temp, names[j]);
                strcpy(names[j], names[j + 1]);
                strcpy(names[j + 1], temp);
            }
        }
    }

    // Check each file for virus signature
    for (int i = 0; i < n; i++) {
        strcpy(path, argv[1]);
        strcat(path, "/");
        strcat(path, names[i]);

        if (choose == '0') {
            bool = virusCheck(path, argv[2]);
        }
        else {
            bool = fastVirusCheck(path, argv[2]);
        }

        if (bool == Infected) {
            fprintf(log, "\n%s is INFECTED\n", names[i]);
            printf("\n%s is INFECTED\n", names[i]);
        }
        else {
            fprintf(log, "\n%s is SAFE\n", names[i]);
            printf("\n%s is SAFE\n", names[i]);
        }
    }

    fclose(log);
    closedir(directory);
    return 0;
}

int fastVirusCheck(char* path, char* virusPath) {
    FILE* virusFile = fopen(virusPath, "rb");
    FILE* scanFile = fopen(path, "rb");

    if (virusFile == NULL || scanFile == NULL){
        printf("Error: Failed to open file\n");
        return 2;
    }

    fseek(scanFile, 0, SEEK_END);
    long int size = ftell(scanFile);
    fseek(scanFile, 0, SEEK_SET);
    fseek(virusFile, 0, SEEK_END);
    long int virusSize = ftell(virusFile);
    fseek(virusFile, 0, SEEK_SET);
    
    if (size < virusSize){
        printf("The file to be scanned is smaller than the virus signature file\n");
        return 1;
    }

    // Perform a virus check in the first 20% of the file to be scanned
    long int i = 0;
    long int j = 0;
    char ch = ' ';
    char ch1 = ' ';
    while (i < size / 5){
        ch = (char)fgetc(scanFile);
        ch1 = (char)fgetc(virusFile);
        if (ch == ch1){
            j++;
            if (j >= virusSize){
                printf("The virus was found in the first 20%% of the file\n");
                return FIRST_PART;
            }
        }
        else{
            fseek(virusFile, 0, SEEK_SET);
            j = 0;
        }
        i++;
    }
    fclose(virusFile);
    fclose(scanFile);
    return 1;
}

int virusCheck(char* path, char* virusPath){
    FILE* virusFile = fopen(virusPath, "rb");
    FILE* scanFile = fopen(path, "rb");

    long int i = 0;  
    long int j = 0;  
    long int size = 0;  
    long int virusSize = 0;  
    char ch = ' '; 
    char ch1 = ' '; 

    if (virusFile == NULL || scanFile == NULL){
        printf("Error opening file");
        return 2;
    }

    fseek(scanFile, 0, SEEK_END);
    size = ftell(scanFile);
    fseek(scanFile, 0, SEEK_SET);
    fseek(virusFile, 0, SEEK_END);
    virusSize = ftell(virusFile);
    fseek(virusFile, 0, SEEK_SET);

    while (i < size){
        ch = (char)fgetc(scanFile);
        ch1 = (char)fgetc(virusFile);

        if (ch == ch1){
            j++;
            if (j >= virusSize){
                return 0;
            }
        }
        else{
            fseek(virusFile, 0, SEEK_SET);
            j = 0;
        }
        i++;
    }
    fclose(virusFile);
    fclose(scanFile);
    return 1;
}
