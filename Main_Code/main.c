//
//  main.c
//  Parallel_C_Codes
//
//  Created by zhangcaihua on 11/21/16.
//  Copyright © 2016 zhangcaihua. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "parallel_functions.h"


int main(int argc, const char * argv[]) {
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
    
    
    
    /* **Data read correctiveness testing** */
    char *name1="test1";
    char *name2="test2";
    char *name3="test3";
    char *name4="test4";
    char *name5="test5";
    char *name6="test6";
    char *name7="test7";
    char *name8="test8";
    
    Data_Read_Corrective_Test(row_GenCorrective, col_GenCorrective, GenCorrectiveDispatchLimitData, name1);
    printf("row = %d, col = %d\n", row_GenCorrective, col_GenCorrective);
    Data_Read_Corrective_Test(row_GenData, col_GenData, GenData, name2);
    printf("row = %d, col = %d\n", row_GenData, col_GenData);
    Data_Read_Corrective_Test(row_LineData, col_LineData, LineData, name3);
    printf("row = %d, col = %d\n", row_LineData, col_LineData);
    Data_Read_Corrective_Test(row_LoadFactor, col_LoadFactor, LoadFactor, name4);
    printf("row = %d, col = %d\n", row_LoadFactor, col_LoadFactor);
    
    Data_Read_Corrective_Test(row_LoadForecast, col_LoadForecast, LoadForecastData, name5);
    printf("row = %d, col = %d\n", row_LoadForecast, col_LoadForecast);
    Data_Read_Corrective_Test(row_SFData, col_SFData, ShiftFactorData, name6);
    printf("row = %d, col = %d\n", row_SFData, col_SFData);
    Data_Read_Corrective_Test(row_SRandMSRData, col_SRandMSRData, SRCostandMaxSustainedRateData, name7);
    printf("row = %d, col = %d\n", row_SRandMSRData, col_SRandMSRData);
    Data_Read_Corrective_Test(row_SRTData, col_SRTData, SRSystemRequirementData, name8);
    printf("row = %d, col = %d\n", row_SRTData, col_SRTData);
    
    
    
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
