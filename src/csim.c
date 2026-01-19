//88eb9e3f2708251f1f2f778f6df7218c7204e554
//5:36
/*
AUTHOR: Matthew Rackley
DATE: 04/20/2023
PURPOSE: Cache hit and miss simulation.
*/

#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>


struct line {
    unsigned int valid; //Valid bit 0 = invalid, !0 = valid
    unsigned int tag; //Tag for the line
    unsigned int block; //Will fix this later to take an arbitrary size block.
    unsigned int count;
};

//void printSummary(int x, int y, int z);
void printBits(unsigned int num, int size);
//void printCache(struct line **cache,int S,int E);
void args(int argc, char *argv[], unsigned int *s, unsigned int *E, unsigned int *b, unsigned int *verbose);
void search(struct line **cache, int E, int tag, int setIndex, int block, int *hit, int *miss, int *eviction,unsigned int verbose);


FILE* open(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    //printf("INSIDE MAIN\n");
    unsigned int s; //Number of set index bits
    unsigned int E; //Number of lines per set. 
    unsigned int b; //Block offset bits.
    unsigned int verbose = 0; //Verbose mode    
    args(argc, argv, &s, &E, &b, &verbose); //Establish Args
    //printf("AFTER ARGS\n");
    unsigned int S = pow(2,s); //Number of cache sets.
    //unsigned int B = pow(2,b); // Block size in bytes.
    unsigned int m = 32; // physical address bit size.
    //unsigned t = m - (b+s); ; // Number of tag bits.
    //unsigned int C = (S*E*B); // Cache size (bytes) not including valid + tag.

    FILE *filePointer = open(argc, argv); // Establish file name.


    //printf("verbose:%d s:%d S:%d E:%d b:%d B:%d m:%d t:%d C:%d \n",verbose,s,S,E,b,B,m,t,C);

    //cache[i][j] where i = set index, and j is the line index in the set.
    //printf("Malloc\n");
    struct line **cache = malloc(S*sizeof(struct line *));
    for (int i = 0; i < S; i++){
        cache[i] = malloc(E*sizeof(struct line));
        for(int j = 0; j < E; j++){
            cache[i][j] = (struct line) {0};            
        }
    }
    //printf("End of Malloc\n");
    //printCache(cache, S, E);


    
    char operation;
    unsigned int address;
    int size; 
    int t_bits;
    int s_bits;
    int b_bits;
    int hit = 0;
    int miss = 0;
    int eviction = 0;
    while (fscanf(filePointer, " %c %x,%d", &operation,&address,&size) != EOF){
        //printf("fscanf != EOF\n");
        if (operation == 'I') continue; //Ignoring the I command.
        //printf("Operation: %c, Address: %x, Size: %d\n", operation, address, size);
        t_bits = (unsigned int)address >> (s+b);
        //printBits(address, 32);
        s_bits = address;
        s_bits = s_bits << (m - s - b);
        //printBits(s_bits,32);
        s_bits = (unsigned int)s_bits >> (m - s); 
        //printBits(s_bits,32);
        
        b_bits = address << (m - b);
        b_bits = (unsigned int)b_bits >> (m - b);
        //printf("t_bits: %u  s_bits: %u  b_bits: %u\n\n",t_bits,s_bits,b_bits);
        if(verbose == 1) printf("%c %x,%d ",operation,address,size);
        if (operation == 'M'){
            //printf("M Search 1\n");
            search(cache,E,t_bits,s_bits,b_bits,&hit,&miss,&eviction,verbose);
            //printf("M Search 2\n");
            search(cache,E,t_bits,s_bits,b_bits,&hit,&miss,&eviction,verbose);
        }else{ 
            //printf("Else Search\n");
            search(cache,E,t_bits,s_bits,b_bits,&hit,&miss,&eviction,verbose);
        }
        if(verbose == 1) printf("\n");


    }


    //printf("Free Memory\n");
    for (int i = 0; i < E; i++) {
      free(cache[i]);
    }
    free(cache);
    //printf("printSummary\n");

    printSummary(hit, miss, eviction);

    
    return 0;
}

