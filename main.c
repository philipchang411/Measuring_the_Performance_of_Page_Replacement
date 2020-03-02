#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//global variables so they don't need to be be passed to another function
int reads, writes, traces, debugger;
//instructions
char rw;
unsigned addr, process, nf;
const char *fileName;
struct queue *memory, *queue_A, *queue_B, *clean, *dirty;

//structure for every individual frame
struct page
{
    unsigned page_frame;
    int dirty;
};

//structure for the entire virtual memory
struct queue
{
    struct page *page_table;
    int size;
    int index;
};

//Helper function that will insert tsecond parameter by copying it 
void insert(struct queue *q, struct page *pageChecked)
{
    q->page_table[q->index].page_frame = pageChecked->page_frame;
    q->page_table[q->index].dirty = pageChecked->dirty;
    q->index = q->index + 1;
};

//Help function for deleting an element and moving everything accordingly. Need to move the surrounding elements
void delete (struct queue *q)
{
    int i;
    for (i = 0; i < q->index - 1; i++)
    {
        q->page_table[i].page_frame = q->page_table[i + 1].page_frame;
        q->page_table[i].dirty = q->page_table[i + 1].dirty;
    }

    q->page_table[q->index - 1].page_frame = 0;
    q->page_table[q->index - 1].dirty = 0;
    if (q->index != 0)
        q->index = q->index - 1;
};

//Searches the current virtual addresses until it locates the desired address
int findIndex(struct queue *q, struct page *pageChecked)
{
    int i;
    for (i = 0; i < q->index; i++)
    {
        if (q->page_table[i].page_frame == pageChecked->page_frame)
            return i;
    }

    return -1;
};

//Deletes but doesn't delete the actual element. Instead. It just moves all the information. Makes room without deleting. 
void shiftRight(struct queue *q, int start)
{
    int i;
    for (i = start; i < q->index - 1; i++)
    {
        q->page_table[i].page_frame = q->page_table[i + 1].page_frame;
        q->page_table[i].dirty = q->page_table[i + 1].dirty;
    }

    q->page_table[q->index - 1].page_frame = 0;
    q->page_table[q->index - 1].dirty = 0;
}

//Algoritm to remove the least recently used 
void LRU(FILE **f, int frame, int debugger)
{
    memory = (struct queue *)malloc(sizeof(struct queue));
    memory->page_table = (struct page *)malloc(sizeof(struct page) * frame);
    memory->size = frame;
    memory->index = 0;

    reads = writes = traces;

    while (fscanf(*f, "%x %c", &addr, &rw) != EOF)
    {
        struct page *pageTraced;
        pageTraced = (struct page *)malloc(sizeof(struct page));
        pageTraced->page_frame = addr >> 12;
        pageTraced->dirty = 0;
        traces++;

        if (debugger == 0)
        {
            printf("Address requested: %x \n", addr);
            printf("Page requested: %x \n", pageTraced->page_frame);
        }

        //If the memory is present, do the following
        if (findIndex(memory, pageTraced) != -1)
        {
            if (rw == 'W')
                pageTraced->dirty = 1;
            //Move everything over to account of the most recently used and move the most recently used to the "front"
            shiftRight(memory, findIndex(memory, pageTraced));
            memory->index = memory->index - 1;
            insert(memory, pageTraced);
        }

        //Address is not already present
        else
        {
            //If the VM is full
            if (memory->index == memory->size)
            {
                if (memory->page_table[0].dirty == 1)
                    writes++;
                //move the frames over and make room for the new one
                shiftRight(memory, 0);
                memory->index = memory->index - 1;
            }
            reads++;
            if (rw == 'W')
                pageTraced->dirty = 1;
            insert(memory, pageTraced);
        }

        if (debugger == 0)
        {
            printf("frames: %d \n", frame);
            printf("traces: %d \n", traces);
            printf("reads: %d \n", reads);
            printf("writes: %d \n", writes);
        }
    }
    fclose(*f);
    free(memory->page_table);
    free(memory);

    if (debugger == 1)
    {
        printf("frames: %d \n", frame);
        printf("traces: %d \n", traces);
        printf("reads: %d \n", reads);
        printf("writes: %d \n", writes);
    }
}

