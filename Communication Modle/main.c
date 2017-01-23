//
//  main.c
//  Parallel_C_Codes:UC and TS
//
//  Created by Dr.Fu's group on 12/2016.
//  Copyright © 2016 Dr.Fu's group. All rights reserved.
//


/*The following h files are included in master node & all slave nodes respectively for only 1 time*/    
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "gurobi_c.h"
#include "declare_function.h"
#include "declare_variable.h"
#include "declare_MPIrelated_function.h"
#include "define_macros.h"



int main(int argc,  char * argv[]) {
    
    int row_TaskTable, col_TaskTable;
    
    /* ******* Read size of CSV files ******* */
    //TaskTable size is read
    FILE *f_TaskTable_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV_updated/Tasktable.csv", "r");
    File_Size(f_TaskTable_stream, &row_TaskTable, &col_TaskTable);// Get rows and cols from file
    //printf("row=%d, col=%d\n", row_TaskTable,col_TaskTable);
    
    //TaskTableData Read but is should be type INT instead of DOUBLE
    int *TaskTableData;
    TaskTableData = calloc(row_TaskTable*col_TaskTable, sizeof(int));
    Data_Read_Int(f_TaskTable_stream, row_TaskTable, col_TaskTable, TaskTableData);
    
    int *TaskTable_TaskArray[row_TaskTable];//This will give each task a row to demonstrate its 15 values
    for (int i = 0; i < row_TaskTable; i++) {
        TaskTable_TaskArray[i] = &TaskTableData[i*col_TaskTable];
    }
    
    /* This is for test data read
    for (int i = 0; i < row_TaskTable; i++) {
        for (int j = 0; j < col_TaskTable; j++) {
            printf("%d\t",TaskTable_TaskArray[i][j]);
        }
        printf("\n");
    }
    */
    
    /* ******* Identify task belonging ******* */
    //It is important to know that the all variables will start from 0 INSTEAD of 1.
    //This will identify each task belongs to which node
    int node[row_TaskTable], scenario[row_TaskTable], module[row_TaskTable], task[row_TaskTable];
    
    for (int i = 0; i < row_TaskTable; i++) {
        task_allocation(TaskTable_TaskArray, i, &node[i], &scenario[i], &module[i], &task[i]);
//        printf("\n");
//        printf("node number is %d\t", node[i]);
//        printf("scenario number is %d\t", scenario[i]);
//        printf("module number is %d\t", module[i]);
//        printf("task number is %d\n", task[i]);

    }
    
    
    /* ******* OpenMPI Task Assignment ******* */
    // Any variable with dummy should be changed. Else variables may need to keep.
    int id;
    int ierr;
    int node_size;
    int id_masternode = 0;
    
    int datasizearray_row = 1; // This determine size the datasizearray arrangement 1*2. This array has two items
    int datasizearray_col = 2;//The first column is data row number; the second column is data column number
    
    int dummy_master_send_tag = 1;
    int dummy_master_recv_tag = 2;

    
    ierr = MPI_Init ( &argc, &argv );
    ierr = MPI_Comm_rank ( MPI_COMM_WORLD, &id );
    ierr = MPI_Comm_size ( MPI_COMM_WORLD, &node_size );
    
    //This is related to Master node
    if (id == id_masternode) { //Node 0 is define as master node
        
        int stop_counter;
        int MPI_Iteration_Times = 2;
        int stop_decision = 0; //Initialize stop decision. top_decision=0, continue; stop_decision=1, stop.
        
        
        // This is the fake input. Just use for testing
        int dummy_row = 1;
        int dummy_col = 3;
        double dummy_send[dummy_row*dummy_col];
        double dummy_recv_master; // This means that master node will receve 2 out comes from slave node each time
        dummy_send[0]= 7;
        dummy_send[1]= 15;
        dummy_send[2]= 6;
        double send_sizeinfo[2]={dummy_row,dummy_col};// This sending array info. This should be keep.
        
        char filename[80];
        FILE * fp_recv[node_size-1];// receve slave nodes message
        
        for (int i = 0; i < node_size-1; i++) {
            sprintf(filename, "/Users/zhangcaihua/Desktop/SCUC_Parallel_C/Build/Products/Debug/recv_node%d.txt", i+1);
            fp_recv[i] = fopen(filename, "w+");
        }
        
        
        for (stop_counter=0;  ; stop_counter++)
        {
            if (stop_counter == 0) // First time enter the MPI communication
            {
                //This will define whether MPI iterations will stop and send decision to each node
                MasterNode_send_stopdecision(node_size , &stop_decision); // stop_decision=0, continue; stop_decision=1, stop.
                
                
                // This part related to data sending from Master node to Slave node
                
                //Send data size and data
                for (int i = 0 ; i  < row_TaskTable; i++) {//Iterate all tasks within TaskTable. TaskTable can from 0 to number of task -1.
                    for (int j = 1; j < node_size; j++) {//Find each task to go. j should start at 1 due to the 0 is master node
                        if (node[i] == j) {
                            /* *****This part should change VERY carefully to determine the sending data. Instrunctions are provided below, which have 3 lines of comments****** */
                            /*This place should determine the sending data*/
                            //The "dummy_send" is the array to hold the data.
                            //The "node", "scenario", and "module" arrays can use to decide the sending data
                            
                            //Send data size
                            //Send task data size information. 1 means 1 row, 2 means 2 col. [row, col]: 1*2
                            MasterNode_sendto_SlaveNode(send_sizeinfo, node[i], dummy_master_send_tag, datasizearray_row, datasizearray_col);
                            
                            //Send data
                            MasterNode_sendto_SlaveNode(dummy_send, node[i], dummy_master_send_tag, dummy_row, dummy_col);//Send task data from node master to node j
                        }//END if
                    }//END j loop
                }// END i loop
                
                
                // This part related to data receving from Slave node to Master node
                for (int i = 1; i < node_size; i++) {
                    
                    MasterNode_receivefrom_SlaveNode(&dummy_recv_master, i, dummy_master_recv_tag, 1, 1); //Receive iteration times from nodes , which is 1 row and 1 col.
                    //printf("I am node %d, recv is %f\n", id, dummy_recv_master);
                }
            
            }//End stop_counter == 0 if part
            
            
            
            // Next time enter the MPI iteration
            else
            {
                
                // This is stop criterion
                if (stop_counter >= MPI_Iteration_Times) { // If the iteration run TWICE, the MPI loop will terminate
                    stop_decision = 1;
                    MasterNode_send_stopdecision(node_size , &stop_decision); // stop_decision=0, continue; stop_decision=1, stop.
                    break;
                    //printf("I am node %d, stop_counter: %d\n",id, stop_counter);
                }
                else{
                    stop_decision = 0;
                    MasterNode_send_stopdecision(node_size , &stop_decision); // stop_decision=0, continue; stop_decision=1, stop.
                }
                
                
                // This part related to data sending from Master node to Slave node
                
                //Send data
                for (int i = 0 ; i  < row_TaskTable; i++) {//Iterate all tasks within TaskTable. TaskTable can from 0 to number of task -1.
                    for (int j = 1; j < node_size; j++) {//Find each task to go. j should start at 1 due to the 0 is master node
                        if (node[i] == j) {
                            /* *****This part should change VERY carefully to determine the sending data. Instrunctions are provided below, which have 3 lines of comments****** */
                            /*This place should determine the sending data*/
                            //The "dummy_send" is the array to hold the data.
                            //The "node", "scenario", and "module" arrays can use to decide the sending data
                            
                            //Send data size
                            //Send task data size information. 1 means 1 row, 2 means 2 col. [row, col]: 1*2
                            MasterNode_sendto_SlaveNode(send_sizeinfo, node[i], dummy_master_send_tag, datasizearray_row, datasizearray_col);
                            
                            //Send data
                            MasterNode_sendto_SlaveNode(dummy_send, node[i], dummy_master_send_tag, dummy_row, dummy_col);//Send task data from node master to node j
                        }//END if
                    }//END j loop
                }// END i loop

                
                // This part related to data receving from Slave node to Master node
                for (int i = 1; i < node_size; i++) {
                    MasterNode_receivefrom_SlaveNode(&dummy_recv_master, i, dummy_master_recv_tag, 1, 1); //Receive iteration times from nodes, which is 1 row and 2 col.
                    fprintf(fp_recv[i-1], "%f\n", dummy_recv_master);// Because file pointer start from 0.
                }// End for i loop
                
            }//End stop_counter == 0 else part
            
        }//End for whold sending and recving part
        
        
        /* ***finalize MPI*** */
        ierr = MPI_Finalize(); //All processors must call this routine before exiting. All processors will still exist but may not make any further MPI calls.
        
        //Close opened files
        for (int i = 0; i < node_size-1; i++) {
            fclose(fp_recv[i]);
        }
    }// END outside Master node
    
    
    //This is related to Slave node
    if (id != id_masternode) { //Nodes which are not Master node is define as Slave nodes
        
        
        int stop_decision_i;
        double output_i = 0;
        char filename[80];
        FILE * fp_recv;// receve slave nodes message
        sprintf(filename, "/Users/zhangcaihua/Desktop/SCUC_Parallel_C/Build/Products/Debug/slave%d_recv.txt", id);
        fp_recv = fopen(filename, "w+");
        
        while (1) {
            //Receive stop decision from master node
            SlaveNode_receive_stopdecision(&stop_decision_i, id_masternode, id);// The Tag of recving end is it's node's ID
            
            
            //If stop decision equals to 0, which means not stop, program in slave node will continue
            if (stop_decision_i == 0) {
                
                
                for (int i = 0 ; i  < row_TaskTable; i++) {//Iterate all tasks within TaskTable. TaskTable can from 0 to number of task -1.
                    if (node[i] == id) {
                        
                        // This part is related to data receving
                        double recv_sizeinfo[2];
                        SlaveNode_receivefrom_MasterNode (recv_sizeinfo, id_masternode, dummy_master_send_tag, datasizearray_row, datasizearray_col);//Receving task data size info from master node.
                        
                        double dummy_recv_slave[(int)(recv_sizeinfo[0]*recv_sizeinfo[1])];//the first item is array row number, the second item is array col number.
                        SlaveNode_receivefrom_MasterNode (dummy_recv_slave, id_masternode, dummy_master_send_tag, (int)recv_sizeinfo[0], (int)recv_sizeinfo[1]);//Receving task data size info from master node.
                        
                        for (int i = 0; i < (int)recv_sizeinfo[0]; i++) { //receive data size: row
                            for (int j = 0; j < (int)recv_sizeinfo[1]; j++) {//receive data size: col
                                fprintf(fp_recv, "%f\t", dummy_recv_slave[j+i*(int)recv_sizeinfo[1]]);// Because file pointer start from 0.
                            }
                            fprintf(fp_recv, "\n");// Because file pointer start from 0.
                        }
                        
                    }//END if
                }// END i loop
                fprintf(fp_recv, "\n\n");// Because file pointer start from 0.
                
                
                // This part is related to send data back to master node
                SlaveNode_sendto_MasterNode(&output_i, id_masternode, dummy_master_recv_tag, 1, 1);
                //printf("I am node %d, send back %f\n", id, output_i);
                output_i=output_i+1;
            }
            
            else if (stop_decision_i == 1){
                
                break; // This will end receving iteration
            }
            
        }
        
        /* ***finalize MPI*** */
        ierr = MPI_Finalize(); //All processors must call this routine before exiting. All processors will still exist but may not make any further MPI calls.
        
        //Close opened files
        fclose(fp_recv);

        
    }// END outside if

    
    
    
    free(TaskTableData);//Relase unused memory
    fclose(f_TaskTable_stream); //Relase file pointer when program finish
    return 0;

}//END:Main codes





