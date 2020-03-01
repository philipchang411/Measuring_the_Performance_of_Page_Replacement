#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

long * RDM(char file[255], int size, long* values, bool debug);
long * LRU(char file[255], int size, long* values);
long * FIFO(char file[255], int size, long* values, bool debug);
long * VMS(char file[255], int size, long* values);

int main(int argc, const char* argv[])
{
    long totalLines;
    long diskReads; 
    long diskWrites;
    long tempArray[3];
    long* values;
    bool debug;
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

    if(strcmp(argv[4], "debug")==0)
        debug=true;
    //Determines which function to be directed to
    strcpy(temp, argv[3]);
    if(strcmp(temp, "fifo") == 0 || strcmp(temp, "FIFO") == 0)
        values = FIFO(temp1, size, tempArray, debug);
    else if(strcmp(temp, "LRU") == 0 || strcmp(temp, "lru") == 0)
        LRU(temp1, size, tempArray);
    else if(strcmp(temp, "RDM") == 0 || strcmp(temp, "rdm") == 0)
        values = RDM(temp1, size, tempArray, debug);
    else if(strcmp(temp, "VMS") == 0 || strcmp(temp, "vms") == 0)
        VMS(temp1, size, tempArray);
    else
    {
        printf("You did not input a valid function.\n");
        exit(0);
    }

    //Outputs
    printf("Total Memory Frames: %d\n", atoi(argv[2]));
    printf("Events in trace: %ld\n", values[2]);
    printf("Total Disk Reads: %ld\n", values[0]);
    printf("Total Disk Write: %ld\n", values[1]);

    return 0;
}

long * RDM(char file[255], int size, long* values, bool debug)
{
   FILE *inputFile = fopen(file, "r");
    char temp[255];
    char address[255];
    char instruction[255];
    long diskReads =0;
    long diskWrites =0;
    long totalLines=0;
    //Creates the Array based on the input
    struct page
    {
        char name[24];
        bool dirtyBit;
    };

    struct page pageTable[size];
    int sizeOfTable = 0;
    int end = -1;

    while (fscanf(inputFile, "%s", address) == 1) // expect 1 successful conversion
    {
        for(int i = 0; i < strlen(address)-3; i++)
        {
            temp[i] = address[i];
        }
        
        fscanf(inputFile, "%s", instruction);
        bool present = false;
        
        if(debug == true)
        {
            printf("The current address: %s\nThe Current direction: %s\n", temp, instruction);

            for(int i = 0; i < size; i++)
            {
                printf("Content in the %d element %s\n", i, pageTable[i].name);
            }
        }
        //If writing to the desk, check to the address is presentin the stack. If not then write to the disk. If it is, write to the frame and make dirty bit
        if(strcmp(instruction, "W"))
        {
            for(int i = 0; i < size; i++)
            {
                if(strcmp(pageTable[i].name, temp) == 0)
                {
                    pageTable[i].dirtyBit = true;
                    present = true;
                }
            }
            //The data is not in the frames so you must insert
            if(present == false)
            {
                //The frames aren't full so you don't need to take out
                if(sizeOfTable != size)
                {
                    //INSERT
                    strcpy(pageTable[sizeOfTable-1].name,temp);
                    sizeOfTable++;
                }
                else
                {
                    //In order to move each data over, strcpy everything over one and insert in the beginnging
                    int randomElement = rand() %size;
                    struct page transfer = pageTable[randomElement];
                    strcpy(pageTable[randomElement].name,temp);
                    if(pageTable[randomElement].dirtyBit)
                        diskWrites++;
                }
                
            }
        }
        //If the instruction is 'R'
        else
        {
            for(int i = 0; i < size; i++)
            {
                if(strcmp(pageTable[i].name, temp) == 0)
                    present = true;
            }
            //If the data isn't there, add it to the frame
            if(present == false)
            {
                diskReads++;
                if(sizeOfTable != size)
                {
                    //INSERT for QUEUE Replaces the first input if it is full
                    strcpy(pageTable[sizeOfTable-1].name,temp);
                    sizeOfTable++;
                }
                else
                {
                    //In order to move each data over, strcpy everything over one and insert in the beginnging
                    int randomElement = rand() %size;
                    struct page transfer = pageTable[randomElement];
                    strcpy(pageTable[randomElement].name,temp);
                }
            }
        }
        totalLines++;
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
    values[0] = diskReads;
    values[1] = diskWrites;
    values[2] = totalLines;
    return values;
}

long * LRU(char file[255], int size, long* values)
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
    return values;
}

