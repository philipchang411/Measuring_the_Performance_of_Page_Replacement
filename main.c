#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int RDM(char file[255], int size);
int LRU(char file[255], int size);
int FIFO(char file[255], int size);
int VMS(char file[255], int size);

int main(int argc, const char* argv[])
{
    int totalLines;
    int diskReads; 
    int diskWrites;

    //If there isn't any arguments
    if(argc == 1)
    {
        printf("You did not provide any arguments.\n");
        exit(0);
    }

    //Needs five arguments to work
    if(argc != 5)
    {
        printf("You did not provide enough arguments.\n");
        exit(0);
    }

    char temp[255];
    strcpy(temp, argv[2]);
    int size = atoi(temp);

    //Grabs the file name
    char temp1[255];
    strcpy(temp1, argv[1]);

    //Determines which function to be directed to
    strcpy(temp, argv[3]);
    printf("Process: %s", temp);
    if(strcmp(temp, "fifo") == 0 || strcmp(temp, "FIFO") == 0)
        FIFO(temp1, size);
    else if(strcmp(temp, "LRU") == 0 || strcmp(temp, "lru") == 0)
        LRU(temp1, size);
    else if(strcmp(temp, "RDM") == 0 || strcmp(temp, "rdm") == 0)
        RDM(temp1, size);
    else if(strcmp(temp, "VMS") == 0 || strcmp(temp, "vms") == 0)
        VMS(temp1, size);
    else
    {
        printf("You did not input a valid function.\n");
        exit(0);
    }

    //Outputs
    printf("Total Memory Frames: %d\n", atoi(argv[2]));
    printf("Events in trace: %d\n", totalLines);
    printf("Total Disk Reads: %d\n", diskReads);
    printf("Total Disk Write: %d\n", diskWrites);

    return 0;
}

int RDM(char file[255], int size)
{
    FILE *inputFile = fopen(file, "r");
    char temp[255];
    
    //Creates the Array based on the input
    struct page
    {
        char name[24];
        bool dirtyBit;
    };
    struct page pageTable[size];
    while (fscanf(inputFile, "%s", temp) == 1) // expect 1 successful conversion
    {
        printf("Line: %s\n", temp);
    }
    if (feof(inputFile)) 
    {
        printf("All done\n");
    }
    else
    {
        printf("Ummmm Idk\n");
    }

    fclose(inputFile);
    return 0;
}

int LRU(char file[255], int size)
{
    FILE *inputFile = fopen(file, "r");
    char temp[255];
    
    //Creates the Array based on the input
    struct page
    {
        char name[24];
        bool dirtyBit;
    };
    struct page pageTable[size];
    while (fscanf(inputFile, "%s", temp) == 1) // expect 1 successful conversion
    {
        printf("Line: %s\n", temp);
    }
    if (feof(inputFile)) 
    {
        printf("All done\n");
    }
    else
    {
        printf("Ummmm Idk\n");
    }

    fclose(inputFile);
    return 0;
}

//Needs to use a queue in order to keep track of whick to replace
int FIFO(char file[255], int size)
{
    FILE *inputFile = fopen(file, "r");
    char temp[255];
    char instruction[255];
    
    //Creates the Array based on the input
    struct page
    {
        char name[24];
        bool dirtyBit;
    };
    struct page pageTable[size];

    while (fscanf(inputFile, "%s", temp) == 1) // expect 1 successful conversion
    {
        fscanf(inputFile, "%s", instruction);
        if(strcmp(instruction, "R") == 0)
            printf("Successful");
    }
    if (feof(inputFile)) 
    {
        printf("All done\n");
    }
    else
    {
        printf("Ummmm Idk\n");
    }

    fclose(inputFile);
    return 0;
}

int VMS(char file[255], int size)
{
    FILE *inputFile = fopen(file, "r");
    char temp[255];
    
    //Creates the Array based on the input
    struct page
    {
        char name[24];
        bool dirtyBit;
    };
    struct page pageTable[size];
    while (fscanf(inputFile, "%s", temp) == 1) // expect 1 successful conversion
    {
        printf("Line: %s\n", temp);
    }
    if (feof(inputFile)) 
    {
        printf("All done\n");
    }
    else
    {
        printf("Ummmm Idk\n");
    }

    fclose(inputFile);
    return 0;
}