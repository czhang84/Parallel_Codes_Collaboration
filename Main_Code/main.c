//
//  main.c
//  Parallel_C_Codes
//
//  Created by zhangcaihua on 11/21/16.
//  Copyright © 2016 zhangcaihua. All rights reserved.
// /usr/local/bin/mpirun

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "parallel_functions.h"
#include <mpi.h>


int main(int argc,  char * argv[]) {
    /* ****Data Reading from CSV files**** */
    /* **Read size of CSV files ** */
    //GenCorrectiveDispatchLimitData Size
    int row_GenCorrective, col_GenCorrective;
    FILE *f_GenCorrective_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV_updated/GenCorrectiveDispatchLimitData.csv", "r");
    File_Size(f_GenCorrective_stream, &row_GenCorrective, &col_GenCorrective);// get rows and cols from file
    
    //GenData Size
    int row_GenData, col_GenData;
    FILE *f_GenData_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV_updated/GenData.csv", "r");
    File_Size(f_GenData_stream, &row_GenData, &col_GenData);// get rows and cols from file
    
    //LineData Size
    int row_LineData, col_LineData;
    FILE *f_LineData_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV_updated/LineData.csv", "r");
    File_Size(f_LineData_stream, &row_LineData, &col_LineData);// get rows and cols from file
    
    //LoadFactor Size
    int row_LoadFactor, col_LoadFactor;
    FILE *f_LoadFactor_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV_updated/LoadFactor.csv", "r");
    File_Size(f_LoadFactor_stream, &row_LoadFactor, &col_LoadFactor);// get rows and cols from file
    
    //LoadForecastData Size
    int row_LoadForecast, col_LoadForecast;
    FILE *f_LoadForecast_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV_updated/LoadForecastData.csv", "r");
    File_Size(f_LoadForecast_stream, &row_LoadForecast, &col_LoadForecast);// get rows and cols from file
    
    //ShiftFactorData Size
    int row_SFData, col_SFData;
    FILE *f_SFData_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV_updated/ShiftFactorData.csv", "r");
    File_Size(f_SFData_stream, &row_SFData, &col_SFData);// get rows and cols from file
    
    //SRCostandMaxSustainedRateData Size
    int row_SRandMSRData, col_SRandMSRData;
    FILE *f_SRandMSRData_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV_updated/SRCostandMaxSustainedRateData.csv", "r");
    File_Size(f_SRandMSRData_stream, &row_SRandMSRData, &col_SRandMSRData);// get rows and cols from file
    
    //SRSystemRequirementData Size
    int row_SRTData, col_SRTData;
    FILE *f_SRTData_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV_updated/SRSystemRequirementData.csv", "r");
    File_Size(f_SRTData_stream, &row_SRTData, &col_SRTData);// get rows and cols from file
    //printf("row=%d\t col=%d\t\n",row_GenData,col_GenData);
    
    /* **Read data according to the size of CSV files** */
    //GenCorrectiveDispatchLimitData Read
    double *GenCorrectiveDispatchLimitData;
    GenCorrectiveDispatchLimitData = calloc(row_GenCorrective*col_GenCorrective, sizeof(double));
    Data_Read( f_GenCorrective_stream, row_GenCorrective, col_GenCorrective, GenCorrectiveDispatchLimitData);
    
    //GenData Read
    double *GenData;
    GenData = calloc(row_GenData*col_GenData, sizeof(double));
    Data_Read( f_GenData_stream, row_GenData, col_GenData, GenData);

    //LineData Read
    double *LineData;
    LineData = calloc(row_LineData*col_LineData, sizeof(double));
    Data_Read( f_LineData_stream, row_LineData, col_LineData, LineData);
    
    //LoadFactor Read
    double *LoadFactor;
    LoadFactor = calloc(row_LoadFactor*col_LoadFactor, sizeof(double));
    Data_Read( f_LoadFactor_stream, row_LoadFactor, col_LoadFactor, LoadFactor);
    
    //LoadForecastData Read
    double *LoadForecastData;
    LoadForecastData = calloc(row_LoadForecast*col_LoadForecast, sizeof(double));
    Data_Read( f_LoadForecast_stream, row_LoadForecast, col_LoadForecast, LoadForecastData);

    //ShiftFactorData Read
    double *ShiftFactorData;
    ShiftFactorData = calloc(row_SFData*col_SFData, sizeof(double));
    Data_Read( f_SFData_stream, row_SFData, col_SFData, ShiftFactorData);
    
    //SRCostandMaxSustainedRateData Read
    double *SRCostandMaxSustainedRateData;
    SRCostandMaxSustainedRateData = calloc(row_SRandMSRData*col_SRandMSRData, sizeof(double));
    Data_Read( f_SRandMSRData_stream, row_SRandMSRData, col_SRandMSRData, SRCostandMaxSustainedRateData);
    
    //SRSystemRequirementData Read
    double *SRSystemRequirementData;
    SRSystemRequirementData = calloc(row_SRTData*col_SRTData, sizeof(double));
    Data_Read( f_SRTData_stream, row_SRTData, col_SRTData, SRSystemRequirementData);
    
    /* **Read data to structure** */
    
    //GenCorrectiveDispatchLimitData Read
    GenCorrectiveDispatchLimitData_Struct GenCorrectiveDispatchLimitData_Set;
    Data_StoreTo_Structure (&GenCorrectiveDispatchLimitData_Set.Gen_BusNum, GenCorrectiveDispatchLimitData, row_GenCorrective, col_GenCorrective) ; //It only accept the first element address of the structure
    
    //GenData Read
    GenData_Struct GenData_Set;
    Data_StoreTo_Structure (&GenData_Set.Gen_BusNum, GenData, row_GenData, col_GenData) ;//It only accept the first element address of the structure
    
    //LineData Read
    LineData_Struct LineData_Set;
    Data_StoreTo_Structure (&LineData_Set.Line_Num, LineData, row_LineData, col_LineData) ;//It only accept the first element address of the structure
    
    //LoadFactor Read
    LoadFactor_Struct LoadFactor_Set;
    Data_StoreTo_Structure (&LoadFactor_Set.LoadFactor_BusNum, LoadFactor, row_LoadFactor, col_LoadFactor) ;//It only accept the first element address of the structure
    
    //LoadForecastData Read
    LoadForecastData_Struct LoadForecastData_Set;
    Data_StoreTo_Structure (&LoadForecastData_Set.LoadForecast_CorrectionLoad, LoadForecastData, row_LoadForecast, col_LoadForecast) ;//It only accept the first element address of the structure
    
    //SRCostandMaxSustainedRateData Read
    SRCostandMaxSustainedRateData_Struct SRCostandMaxSustainedRateData_Set;
    Data_StoreTo_Structure (&SRCostandMaxSustainedRateData_Set.SR_MSR_BusNum, SRCostandMaxSustainedRateData, row_SRandMSRData, col_SRandMSRData) ;//It only accept the first element address of the structure
    
    //SRSystemRequirementData Read
    SRSystemRequirementData_Struct SRSystemRequirementData_Set;
    Data_StoreTo_Structure (&SRSystemRequirementData_Set.SRT_SpiningResreve, SRSystemRequirementData, row_SRTData, col_SRTData) ;//It only accept the first element address of the structure
    
    //SFData Read (The ShiftFactor use different kind of data structure. It is an array of pointer.)
    double *SFData_Set[row_SFData];
    for (int i = 0; i < row_SFData; i++) {
        SFData_Set[i] = &ShiftFactorData[i*col_SFData];
    }
    
    
//    
//    /* **Data read correctiveness testing** */
//    char *name1="test1";
//    char *name2="test2";
//    char *name3="test3";
//    char *name4="test4";
//    char *name5="test5";
//    char *name6="test6";
//    char *name7="test7";
//    char *name8="test8";
//    
//    Data_Read_Corrective_Test(row_GenCorrective, col_GenCorrective, GenCorrectiveDispatchLimitData, name1);
//    printf("row = %d, col = %d\n", row_GenCorrective, col_GenCorrective);
//    Data_Read_Corrective_Test(row_GenData, col_GenData, GenData, name2);
//    printf("row = %d, col = %d\n", row_GenData, col_GenData);
//    Data_Read_Corrective_Test(row_LineData, col_LineData, LineData, name3);
//    printf("row = %d, col = %d\n", row_LineData, col_LineData);
//    Data_Read_Corrective_Test(row_LoadFactor, col_LoadFactor, LoadFactor, name4);
//    printf("row = %d, col = %d\n", row_LoadFactor, col_LoadFactor);
//    
//    Data_Read_Corrective_Test(row_LoadForecast, col_LoadForecast, LoadForecastData, name5);
//    printf("row = %d, col = %d\n", row_LoadForecast, col_LoadForecast);
//    Data_Read_Corrective_Test(row_SFData, col_SFData, ShiftFactorData, name6);
//    printf("row = %d, col = %d\n", row_SFData, col_SFData);
//    Data_Read_Corrective_Test(row_SRandMSRData, col_SRandMSRData, SRCostandMaxSustainedRateData, name7);
//    printf("row = %d, col = %d\n", row_SRandMSRData, col_SRandMSRData);
//    Data_Read_Corrective_Test(row_SRTData, col_SRTData, SRSystemRequirementData, name8);
//    printf("row = %d, col = %d\n", row_SRTData, col_SRTData);
//    
//    
//    
//    /* **Print iteratively to check the arrays in the structures** */
//    for (int i = 0; i < col_GenCorrective; ++i){
//        printf("%f", GenCorrectiveDispatchLimitData_Set.Gen_CorrectiveDispatchLimit[i]);
//        printf("\n");
//    }
//    
    
    /* ***************************************************************************************************************************************** */
    
    
    /* **Define and set the control constants** */
    const int k_limit=1000;
    const int Convergence_threshold_Coefficient=0.2;
    const int Convergence_threshold_PG_GENbase=Convergence_threshold_Coefficient;
    const int Lambda_AllModule_Initial=0;
    const int C_AllModule_Initial=0.05;
    const int Beta_UpdateC=1.01;
    const int RelaxGENBase_capacity=0; //=0,not relax
    const int RelaxPowerBalance_base=0; //=0,not relax
    printf("k_limit = %d, Convergence_threshold_PG_GENbase = %d, Lambda_AllModule_Initial = %d, C_AllModule_Initial = %d, Beta_UpdateC = %d, RelaxGENBase_capacity = %d, RelaxPowerBalance_base = %d \n", k_limit, Convergence_threshold_PG_GENbase, Lambda_AllModule_Initial, C_AllModule_Initial, Beta_UpdateC, RelaxGENBase_capacity, RelaxPowerBalance_base);
    printf("\n");
    
    
    
    /* **Input generator and system paremeters** */
    
    
    
    
    
    
    /* **Define and value the common constants** */
    const int NT = col_LoadForecast, NG = col_GenData, NLd = col_LoadFactor, NB = col_SFData, NL = row_SFData;
    printf("NT = %d, NG = %d, NLd = %d, NB = %d, NL = %d \n", NT, NG, NLd, NB, NL);
    printf("\n");
    
    
    
    /* **Define and initialize the variables** */
    double PG_GENbase_k[NG*NT]; // gen to main (and) main to system ****************
    int I_GENbase_k[NG*NT];
    double PG_Systembase_k[NG*NT];// system to main (and) main to gen ****************
    
    for (int i = 0; i < NG*NT; ++i){
        PG_GENbase_k[i]=0;
        I_GENbase_k[i]=0;
        PG_Systembase_k[i]=0;
        
        //printf("%f", PG_GENbase_k[i]);// This I will change to print to file *****
        //printf("\n");// This I will change to print to file *****
    }
    
    
    
    /* **Define and initialize the iteratively changed parameters** */
    int k=0;
    int Covergence_judge_PG_GENbase=0;
    double Lambda_PG_GENbase_k[NG*NT]; //main to gen (and) to system ****************
    double C_PG_GENbase_k[NG*NT]; //main to gen (and) to system ****************
    double Difference_PG_GENbase_k[NG*NT];
    int Convergence_ElementMatrix_PG_GENbase_k[NG*NT];
    int Convergence_Percentage_PG_GENbase_k=0;
    printf("k = %d, Covergence_judge_PG_GENbase = %d, Convergence_Percentage_PG_GENbase_k = %d \n", k, Covergence_judge_PG_GENbase, Convergence_Percentage_PG_GENbase_k);
    
    
    for (int i = 0; i < NG*NT; ++i){
        Lambda_PG_GENbase_k[i]=Lambda_AllModule_Initial;
        C_PG_GENbase_k[i]=C_AllModule_Initial;
        Difference_PG_GENbase_k[i]=0;
        Convergence_ElementMatrix_PG_GENbase_k[i]=0;
        
        //printf("%f", Lambda_PG_GENbase_k[i]); // This I will change to print to file *****
        //printf("\n"); // This I will change to print to file *****
    }
    
    
    
    /* **Iterative calculation** */
    while (Covergence_judge_PG_GENbase == 0 && k < k_limit)
    {
        /* calculate the optimal resluts of GEN base module*/
        //        Module_GENbase(); //call function to calculate
        
        /* calculate the optimal resluts of System base module*/
        //        Module_Systembase(); //call function to calculate
        
        /* analyze the convergence of PG_GENbase*/
        
        
        
        /* if not convergence, update parameters*/
        if (Covergence_judge_PG_GENbase == 0){
            k=k+1;
            //        Update_Penalty(); //call function to update Lagrangian penalties
        }
        
    } //end of while loop
    
    /* ZHANG***************************************************************************************************************************************** */
    /* Process 0 is the "monitor". It chooses the inputs, and sends them to the workers. It waits for the outputs */
    int ierr_mpi;
    int id_mpi;
    int size_mpi;
    double wtime_mpi;
    
    /*ID and Tag: send to SLAVE NODES setting*/
    int id_mpi_gen_master = 0; // This is just for the example. We can talk about it to make it generalized. ******Gong******
    int tag_mpi_gen = 1; // This is just for the example. We can talk about it to make it generalized. ******Gong******
    int id_mpi_system_master = 0; // This is just for the example. We can talk about it to make it generalized. ******Gong******
    int tag_mpi_system = 2; // This is just for the example. We can talk about it to make it generalized. ******Gong******
    
    /*ID and Tag: send to MASTER NODES setting*/
    int id_mpi_gen_slave = 1; // This is just for the example. We can talk about it to make it generalized. ******Gong******
    int id_mpi_system_slave = 2; // This is just for the example. We can talk about it to make it generalized. ******Gong******
    

    
    ierr_mpi = MPI_Init ( &argc,  &argv );
    ierr_mpi = MPI_Comm_rank ( MPI_COMM_WORLD, &id_mpi );
    ierr_mpi = MPI_Comm_size ( MPI_COMM_WORLD, &size_mpi );
    
    /* Make sure we have enough processes */
    if ( size_mpi < 3 )
    {
        printf ( "\n" );
        printf ( "MPI_MULTITASK - Fatal error!\n" );
        printf ( "  Number of available processes must be at least 3!\n" );
        ierr_mpi = MPI_Finalize ( );
        exit ( 1 );
    }
    
    /*Run program P0 on process 0*/
    if (id_mpi == 0) {
        wtime_mpi = MPI_Wtime ( );//MPI_Wtime returns a floating-point number of seconds, representing elapsed wall-clock time since some time in the past.
        
        int nonstop_decision;
        int stop_counter;
        int end_point = 2; // This program will loop 3 times. It can be set to k_limit in your **Iterative calculation** section. ******Gong******
            
            for (stop_counter=0;  ; stop_counter++) // this loop will execute 3 times.
            {
                printf("the stop_counter is %d\n", stop_counter);
                p0_stop_decision( &nonstop_decision,  stop_counter, end_point); // You may put your **Iterative calculation** section into this function ******Gong******
                printf("stop decision is %d ", nonstop_decision);
                printf("\n");
                p0_send_decision( size_mpi, nonstop_decision);
                
                if (nonstop_decision == 0){ //stop_decision = 0 will end program; stop_decision = 1 means program will go on.
                    break;}
                
                
                // This will send Lambda to Generation module and System module
                p0_set_input ( Lambda_PG_GENbase_k, NG, NT ); // You may put your **Lambda and C update** section into this function ******Gong******
                p0_send_input ( Lambda_PG_GENbase_k, id_mpi_gen_slave, tag_mpi_gen, NG, NT );
                p0_send_input ( Lambda_PG_GENbase_k, id_mpi_system_slave, tag_mpi_system, NG, NT );
                
                // This will send C to Generation module and System module
                p0_set_input ( C_PG_GENbase_k, NG, NT ); // You may put your **Lambda and C update** section into this function ******Gong******
                p0_send_input ( C_PG_GENbase_k, id_mpi_gen_slave, tag_mpi_gen, NG, NT );
                p0_send_input ( C_PG_GENbase_k, id_mpi_system_slave, tag_mpi_system, NG, NT );
                
                // This will receive PG from Generation module
                p0_receive_output(PG_GENbase_k, id_mpi_gen_slave, tag_mpi_gen, NG, NT);
                p0_receive_output(PG_Systembase_k, id_mpi_system_slave, tag_mpi_system, NG, NT);
            }
        
        wtime_mpi = MPI_Wtime ( ) - wtime_mpi;
        printf ( "  Process 0 time = %g\n", wtime_mpi );
        ierr_mpi = MPI_Finalize(); //All processes must call this routine before exiting. All processes will still exist but may not make any further MPI calls.
    }
    
    
    
    else if (id_mpi == 1){
        
        
        //wtime = MPI_Wtime ( );
        double Lambda_PG_GENbase_k_slave[NG*NT]; // This variable is what I added to differcient from master node variable ******Gong******
        double C_PG_GENbase_k_slave[NG*NT]; // This variable is what I added to differcient from master node variable ******Gong******
        double PG_GENbase_k_slave[NG*NT]; // This variable is what I added to differcient from master node variable ******Gong******
        int nonstop_decision_i;
        
        
        while (1)
        {
            p1_receive_decision(&nonstop_decision_i, id_mpi_gen_master, tag_mpi_gen);
            printf("stop decision received at subproblem %d is %d \n", id_mpi_gen_master, nonstop_decision_i);
            
            
            if (nonstop_decision_i == 1)
            {
                
                wtime_mpi = MPI_Wtime ( );
                printf("the loop is no error at this moment at process %d\n", id_mpi_gen_master);
                
                p1_receive_input (Lambda_PG_GENbase_k_slave, id_mpi_gen_master, tag_mpi_gen, NG, NT);
                p1_receive_input (C_PG_GENbase_k_slave, id_mpi_gen_master, tag_mpi_gen, NG, NT);
                p1_compute_output(Lambda_PG_GENbase_k_slave, C_PG_GENbase_k_slave,nonstop_decision_i, NG, NT, PG_GENbase_k_slave); // This is the main calculation place. It is VERY IMPORTANT ******Gong******
                p1_send_output ( PG_GENbase_k_slave, id_mpi_gen_master, tag_mpi_gen, NG, NT );
                
                wtime_mpi = MPI_Wtime ( ) - wtime_mpi;
                
                printf ( "  Process %d time = %g\n",id_mpi, wtime_mpi );
            }
            
            else
            {
                printf("MPI communication will terminate \n");
                ierr_mpi = MPI_Finalize ( );//All processes must call this routine before exiting. All processes will still exist but may not make any further MPI calls.
                break;
            }
            
        }
        
        
    }
    else if (id_mpi == 2){
        
        
        //wtime = MPI_Wtime ( );
        double Lambda_PG_GENbase_k_slave[NG*NT]; // This variable is what I added to differcient from master node variable ******Gong******
        double C_PG_GENbase_k_slave[NG*NT]; // This variable is what I added to differcient from master node variable ******Gong******
        double PG_Systembase_k_slave[NG*NT]; // This variable is what I added to differcient from master node variable ******Gong******
        int nonstop_decision_i;
        
        
        while (1)
        {
            p1_receive_decision(&nonstop_decision_i, id_mpi_system_master, tag_mpi_system);
            printf("stop decision received at subproblem %d is %d \n", id_mpi_system_master, nonstop_decision_i);
            
            
            if (nonstop_decision_i == 1)
            {
                
                wtime_mpi = MPI_Wtime ( );
                printf("the loop is no error at this moment at process %d\n", id_mpi_system_master);
                
                p1_receive_input (Lambda_PG_GENbase_k_slave, id_mpi_system_master, tag_mpi_system, NG, NT);
                p1_receive_input (C_PG_GENbase_k_slave, id_mpi_system_master, tag_mpi_system, NG, NT);
                p1_compute_output(Lambda_PG_GENbase_k_slave, C_PG_GENbase_k_slave, nonstop_decision_i, NG, NT, PG_Systembase_k_slave); // This is the main calculation place. It is VERY IMPORTANT ******Gong******
                p1_send_output ( PG_Systembase_k_slave, id_mpi_system_master, tag_mpi_system, NG, NT );
                
                wtime_mpi = MPI_Wtime ( ) - wtime_mpi;
                
                printf ( "  Process %d time = %g\n",id_mpi, wtime_mpi );
            }
            
            else
            {
                printf("MPI communication will terminate \n");
                ierr_mpi = MPI_Finalize ( );//All processes must call this routine before exiting. All processes will still exist but may not make any further MPI calls.
                break;
            }
            
        }
        
        
    }
    /* ZHANG***************************************************************************************************************************************** */
    
    
    
    
    
    
    
    
    
    /* **Build and output optimal results** */
    
    
    
    /* **Calculate and output operation cost** */
    
    
    /* **Print the critical parameters** */
    printf("k = %d \n", k);
    /****************************************************************************************************************************************** */
    
    
    
    /*Free file pointers and data memory*/
    fclose(f_GenCorrective_stream);
    free(GenCorrectiveDispatchLimitData);
    fclose(f_GenData_stream);
    free(GenData);
    fclose(f_LineData_stream);
    free(LineData);
    fclose(f_LoadFactor_stream);
    free(LoadFactor);
    fclose(f_LoadForecast_stream);
    free(LoadForecastData);
    fclose(f_SFData_stream);
    free(ShiftFactorData);
    fclose(f_SRandMSRData_stream);
    free(SRCostandMaxSustainedRateData);
    fclose(f_SRTData_stream);
    free(SRSystemRequirementData);

    
    return 0;
}
