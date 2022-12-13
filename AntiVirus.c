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

int main(int argc, char* argv[])
{
    // Check if the correct number of command line arguments were provided
    if (argc != 3)
    {
        printf("how to run - 'antiVirus.exe', Dirctory path, virus path\n");
        return 1;
    }

    // Initialize variables
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

    // Prompt user for scan type
    directory = opendir(argv[1]);
    int bool = 0;
    printf("wellcome to my anti-virus ;) hope you like it\n");
    printf("\nFolder to scan: %s\n", argv[1]);
    printf("Virus signature: %s\n", argv[2]);
    printf("\npress 0 for a normal scan or any other key fo quick scan: ");
    choose = getchar();
    printf("procces may take a few minutes\n");

    // Choose scan type and open log file
    if (choose == '0')
    {
        strcat(pathOfLog, argv[1]);
        strcat(pathOfLog, "/slowScanLog.txt");
        strcpy(whatScan, "Normal scan");
        log = fopen(pathOfLog, "w");
    }
    else
    {
        strcat(pathOfLog, argv[1]);
        strcat(pathOfLog, "/fastScanLog.txt");
        strcpy(whatScan, "Quick Scan");
        log = fopen(pathOfLog, "w");
    }

    fputs("wellcome to my anti-virus ;) hope you like it\n", log);
    fprintf(log, "\nFolder to scan:\n%s\n", argv[1]);
    fprintf(log, "Virus signature:\n%s\n", argv[2]);

    // Check if directory was opened successfully
    if (directory == NULL)
    {
        fputs("Error opening file\nhow to run - file.exe Dirctory path virus path (for fast scan do 1 other for slow)\n", log);
        printf("Error opening file\n");
        printf("how to run - file.exe Dirctory path virus path (for fast scan do 1 other for slow)\n");
        return 1;
    }

    // Print scan type to log file
    fprintf(log, "\nScanning option:\n%s\n\n", whatScan);
    fputs("results:\n", log);
    printf("\nscanning:\n");

    // Read files in directory
    while ((dir = readdir(directory)) != NULL)
    {
        if (strcmp(dir->directory_name, ".") && strcmp(dir->directory_name, ".."))
        {
            strcpy(names[n], dir->directory_name);
            n++;
        }
    }

    // Sort file names alphabetically
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

    // Close log file and directory
    fclose(log);
    closedir(directory);

    return 0;
}
int fastVirusCheck(char* path, char* virusPath) {
    // Open the virus signature file and the file to be scanned for viruses
    FILE* virusFile = fopen(virusPath, "rb");
    FILE* scanFile = fopen(path, "rb");

    // Check if either of the files failed to open
    if (virusFile == NULL || scanFile == NULL)
    {
        // Print an error message and return 2 if either of the files failed to open
        printf("Error: Failed to open file\n");
        return 2;
    }

    // Calculate the size of the file to be scanned and the virus signature file
    fseek(scanFile, 0, SEEK_END);
    long int size = ftell(scanFile);
    fseek(scanFile, 0, SEEK_SET);
    fseek(virusFile, 0, SEEK_END);
    long int virusSize = ftell(virusFile);
    fseek(virusFile, 0, SEEK_SET);

    // Return 1 if the size of the file to be scanned is smaller than the size of the virus signature file
    if (size < virusSize)
    {
        printf("The file to be scanned is smaller than the virus signature file\n");
        return 1;
    }

    // Perform a virus check in the first 20% of the file to be scanned
    long int i = 0;
    long int j = 0;
    char ch = ' ';
    char ch1 = ' ';
    while (i < size / 5)
    {
        // Read a character from the file to be scanned and the virus signature file
        ch = (char)fgetc(scanFile);
        ch1 = (char)fgetc(virusFile);

        // If the characters match, increment the counter
        if (ch == ch1)
        {
            j++;

            // If the counter reaches the size of the virus signature file, return FIRST_PART
            if (j >= virusSize)
            {
                printf("The virus was found in the first 20%% of the file\n");
                return FIRST_PART;
            }
        }
        // If the characters do not match, reset the counter and the position in the virus signature file
        else
        {
            fseek(virusFile, 0, SEEK_SET);
            j = 0;
        }
        i++;
    }
    fclose(virusFile);
    fclose(scanFile);

    return 1;
}

int virusCheck(char* path, char* virusPath)
{
    // Open the files for reading in binary mode
    FILE* virusFile = fopen(virusPath, "rb");
    FILE* scanFile = fopen(path, "rb");

    // Initialize variables
    long int i = 0;  // Position in the scanFile
    long int j = 0;  // Number of consecutive matching characters found
    long int size = 0;  // Size of the scanFile
    long int virusSize = 0;  // Size of the virusFile
    char ch = ' ';  // Character from scanFile
    char ch1 = ' ';  // Character from virusFile

    // Check if the files were successfully opened
    if (virusFile == NULL || scanFile == NULL)
    {
        printf("Error opening file");
        return 2;
    }

    // Determine the size of the files
    fseek(scanFile, 0, SEEK_END);
    size = ftell(scanFile);
    fseek(scanFile, 0, SEEK_SET);
    fseek(virusFile, 0, SEEK_END);
    virusSize = ftell(virusFile);
    fseek(virusFile, 0, SEEK_SET);

    // Compare each character of the files
    while (i < size)
    {
        // Read the next character from each file
        ch = (char)fgetc(scanFile);
        ch1 = (char)fgetc(virusFile);

        // Check if the characters match
        if (ch == ch1)
        {
            // If the characters match, increment the counter
            j++;

            // Check if the virus has been found
            if (j >= virusSize)
            {
                return 0;
            }
        }
        else
        {
            // If the characters don't match, reset the counter
            fseek(virusFile, 0, SEEK_SET);
            j = 0;
        }

        // Advance to the next character in the scan file
        i++;
    }

    // Close the files
    fclose(virusFile);
    fclose(scanFile);

    // Return 1 to indicate that the file is not infected
    return 1;
}
