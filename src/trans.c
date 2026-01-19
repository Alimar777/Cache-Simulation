/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    //int* address = &A[0][0];
    //printf("For %dx%d Matrix - Address: %p\n",N,M,(void*)address);
    //int i, j, row, col;
    int block;
    int temp;
    //Jason said divide into sub-arrays so maybe this?
    //Iterate over the blocks?
    if(M == 32 || M == 61){
        if (M == 32) block = 8;
        if (M == 61) block = 17;
        //Swapping around i and j to be j then i resultd in improvment of the non-square matrix.
        for (int j = 0; j < M; j+= block) {
            for (int i = 0; i < N; i+= block) {
                //Increment by 1 inside the sub matrix.
                for(int row = i; row < i + block && row < N; row++){
                    for(int col = j; col < j + block && col < M; col++){
                        //If it's diagonal then keep the same?
                        //if(row == col) temp = A[row][col];
                        //if(row != col) B[col][row] = A[row][col]; 
                        //B[col][row] = A[row][col]; //Naive version
                        if(row == col) temp = *(*(A + row) + col);
                        if(row != col) *(*(B + col) + row) = *(*(A + row) + col);



                    }
                    //if (i == j) B[row][row] = temp;
                    if (i == j) *(*(B + row) + row) = temp;
                }
            }
        }  
    }
    
    if (M == 64) {
        int temp_row,temp_row_2,temp_col,temp_col_2;
        block = 8; 
        int stride = 4; //stride for repeated sets in same block
        for(int row = 0; row < M; row += block){
            for(int col = 0; col < N; col += block){
                //Do col even need these vars?
                //int row = j;
                //int col = i;
                
                //Buffer 1
                temp_row = row;
                temp_col = col;
                do{
                    temp_col += block;
                    if(temp_col >= N){
                        temp_row += block;
                        temp_col = 0;
                    }
                }while(row == temp_col);

                //Why doesn't this work?
                /*
                if((temp_col += block) >= N){
                    temp_row += block;
                    temp_col = 0;
                }*/

                //Buffer 2
                temp_row_2 = temp_row;
                temp_col_2 = temp_col;
                do{
                    temp_col_2 += block;
                    if (temp_col_2 >= N) {
                        temp_row_2 += block;
                        temp_col_2 = 0;
                    }
                } while(row == temp_col_2);



                if (temp_row >= M) { 
                    //Remember row and col are the block steps
                    for (int i = 0; i < block; i++){
                        for(int j = 0; j < block; j++){
                            //if(i == j) temp = A[col+i][row+j];
                            if(i == j) temp = *(*(A+col+i)+row+j);
                            //if(i != j) B[row+j][col+i] = A[col+i][row+j];
                            if(i != j) *(*(B + row + j) + col + i) = *(*(A + col + i) + row + j);
                            //B[row+j][col+i] = A[col+i][row+j];
                        }
                        //B[row+i][col+i] = temp;
                        *(*(B + row + i)+ col + i) = temp;
                    }
                } else {
                    //Dump values into temporary storage of B
                    //Keep aware that each set of non repeating lines needs to be done separately to avoid misses.
                    for(int i = 0; i < stride; i++){
                        for(int j = 0; j < block; j++){
                            //B[temp_row+i][temp_col+j] = A[col+i][row+j];
                            *(*(B + temp_row + i) + temp_col + j) = *(*(A + col + i) + row + j);

                            //B[temp_row+i][temp_col_2+j] = A[col+i+stride][row+j];
                        }
                    }
                    //Second stride block into matrix b buffer
                    for(int i = stride; i < block; i++){
                        for(int j = 0; j < block; j++){
                            //B[temp_row+i][temp_col+j] = A[col+i][row+j];
                            
                            //B[temp_row_2+i][temp_col_2+j] = A[col+i+stride][row+j];
                            *(*(B + temp_row_2 + i - stride) + temp_col_2 + j) = *(*(A + col + i) + row + j);
                        }
                    }
                    
                    //After both matrix B buffers are loaded, do final transpose.
                    //Need to divide the final transpose also into blocks of stride
                    //Not sure why this is the case but it seems to improve the hits a ton.
                    for(int i = 0; i < stride; i++){
                        for(int j = 0; j < stride; j++){ 
                            
                            //B[row+j][col+i] = B[temp_row+i][temp_col+j];
                            *(*(B + row + j)+ col + i) = *(*(B + temp_row + i) + temp_col + j);
                            //B[row+j][col+i+stride]= B[temp_row_2+i][temp_col_2+j];
                            *(*(B + row + j)+ col + i + stride) = *(*(B + temp_row_2 + i) + temp_col_2 + j);

                            //B[row+j+stride][col+i] = B[temp_row+i][temp_col+j+stride];
                            //B[row+j+stride][col+i+stride] = B[temp_row+i][temp_col_2+j+stride];
                        }
                    }
                    //Final transpose part 2.
                    for(int i = stride; i < block; i++){
                        for(int j = stride; j < block; j++){ 
                            //B[row+j][col+i] = B[temp_row+i][temp_col+j];
                            //B[row+j][col+i+stride]= B[temp_row+i][temp_col_2+j];
                            
                            
                            //B[row+j][col+i-stride] = B[temp_row+i-stride][temp_col+j];
                            *(*(B + row + j)+ col + i - stride) = *(*(B + temp_row + i - stride) + temp_col + j);
                            //B[row+j][col+i] = B[temp_row_2+i-stride][temp_col_2+j];
                            *(*(B + row + j)+ col + i) = *(*(B + temp_row_2 + i - stride) + temp_col_2 + j);
                        }
                    }
                }
                
            }
        }
    } 

}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

