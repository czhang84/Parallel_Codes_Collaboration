//
//  declare_variable.h
//  Declare some variables used in the C_Codes: parallel computation of SCUC
//
//  Created by Dr.Fu's group on 12/16.
//  Copyright © 2016 Dr.Fu's group. All rights reserved.
//


#ifndef declare_variable_h
#define declare_variable_h

#include <stdio.h>

#define BUFFER_MAX 2048 //define a constant:BUFFER_MAX


/*Declare variables (constant) indicating rows and column of original CSV generator & system data*/
extern int row_GenCorrective, col_GenCorrective,  				//zhangupdat
    row_GenData, col_GenData, 
    row_LineData, col_LineData, 
    row_LoadFactor, col_LoadFactor, 
    row_LoadForecast, col_LoadForecast, 
    row_SFData, col_SFData, 
    row_SRandMSRData, col_SRandMSRData,
    row_SRTData, col_SRTData;

/*Declare pointers (constant) of arrays of sorts of parameters of generator & system*/
extern double *GenCorrectiveDispatchLimitData,					//zhangupdat
       *GenData,
       *LineData,
       *LoadFactor,
       *LoadForecastData,
       *ShiftFactorData,
       *SRCostandMaxSustainedRateData,
       *SRSystemRequirementData;

/*Declare structures (constant) of original CSV generator & system data*/
typedef struct GenCorrectiveDispatchLimitData{
    double *Gen_BusNum;
    double *Gen_Pmax;
    double *Gen_Pmin;
    double *Gen_RampUp;
    double *Gen_RampDown;
    double *Gen_CorrectiveDispatchLimit;
} GenCorrectiveDispatchLimitData_Struct;

    extern GenCorrectiveDispatchLimitData_Struct GenCorrectiveDispatchLimitData_Set; //declare a structure alias //zhangupdat

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

    extern GenData_Struct GenData_Set; 				//zhangupdat

typedef struct LineData{
    double *Line_Num;
    double *Line_FromBus;
    double *Line_ToBus;
    double *Line_Impedance;
    double *Line_PowerLimit;
    } LineData_Struct;

    extern LineData_Struct LineData_Set; 				//zhangupdat

typedef struct LoadFactor{												
    double *LoadFactor_BusNum;
    double *LoadFactor_LoadDistribution;
    double *LoadFactor_LoadFactor;
} LoadFactor_Struct;

    extern LoadFactor_Struct LoadFactor_Set; 				//zhangupdat

typedef struct LoadForecastData{
    double *LoadForecast_CorrectionLoad;
    double *LoadForecast_LoadIEEE;
    double *LoadForecast_LoadPJM;
} LoadForecastData_Struct;

    extern LoadForecastData_Struct LoadForecastData_Set; 				//zhangupdat


//SFData  is not formed as structure data.  										//Lin change!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


typedef struct SR_MSR_Data{
    double *SR_MSR_BusNum;
    double *SR_MSR_CostCoefficient;
    double *SR_MSR_MaxSustainedRate;
} SRCostandMaxSustainedRateData_Struct;

    extern SRCostandMaxSustainedRateData_Struct SRCostandMaxSustainedRateData_Set; 				//zhangupdat

typedef struct SRT_Data{
    double *SRT_SpiningResreve;
} SRSystemRequirementData_Struct;

    extern SRSystemRequirementData_Struct SRSystemRequirementData_Set; 				//zhangupdat


#endif /* declare_variable_h */

