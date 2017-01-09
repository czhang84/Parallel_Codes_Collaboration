//
//  functions_MPIrelated.c
//  Parallel_C_Codes
//
//  Created by zhangcaihua on 11/21/16.
//  Copyright © 2016 zhangcaihua. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <mpi.h>
//#include "declare_variable.h"
#include "declare_MPIrelated_function.h"



/***********************************************************************************************************************/
/* **Master node make stop decision** */   //**??????????????????????????????????????????????******LIN******
/*void MasterNode_make_stopdecision(int *stop_decision, int stop_counter, int end_point)
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
}*/


/* **Master node send stop decision to slave nodes** */
void MasterNode_send_stopdecision(int process_size,int stop_decision)
{
    int id;
    int ierr;
    int tag;
    int process_expect_master = process_size - 1; // The number of process decision need to be send equals that (total number of process) minius (master node)
    
    for (int i=1; i <= process_expect_master; i++)
    {
        id = i;
        tag = i;
        //printf("the sent stop_decision%d is %d \n",id ,stop_decision);
        ierr = MPI_Send ( &stop_decision, 1, MPI_INT, id, tag, MPI_COMM_WORLD );
    }
}




/* This set each member of input array to 1. It should be adjust accordingly **??????????????????????????????????????????????******LIN****** */
//void MasterNode_set_input(double *input, int row, int col)
//{    
//    for (int i = 0; i < row; i++) {
//        for (int j = 0 ; j < col; j++) {
//            input[j + i*col]=1;
//        }
//    }    
//}





/* master node send array to slave node. The Tag of each message is equal to the ID of receiving process*/
void MasterNode_sendto_SlaveNode(double *input, int receiving_process_ID, int receiving_process_tag, int row, int col)
{
    int ierr;
    int id_target = receiving_process_ID;
    int tag_target = receiving_process_tag;
    int coef_length = row * col;
    
    //printf("id is %d, tag is %d \n", id, tag);
    ierr = MPI_Send ( input, coef_length, MPI_DOUBLE, id_target, tag_target, MPI_COMM_WORLD );
}



/* Master Node receive array from slave node */
void MasterNode_receivefrom_SlaveNode(double *output, int sending_process_ID, int sending_process_tag, int row, int col)
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



/***********************************************************************************************************************/
/* slave node receive the stop decision from master node */
void SlaveNode_receive_stopdecision(int *stop_decision_i, int sending_process_ID, int sending_process_tag)
{
    int ierr;
    MPI_Status status;
    
    ierr = MPI_Recv ( stop_decision_i, 1, MPI_INT, sending_process_ID, sending_process_tag, MPI_COMM_WORLD, &status ); // The stop_desion_i's size is only one int.
    
    //printf("process %d receive stop decision is %d \n", id, *stop_decision_i);
    return;
}



/* Slave Node recieve array from master node */
void SlaveNode_receivefrom_MasterNode(double *input_i, int sending_process_ID, int sending_process_tag, int row, int col)
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



/* Slave Node send array to master node 0*/
void SlaveNode_sendto_MasterNode(double *output_i, int receiving_process_ID, int receiving_process_tag, int row, int col )
{
    int ierr;
    int id_target = receiving_process_ID;
    int tag_target = receiving_process_tag;
    int recv_size = row * col;
    
    //printf(" process %d will send output %f \n", id, output_i);
    ierr = MPI_Send ( output_i, recv_size, MPI_DOUBLE, id_target, tag_target, MPI_COMM_WORLD );
    
    return;
}



/*void SlaveNode_compute_output(double *input_1, double* input_2, int stop_decision_i, int row, int col, double* result)//**??????????????????????????????????????????????******LIN******
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
}*/









