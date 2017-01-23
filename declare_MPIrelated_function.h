//
//  declare_MPIrelated_function.h
//  Declare all the functions for MPI use in the C_Codes: parallel computation of SCUC
//
//  Created by Dr.Fu's group on 12/16.
//  Copyright © 2016 Dr.Fu's group. All rights reserved.
//


#ifndef declare_MPIrelated_function_h
#define declare_MPIrelated_function_h

#include <stdio.h>


/*Master node functions*/
//void MasterNode_make_stopdecision(int *stop_decision, int stop_counter, int end_point);
void MasterNode_send_stopdecision(int process_size,int stop_decision);
//void MasterNode_set_input(double *input, int row, int col);
void MasterNode_sendto_SlaveNode(double *input, int receiving_process_ID, int receiving_process_tag, int row, int col);
void MasterNode_receivefrom_SlaveNode(double *output, int sending_process_ID, int sending_process_tag, int row, int col);

/*Slave node functions*/
void SlaveNode_receive_stopdecision(int *stop_decision_i, int sending_process_ID, int sending_process_tag);
void SlaveNode_receivefrom_MasterNode (double *input_i, int sending_process_ID, int sending_process_tag, int row, int col);
//void p1_compute_output (double *input_i, int stop_decision_i, int row, int col, double* result);
//void SlaveNode_compute_output(double *input_1, double* input_2, int stop_decision_i, int row, int col, double* result);
void SlaveNode_sendto_MasterNode(double *output_i, int receiving_process_ID, int receiving_process_tag, int row, int col);


#endif /* declare_MPIrelated_function_h */
