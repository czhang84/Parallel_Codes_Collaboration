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
#include <mpi.h>

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


/*Array of type Double initialization*/
void Array_initial_double(double *Array_initialition, double length)
{
    int l = (int) length;
    for (int i = 0; i < l; i++) {
        Array_initialition[i] = 0;
    }
}


/*Array of type Int initialization*/
void Array_initial_int(int *Array_initialition, double length)
{
    int l = (int) length;
    for (int i = 0; i < l; i++) {
        Array_initialition[i] = (int)0;
    }
}

/* TIMESTAMP prints the current YMDHMS date as a time stamp */
void timestamp ( )
{
# define TIME_SIZE 40
    
    static char time_buffer[TIME_SIZE];//it exist through program execution. static storage duration, block scope, and no linkage. it's initialize just onece, when timestamp compiled. And set to 0 if not initialize it.
    const struct tm *tm; //value can't be modified by assignment or by incrementing or decrementing
    //the value of tm can be changed but tm points to a value that must remain constant
    time_t now;//type suitable for storing the calendar time
    
    now = time ( NULL );//calculate the current calender time and encodes it into time_t format
    tm = localtime ( &now );//the value of timer is broken up into the structure tm and expressed in the local time zone
    
    strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm );//Formats the time represented in the structure timeptr according to the formatting rules defined in format and stored into str.
    
    printf ( "%s\n", time_buffer );
# undef TIME_SIZE
}


/* Program stop decision */
void p0_stop_decision(int *stop_decision, int stop_counter, int end_point)
{
    if (stop_counter <= end_point )
    {
        *stop_decision = 1; //program will continue
    }
    else
    {
        *stop_decision = 0; // program will stop
    }
    //printf("this is %d loop, and the stop decision is %d \n", stop_counter, *stop_decision);
}


/* Master node send stop decision to slave nodes */
void p0_send_decision(int process_size,int stop_decision)
{
    int id;
    int ierr;
    int tag;
    int process_expect_master = process_size - 1; // The number of process decision need to be send equals that (total number of process) minius (master node)
    
    for (int i=1; i <= process_expect_master; i++)
    {
        id = i;
        tag = i;
        printf("the sent nonstop_decision%d is %d \n",id ,stop_decision);
        ierr = MPI_Send ( &stop_decision, 1, MPI_INT, id, tag, MPI_COMM_WORLD );
    }
}


/* This set each member of input array to 1. It should be adjust accordingly ******Gong****** */
void p0_set_input ( double *input, int row, int col )
{
    
    for (int i = 0; i < row; i++) {
        for (int j = 0 ; j < col; j++) {
            input[j + i*col]=1;
        }
    }
    
}


/* This function sends array to processes. The Tag of each message is equal to the ID of receiving process*/
void p0_send_input ( double *input, int receiving_process_ID, int receiving_process_tag, int row, int col )
{
    int ierr;
    int id_target = receiving_process_ID;
    int tag_target = receiving_process_tag;
    int coef_length = row * col;
    
    //printf("id is %d, tag is %d \n", id, tag);
    ierr = MPI_Send ( input, coef_length, MPI_DOUBLE, id_target, tag_target, MPI_COMM_WORLD );
}



/* p0_receive_output receives output from processes 1 and 2 */
void p0_receive_output ( double *output, int sending_process_ID, int sending_process_tag, int row, int col )
{
    int ierr;
    MPI_Status status;//except process 0, which is master process
    int recv_size = row * col;
    int id_source = sending_process_ID;
    int tag_source = sending_process_tag;
    
    ierr = MPI_Recv ( output, recv_size, MPI_DOUBLE, id_source, tag_source, MPI_COMM_WORLD, &status ); 
    printf ( "\n" );
    
    return;
}


/* Receive the stop decision from master node */
void p1_receive_decision(int *stop_decision_i, int sending_process_ID, int sending_process_tag)
{
    int ierr;
    MPI_Status status;
    
    ierr = MPI_Recv ( stop_decision_i, 1, MPI_INT, sending_process_ID, sending_process_tag, MPI_COMM_WORLD, &status ); // The stop_desion_i's size is only one int.
    
    //printf("process %d receive stop decision is %d \n", id, *stop_decision_i);
    return;
}



/* P1_RECEIVE_INPUT receives input from process 0 */
void p1_receive_input (double *input_i, int sending_process_ID, int sending_process_tag, int row, int col)
{
    int ierr;
    MPI_Status status;
    
    int recv_size = row * col;
    int id_source = sending_process_ID;
    int tag_source = sending_process_tag;
    
    ierr = MPI_Recv ( input_i, recv_size, MPI_DOUBLE, id_source, tag_source, MPI_COMM_WORLD, &status );
    /*
     for (int i =0; i<3 ; i++)
     {
     printf(" the coeff.  is %f \n", input_i[i] );
     }
     */
    return ;
}


void p1_compute_output (double *input_1, double* input_2, int stop_decision_i, int row, int col, double* result)
{
    
    
    static int initial_setup=1; // This will get rid of build model and add variable all the time
    
    if (initial_setup == 1) //start envirnoment setup
    {
        initial_setup = 0; // close initial setup when it already done
        
        // This part will filled with Gurobi INITIAL SETTING  ******Gong******
        
    }
    else //after envirnoment setup, no need setup again
    {
        
        //This part will filled with Gurobi UPDATED SETTING ******Gong******
        
    }
    //This part will filled with Gurobi CALCULATION ******Gong******
    
    
    //*************************************************This is FAKE************************************************* ******Gong******
    for (int i = 0; i < row; i++) {
        for(int j = 0; j < col; j++){
            result[j + i*col] = 0;
        }
    }
    //*************************************************This is FAKE************************************************* ******Gong******

}

/* p1_send_output sends output to process 0*/
void p1_send_output ( double *output_i, int receiving_process_ID, int receiving_process_tag, int row, int col )
{
    int ierr;
    int id_target = receiving_process_ID;
    int tag_target = receiving_process_tag;
    int recv_size = row * col;
    
    //printf(" process %d will send output %f \n", id, output_i);
    ierr = MPI_Send ( output_i, recv_size, MPI_DOUBLE, id_target, tag_target, MPI_COMM_WORLD );
    
    return;
}
