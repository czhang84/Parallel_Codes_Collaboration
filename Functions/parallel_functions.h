//
//  parallel_functions.h
//  Parallel_C_Codes
//
//  Created by zhangcaihua on 11/21/16.
//  Copyright © 2016 zhangcaihua. All rights reserved.
//

#ifndef parallel_functions_h
#define parallel_functions_h

#include <stdio.h>
#define BUFFER_MAX 2048
#define SIZE 186



/*Structure Definication*/
typedef struct GenCorrectiveDispatchLimitData{
    double *Gen_BusNum;
    double *Gen_Pmax;
    double *Gen_Pmin;
    double *Gen_RampUp;
    double *Gen_RampDown;
    double *Gen_CorrectiveDispatchLimit;
} GenCorrectiveDispatchLimitData_Struct;

typedef struct GenData{
    double *Gen_BusNum;
    double *Gen_PriceA;
    double *Gen_PriceB;
    double *Gen_PriceC;
    double *Gen_Pmax;
    double *Gen_Pmin;
    double *Gen_InitialOn;
    double *Gen_MinOn;
    double *Gen_MinOff;
    double *Gen_RampUp;
    double *Gen_RampDown;
    double *Gen_StartUp;
    double *Gen_ShutDown;
    double *Gen_FuelPrice;
} GenData_Struct;

typedef struct LineData{
    double *Line_Num;
    double *Line_FromBus;
    double *Line_ToBus;
    double *Line_Impedance;
    double *Line_PowerLimit;
    } LineData_Struct;

typedef struct LoadFactiorData{
    double *LoadFactor_BusNum;
    double *LoadFactor_LoadDistribution;
    double *LoadFactor_LoadFactor;
} LoadFactor_Struct;

typedef struct LoadForecastData{
    double *LoadForecast_CorrectionLoad;
    double *LoadForecast_LoadIEEE;
    double *LoadForecast_LoadPJM;
} LoadForecastData_Struct;

typedef struct ShiftFactorData{
    double *SF;
} SFData_Struct;

typedef struct SR_MSR_Data{
    double *SR_MSR_BusNum;
    double *SR_MSR_CostCoefficient;
    double *SR_MSR_MaxSustainedRate;
} SRCostandMaxSustainedRateData_Struct;

typedef struct SRT_Data{
    double *SRT_SpiningResreve;
} SRSystemRequirementData_Struct;


/*Function Prototypes*/

// **Raw_Data read from file related functions**
int File_Size(FILE * fstream, int *row, int *col);
int Data_Read( FILE * fstream, int row, int col, double* info);
int Data_Read_Corrective_Test(int row, int col, double *testing_array, char*name);

// **Readed_Data store to structure functions**
void Data_StoreTo_Structure (double ** struct_member_ptr, double * array_data_ptr,int row, int col) ;

// **Array (Matrix in Matlab) operation functions**
void Array_initial_double(double *Array_initialition, double length);
void Array_initial_int(int *Array_initialition, double length);

// **Time related functions**
void timestamp ( );

// **MPI related functions**
// *Master node functions*
void p0_stop_decision(int *stop_decision, int stop_counter, int end_point);
void p0_send_decision(int process_size,int stop_decision);
void p0_set_input ( double *input, int row, int col );
void p0_send_input ( double *input, int receiving_process_ID, int receiving_process_tag, int row, int col );
void p0_receive_output ( double *output, int sending_process_ID, int sending_process_tag, int row, int col );

// *Slave node functions*
void p1_receive_decision(int *stop_decision_i, int sending_process_ID, int sending_process_tag);
void p1_receive_input (double *input_i, int sending_process_ID, int sending_process_tag, int row, int col);
//void p1_compute_output (double *input_i, int stop_decision_i, int row, int col, double* result);
void p1_compute_output (double *input_1, double* input_2, int stop_decision_i, int row, int col, double* result);
void p1_send_output ( double *output_i, int receiving_process_ID, int receiving_process_tag, int row, int col );
#endif /* parallel_functions_h */
