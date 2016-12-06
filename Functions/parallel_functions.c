//
//  parallel_functions.c
//  Parallel_C_Codes
//
//  Created by zhangcaihua on 11/21/16.
//  Copyright © 2016 zhangcaihua. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "parallel_functions.h"
#include <math.h>

/* CSV file size
    This will not count the label row at the top of CSV file
*/
int File_Size(FILE * fstream, int *row, int *col)
{
    FILE * fstream_inside = fstream;
    char buffer[BUFFER_MAX];
    char *record, *line;
    int i = 0, j = 0;
    
    if (fstream_inside == NULL)   {
        printf("\n file opening failed \n");
        return -1;
    }
    // This will ignore the first line of data
    while ((line = fgets(buffer, sizeof(buffer), fstream_inside)) != NULL)
    {
        record = strtok(line, ",");
        // this will record the col number for the second row;
        j = 0;
        while (record != NULL)
        {
            ++j;
            record = strtok(NULL, ",");
        }
        // this will record the row number
        ++i;
    }
    // this will change variables' value in calling function
    *row = i;
    *col = j;
    // this will rewind the file pointer for later use
    rewind(fstream_inside);
    return 0;
}




/*Data read function from CSV file*/
/*
 First, the reading sequence is row by row. So, when output data, please use the same order.
 Second, all data come from CSV file are converted to type double
 */
int Data_Read( FILE * fstream, int row, int col, double* info)
{
    FILE * fstream_inside = fstream;
    char buffer[BUFFER_MAX];
    char *record, *line;
    int i = 0, j = 0;
    //char *first_line_ptr = line;
    
    if (fstream_inside == NULL)   {
        printf("\n file opening failed\n");
        return -1;
    }
    
    while ((line = fgets(buffer, sizeof(buffer), fstream_inside)) != NULL)
    {
        record = strtok(line, ",");
        j = 0;
        while (record != NULL)
        {
            info[j + i*col] = atof(record);
            ++j;
            record = strtok(NULL, ",");
        }
        ++i;
    }
    
    printf("\n");
    rewind(fstream_inside);
    return 0;
}


/*Structure Read*/
// This will let the structure store the address of beginning of each row. The array will be parsed as rows through column number even the origional data array is a very long array
// This will read double variables array
void Data_StoreTo_Structure (double ** struct_member_ptr, double * array_data_ptr,int row, int col) // This will accept the address of first element of the structure. The structure will store same type of pointers.
{
    
    //struct_member_ptr = &GenCorrectiveDispatchLimitData_Set.Gen_BusNum; // This is the first element of the structure
    for (int i = 0; i < row; i ++){
        //printf("%p\n", struct_member_ptr);
        //struct_member_ptr++;
        *struct_member_ptr = &array_data_ptr[i*col];
        struct_member_ptr++;
        //printf("%p\n", *struct_member_ptr);
    }
}



/*Data read correctiveness testing*/
int Data_Read_Corrective_Test(int row, int col, double *testing_array, char*name)
{
    
    FILE *outputfile;
    outputfile = fopen(name, "w");
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            fprintf(outputfile, "%f\t", testing_array[j+col*i]);
        }
        fprintf(outputfile, "\n");
    }
    
    fclose(outputfile);
    return 0;
}
