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
int file_size(FILE * fstream, int *row, int *col)
{
    FILE * fstream_inside = fstream;
    char buffer[BUFFER_MAX];
    char *record, *line;
    int i = 0, j = 0;
    int first_time_recorder = 0;
    float test_char = 0.0;
    
    
    if (fstream_inside == NULL)   {
        printf("\n file opening failed \n");
        return -1;
    }
    // This will ignore the first line of data
    while ((line = fgets(buffer, sizeof(buffer), fstream_inside)) != NULL)
    {
        // this will eliminate the first line when it is the label of each column
        record = strtok(line, ",");
        while (first_time_recorder == 0 && record != NULL) {
            test_char = test_char +  atof(record);
            record = strtok(NULL, ",");
        }

        
        if (first_time_recorder == 0) {
            if (test_char == 0.0 ) {
                first_time_recorder++;
                continue;
            }
            else{
                // rewind if the first line is data
                first_time_recorder++;
                rewind(fstream_inside);
                line = fgets(buffer, sizeof(buffer), fstream_inside);
                record = strtok(line, ",");
            }
        }
        
        
        // this will record the col number for the second row;
        // It will ignore the first line no matter it's a line of data or line of lable
        j = 0;
        while (record != NULL && i != 0)
        {
            ++j;
            record = strtok(NULL, ",");
        }
        // this will record the row number
        // But it will include the first line. If the first line is lable, remaining program will handle.
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
    int first_time_recorder = 0;
    float test_char = 0;
    //char *first_line_ptr = line;
    
    if (fstream_inside == NULL)   {
        printf("\n file opening failed\n");
        return -1;
    }
    
    while ((line = fgets(buffer, sizeof(buffer), fstream_inside)) != NULL)
    {

        // this will eliminate the first line when it is the label of each column
        record = strtok(line, ",");
        while (first_time_recorder == 0 && record != NULL) {
            test_char = test_char +  atof(record);
            record = strtok(NULL, ",");
        }

        if (first_time_recorder == 0) {
            if (test_char == 0.0 ) {
                first_time_recorder++;
                continue;
            }
            else{
                // rewind if the first line is data
                first_time_recorder++;
                
                rewind(fstream_inside);
                line = fgets(buffer, sizeof(buffer), fstream_inside);
                record = strtok(line, ",");
            }
        }
        
        // this will read file from first line if it's not lable, or the second line if it's data no matter the first col is 0 or not
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