/* void printCache(struct line **cache, int S, int E) {
    for (int i = 0; i < S; i++) {
        for (int j = 0; j < E; j++) {
            printf("Set %d, Line %d: Valid: %u Tag: %u Block: %u\n", i, j, cache[i][j].valid, cache[i][j].tag, cache[i][j].block);
        }
    }
} */

/* void printSummary(int x, int y, int z){
    printf("hits:%d misses:%d evictions:%d\n",x,y,z);

} */

FILE* open(int argc, char* argv[]){
    FILE* filePointer = NULL;
    if (argc == 9){ //If verbose mode is not activated.
        if((filePointer = fopen(argv[8],"r")) == NULL){
            printf("Error opening file.\n");
            exit(1);
        }; 
    }
    if (argc == 10){
        if((filePointer = fopen(argv[9],"r")) == NULL){
            printf("Error opening file.\n");
            exit(1);
        };
    }
    return filePointer;
}


void args(int argc, char *argv[], unsigned int *s, unsigned int *E, unsigned int *b, unsigned int *verbose){
    //printf("Inside args()\n");
    if (argc == 2){
        printf("The following is the format for csim.c:\n ./csim [-v] -s <s> -E ,E. -b <b> -t <tracefile>\n where -v is optional to run verbose mode.\n");
        exit(0);
    }

    if (argc == 9){ //If verbose mode is not activated.
        *s = atoi(argv[2]);
        *E = atoi(argv[4]); 
        *b = atoi(argv[6]);
    }
    if (argc == 10){
        *verbose = 1; //If verbose mode IS activated.
        *s = atoi(argv[3]);
        *E = atoi(argv[5]); 
        *b = atoi(argv[7]);
    }
    //printf("Args() returning...\n");
}

void search(struct line **cache, int E, int tag, int setIndex, int block, int *hit, int *miss, int *eviction, unsigned int verbose){
    int check = 0;
    int j = 0;
    //for (j; j < E ; j++) {
    while(j < E){
        cache[setIndex][j].count++;
        
        if (cache[setIndex][j].valid && cache[setIndex][j].tag == tag){
            check = 1; //State machine
            (*hit)++; //Increment Hit
            //setting values to the struct
            cache[setIndex][j].block = block;
            cache[setIndex][j].count = 0;
            
            if(verbose == 1) printf("hit ");


            
            break;
        } else if (!cache[setIndex][j].valid) {
            check = 1; // state machine
            (*miss)++; // increment miss
            //Setting values to the struct
            cache[setIndex][j].valid = 1;
            cache[setIndex][j].tag = tag;
            cache[setIndex][j].block = block;
            cache[setIndex][j].count = 0;
            
            if(verbose == 1) printf("miss ");
            
            break;
        }
        j++;//Delete me please i'm deseperate!
    }
    
    j++;
    while( j < E){
        cache[setIndex][j].count++; //Increment the rest...Hopefully.
        j++;
    } 

    if(check == 0){
        
        (*miss)++;
        (*eviction)++;
        
        int maxIndex = 0;
        int max = 0;
        for (int j = 0; j < E; j++){
            if (cache[setIndex][j].count > max) {
                max = cache[setIndex][j].count;
                maxIndex = j;
            }
        }

        cache[setIndex][maxIndex].valid = 1;
        cache[setIndex][maxIndex].tag = tag;
        cache[setIndex][maxIndex].block = block;
        cache[setIndex][maxIndex].count = 0;
        if(verbose == 1) printf("miss eviction ");
    }

}





/*
*printBits prints a binary representation of integer up to size places.
*@param unsigned int as the number to be printed.
*@param int the size of the bitvector.
*@return Void but prints to console.
*/
 void printBits(unsigned int num, int size){
    for(int i = size-1; i >= 0 ;i--){
       if(num & (1 << i)) printf("1");
       else printf("0");
    }
    printf("\n");
}