//Needs to use a queue in order to keep track of whick to replace
long * FIFO(char file[255], int size, long* values, bool debug)
{
    FILE *inputFile = fopen(file, "r");
    char temp[255];
    char address[255];
    char instruction[255];
    long diskReads =0;
    long diskWrites =0;
    long totalLines=0;
    //Creates the Array based on the input
    struct page
    {
        char name[24];
        bool dirtyBit;
    };

    struct page pageTable[size];
    int sizeOfTable = 0;
    int end = -1;

    while (fscanf(inputFile, "%s", address) == 1) // expect 1 successful conversion
    {
        for(int i = 0; i < strlen(address)-3; i++)
        {
            temp[i] = address[i];
        }
        
        fscanf(inputFile, "%s", instruction);
        bool present = false;
        
        if(debug == true)
        {
            printf("The current address: %s\nThe Current direction: %s\n", temp, instruction);

            for(int i = 0; i < size; i++)
            {
                printf("Content in the %d element %s\n", i, pageTable[i].name);
            }
        }
        //If writing to the desk, check to the address is presentin the stack. If not then write to the disk. If it is, write to the frame and make dirty bit
        if(strcmp(instruction, "W"))
        {
            for(int i = 0; i < size; i++)
            {
                if(strcmp(pageTable[i].name, temp) == 0)
                {
                    pageTable[i].dirtyBit = true;
                    present = true;
                }
            }
            //The data is not in the frames so you must insert
            if(present == false)
            {
                //The frames aren't full so you don't need to take out
                if(sizeOfTable != size)
                {
                    //INSERT for QUEUE Replaces the first input if it is full
                    if(end == (size-1))
                        end=-1;
                    strcpy(pageTable[++end].name,temp);
                    sizeOfTable++;
                }
                else
                {
                    //In order to move each data over, strcpy everything over one and insert in the beginnging
                    for(int i = 0; i > size-1; i++)
                    {
                        strcpy(pageTable[i].name, pageTable[i+1].name);
                        pageTable[i].dirtyBit = pageTable[i+1].dirtyBit;
                    }
                    if(pageTable[size-1].dirtyBit)
                        diskWrites++;
                    strcpy(pageTable[size-1].name,temp);
                    pageTable[size-1].dirtyBit = true;
                }
                
            }
        }
        //If the instruction is 'R'
        else
        {
            for(int i = 0; i < size; i++)
            {
                if(strcmp(pageTable[i].name, temp) == 0)
                    present = true;
            }
            //If the data isn't there, add it to the frame
            if(present == false)
            {
                diskReads++;
                if(sizeOfTable != size)
                {
                    //INSERT for QUEUE Replaces the first input if it is full
                    if(end == (size-1))
                        end=-1;
                    strcpy(pageTable[++end].name,temp);
                    sizeOfTable++;
                }
                else
                {
                    //In order to move each data over, strcpy everything over one and insert in the beginnging
                    for(int i = 0; i > size-1; i++)
                    {
                        strcpy(pageTable[i].name, pageTable[i+1].name);
                        pageTable[i].dirtyBit = pageTable[i+1].dirtyBit;
                    }
                    strcpy(pageTable[size-1].name,temp);
                }
            }
        }
        totalLines++;
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
    values[0] = diskReads;
    values[1] = diskWrites;
    values[2] = totalLines;
    return values;
}

long * VMS(char file[255], int size, long* values)
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
    return values;
}