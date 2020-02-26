#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int RDM();
int LRU();
int FIFO();
int VMS();

int main(int argc, const char* argv[])
{
    int totalLines;
    int diskReads; 
    int diskWrites;

    if(argc == 1)
    {
        printf("You did not provide any arguments.\n");
        exit(0);
    }

    if(argc != 5)
    {
        printf("You did not provide enough arguments.\n");
        exit(0);
    }
    
    //Grabs the file name and opens the file
    char temp[255];
    strcpy(temp, argv[1]);
    FILE *inputFile = fopen(temp, "r");

    //Creates the Array based on the input
    struct page
    {
        char name[24];
        bool dirtyBit;
    };
    strcpy(temp, argv[2]);
    int size = atoi(temp);
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

    //Determines which function to be directed to
    strcpy(temp, argv[3]);
    printf("Process: %s", temp);
    if(strcmp(temp, "fifo") == 0 || strcmp(temp, "FIFO") == 0)
        FIFO();
    else if(strcmp(temp, "LRU") == 0 || strcmp(temp, "lru") == 0)
        LRU();
    else if(strcmp(temp, "RDM") == 0 || strcmp(temp, "rdm") == 0)
        RDM();
    else if(strcmp(temp, "VMS") == 0 || strcmp(temp, "vms") == 0)
        VMS();
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

    fclose(inputFile);
    return 0;
}

int RDM()
{
    return 0;
}

int LRU()
{
    return 0;
}

int FIFO()
{
    return 0;
}

int VMS()
{
    return 0;
}