//Algoritm to remove the element that was first added
void FIFO(FILE **f, int frame, int debugger)
{
    memory = (struct queue *)malloc(sizeof(struct queue));
    memory->page_table = (struct page *)malloc(sizeof(struct page) * frame);
    memory->size = frame;
    memory->index = 0;
    reads = writes = traces;

    while (fscanf(*f, "%x %c", &addr, &rw) != EOF)
    {
        struct page *pageTraced;
        pageTraced = (struct page *)malloc(sizeof(struct page));
        pageTraced->page_frame = addr >> 12;
        pageTraced->dirty = 0;
        traces++;

        if (debugger == 0)
        {
            printf("Address requested: %x \n", addr);
            printf("Page requested: %x \n", pageTraced->page_frame);
        }

        //If the address is present
        if (findIndex(memory, pageTraced) != -1)
        {
            if (debugger == 0)
            {
                printf("Page found in mmeory \n");
                if (rw == 'W')
                    printf("Page marked drity \n");
            }
            if (rw == 'W')
                memory->page_table[findIndex(memory, pageTraced)].dirty = 1;
        }
        else
        {
            //If the memory is full, then do this
            if (memory->index == memory->size)
            {
                if (memory->page_table[0].dirty == 1)
                    writes++;
                delete (memory);
            }

            reads++;
            if (rw == 'W')
                pageTraced->dirty = 1;
            insert(memory, pageTraced);
        }

        if (debugger == 0)
        {
            printf("frames: %d \n", frame);
            printf("traces: %d \n", traces);
            printf("reads: %d \n", reads);
            printf("writes: %d \n", writes);
        }
    }
    fclose(*f);
    if (debugger == 1)
    {
        printf("frames: %d \n", frame);
        printf("traces: %d \n", traces);
        printf("reads: %d \n", reads);
        printf("writes: %d \n", writes);
    }
}

//Algorithm for VMS
void VMS(FILE **f, int frame, int debugger)
{

    queue_A = (struct queue *)malloc(sizeof(struct queue));
    queue_A->page_table = (struct page *)malloc(sizeof(struct page) * ((frame / 2) + 1));
    queue_A->size = frame;
    queue_A->index = 0;

    queue_B = (struct queue *)malloc(sizeof(struct queue));
    queue_B->page_table = (struct page *)malloc(sizeof(struct page) * ((frame / 2) + 1));
    queue_B->size = frame;
    queue_B->index = 0;

    clean = (struct queue *)malloc(sizeof(struct queue));
    clean->page_table = (struct page *)malloc(sizeof(struct page) * ((frame / 2) + 1));
    clean->size = frame;
    clean->index = 0;

    dirty = (struct queue *)malloc(sizeof(struct queue));
    dirty->page_table = (struct page *)malloc(sizeof(struct page) * ((frame / 2) + 1));
    dirty->size = frame;
    dirty->index = 0;

    reads = writes = traces;

    while (fscanf(*f, "%x %c", &addr, &rw) != EOF)
    {
        struct page *pageTraced;
        pageTraced = (struct page *)malloc(sizeof(struct page));

        struct page *temp;
        temp = (struct page *)malloc(sizeof(struct page));

        pageTraced->page_frame = addr >> 12;
        pageTraced->dirty = 0;
        traces++;

        if (debugger == 0)
        {
            printf("Address requested: %x \n", addr);
            printf("Page requested: %x \n", pageTraced->page_frame);
        }

        if ((findIndex(queue_A, pageTraced) != -1) || (findIndex(queue_B, pageTraced) != -1))
        {
            if (rw == 'W')
            {
                if ((addr >> 28) == 3)
                    queue_A->page_table[findIndex(queue_A, pageTraced)].dirty = 1;
                else
                    queue_B->page_table[findIndex(queue_B, pageTraced)].dirty = 1;
            }
        }
        else
        {
            if (findIndex((clean), pageTraced) != -1)
            {
                temp->page_frame = pageTraced->page_frame;
                temp->dirty = pageTraced->dirty;
                shiftRight(clean, findIndex(clean, pageTraced));
                clean->index = clean->index - 1;
            }
            else if (findIndex((dirty), pageTraced) != -1)
            {
                temp->page_frame = pageTraced->page_frame;
                temp->dirty = pageTraced->dirty;
                shiftRight(dirty, findIndex(clean, pageTraced));
                dirty->index = dirty->index - 1;
            }
            else
            {
                reads++;
                temp->page_frame = pageTraced->page_frame;
                temp->dirty = pageTraced->dirty;
            }
            if ((queue_A->index) >= (frame >> 1) && (addr >> 28) == 3)
            {
                if (queue_A->page_table[queue_A->index - 1].dirty == 0)
                {
                    insert(clean, (&queue_A->page_table[queue_A->index - 1]));
                    shiftRight(queue_A, 0);
                    queue_A->index = queue_A->index - 1;
                }
                else
                {
                    insert(dirty, (&queue_A->page_table[queue_A->index - 1]));
                    shiftRight(queue_A, 0);
                    queue_A->index = queue_A->index - 1;
                }
            }
            if ((queue_B->index) >= (frame >> 1) && (addr >> 28) != 3)
            {
                if (queue_B->page_table[queue_B->index - 1].dirty == 0)
                {
                    insert(clean, (&queue_B->page_table[queue_B->index - 1]));
                    shiftRight(queue_B, 0);
                    queue_B->index = queue_B->index - 1;
                }
                else
                {
                    insert(dirty, (&queue_B->page_table[queue_B->index - 1]));
                    shiftRight(queue_B, 0);
                    queue_B->index = queue_B->index - 1;
                }
            }
            if (((queue_A->index) + (queue_B->index) + (clean->index) + (dirty->index)) >= frame)
            {
                if (clean->index != 0)
                {
                    shiftRight(clean, 0);
                    clean->index = clean->index - 1;
                }
                else if (dirty->index != 0)
                {
                    shiftRight(dirty, 0);
                    dirty->index = dirty->index - 1;
                    writes++;
                }
            }
            if ((queue_A->index) < (frame >> 1) && (addr >> 28) == 3)
            {
                if (rw == 'W')
                    temp->dirty = 1;
                insert(queue_A, temp);
            }
            else if ((queue_B->index) < (frame >> 1) && (addr >> 28) != 3)
            {
                if (rw == 'W')
                    temp->dirty = 1;
                insert(queue_B, temp);
            }
        }

        if (debugger == 0)
        {
            printf("frames: %d \n", frame);
            printf("traces: %d \n", traces);
            printf("reads: %d \n", reads);
            printf("writes: %d \n", writes);
        }
    }

    if (debugger == 1)
    {
        printf("frames: %d \n", frame);
        printf("traces: %d \n", traces);
        printf("reads: %d \n", reads);
        printf("writes: %d \n", writes);
    }
}

//Algoritm that randomly removes the element
void RMD(FILE **f, int frame, int debugger)
{
    memory = (struct queue *)malloc(sizeof(struct queue));
    memory->page_table = (struct page *)malloc(sizeof(struct page) * frame);
    memory->size = frame;
    memory->index = 0;
    reads = writes = traces;

    while (fscanf(*f, "%x %c", &addr, &rw) != EOF)
    {
        struct page *pageTraced;
        pageTraced = (struct page *)malloc(sizeof(struct page));
        pageTraced->page_frame = addr >> 12;
        pageTraced->dirty = 0;
        traces++;

        if (debugger == 0)
        {
            printf("Address requested: %x \n", addr);
            printf("Page requested: %x \n", pageTraced->page_frame);
        }

        //If the address is there
        if (findIndex(memory, pageTraced) != -1)
        {
            if (debugger == 0)
            {
                printf("Page found in memory \n");
                if (rw == 'W')
                    printf("Page marked dirty \n");
            }
            if (rw == 'W')
                memory->page_table[findIndex(memory, pageTraced)].dirty = 1;
        }

        //Address is not there
        else
        {
            //Only do this if the memory is full
            if(memory->index == memory->size)
            {
                memory->index = rand()%(memory->size);
                if (memory->page_table[0].dirty == 1)
                    writes++;
                delete(memory);
            }

            reads++;
            if (rw == 'W')
                pageTraced->dirty = 1;
            
            //Inserts the pageTraced into memory, which would be the previously deleted spot
            insert(memory, pageTraced);
        }

        if (debugger == 0)
        {
            printf("frames: %d \n", frame);
            printf("traces: %d \n", traces);
            printf("reads: %d \n", reads);
            printf("writes: %d \n", writes);
        }
    }
    fclose(*f);
    if (debugger == 1)
    {
        printf("frames: %d \n", frame);
        printf("traces: %d \n", traces);
        printf("reads: %d \n", reads);
        printf("writes: %d \n", writes);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 5)
        printf("Format: memsim tracefile nframes LRU|FIFO|VMS debug|quiet");

    FILE *f;
    f = fopen(argv[1], "r");

    //Number of frames in Virtual Memory
    int n = atoi(argv[2]);

    if (!strcmp(argv[4], "debug"))
        debugger = 0;
    else if (!strcmp(argv[4], "quiet"))
        debugger = 1;

    //Determines what algorithm it uses. Takes capital and lower case
    if (strcmp(argv[3], "LRU") == 0 || (strcmp(argv[3], "lru")) == 0)
        LRU(&f, n, debugger);
    else if (strcmp(argv[3], "FIFO")==0 || (strcmp(argv[3], "fifo")==0))
        FIFO(&f, n, debugger);
    else if (strcmp(argv[3], "VMS") ==0||(strcmp(argv[3], "vms"))==0)
        VMS(&f, n, debugger);
    else if(strcmp(argv[3], "RMD")==0|| (strcmp(argv[3], "rmd"))==0)
        RMD(&f, n, debugger);
    else
    {
        printf("That method is not recognized");
        return 0;
    }
}