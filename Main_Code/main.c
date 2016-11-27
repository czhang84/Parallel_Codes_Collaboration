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
    //GenCorrectiveDispatchLimitData_54Unit Size
    int row_GenCorrective, col_GenCorrective;
    FILE *f_GenCorrective_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV/GenCorrectiveDispatchLimitData_54Unit.csv", "r");
    file_size(f_GenCorrective_stream, &row_GenCorrective, &col_GenCorrective);// get rows and cols from file
    
    //GenData_54Unit_IEEE118 Size
    int row_GenData, col_GenData;
    FILE *f_GenData_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV/GenData_54Unit_IEEE118.csv", "r");
    file_size(f_GenData_stream, &row_GenData, &col_GenData);// get rows and cols from file
    
    //LineData_186Branch_IEEE118 Size
    int row_LineData, col_LineData;
    FILE *f_LineData_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV/LineData_186Branch_IEEE118.csv", "r");
    file_size(f_LineData_stream, &row_LineData, &col_LineData);// get rows and cols from file
    
    //LoadFactor_91Side_IEEE118 Size
    int row_LoadFactor, col_LoadFactor;
    FILE *f_LoadFactor_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV/LoadFactor_91Side_IEEE118.csv", "r");
    file_size(f_LoadFactor_stream, &row_LoadFactor, &col_LoadFactor);// get rows and cols from file
    
    //LoadForecastData_24Hour_IEEE118 Size
    int row_LoadForecast, col_LoadForecast;
    FILE *f_LoadForecast_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV/LoadForecastData_24Hour_IEEE118.csv", "r");
    file_size(f_LoadForecast_stream, &row_LoadForecast, &col_LoadForecast);// get rows and cols from file
    
    //SFData_186Branch118Bus_IEEE118 Size
    int row_SFData, col_SFData;
    FILE *f_SFData_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV/SFData_186Branch118Bus_IEEE118.csv", "r");
    file_size(f_SFData_stream, &row_SFData, &col_SFData);// get rows and cols from file
    
    //SRandMSRData_54Unit_forIEEE118 Size
    int row_SRandMSRData, col_SRandMSRData;
    FILE *f_SRandMSRData_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV/SRandMSRData_54Unit_forIEEE118.csv", "r");
    file_size(f_SRandMSRData_stream, &row_SRandMSRData, &col_SRandMSRData);// get rows and cols from file
    
    //SRTData_24Hour_forIEEE118 Size
    int row_SRTData, col_SRTData;
    FILE *f_SRTData_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV/SRTData_24Hour_forIEEE118.csv", "r");
    file_size(f_SRTData_stream, &row_SRTData, &col_SRTData);// get rows and cols from file
    //printf("row=%d\t col=%d\t\n",row_GenData,col_GenData);
    
    /* **Read data according to the size of CSV files** */
    //GenCorrectiveDispatchLimitData_54Unit Read
    double *GenCorrectiveDispatchLimitData_54Unit_data;
    GenCorrectiveDispatchLimitData_54Unit_data = calloc(row_GenCorrective*col_GenCorrective, sizeof(double));
    Data_Read( f_GenCorrective_stream, row_GenCorrective, col_GenCorrective, GenCorrectiveDispatchLimitData_54Unit_data);
    
    //GenData_54Unit_IEEE118 Read
    double *GenData_54Unit_IEEE118_data;
    GenData_54Unit_IEEE118_data = calloc(row_GenData*col_GenData, sizeof(double));
    Data_Read( f_GenData_stream, row_GenData, col_GenData, GenData_54Unit_IEEE118_data);

    //LineData_186Branch_IEEE118 Read
    double *LineData_186Branch_IEEE118_data;
    LineData_186Branch_IEEE118_data = calloc(row_LineData*col_LineData, sizeof(double));
    Data_Read( f_LineData_stream, row_LineData, col_LineData, LineData_186Branch_IEEE118_data);
    
    //LoadFactor_91Side_IEEE118 Read
    double *LoadFactor_91Side_IEEE118_data;
    LoadFactor_91Side_IEEE118_data = calloc(row_LoadFactor*col_LoadFactor, sizeof(double));
    Data_Read( f_LoadFactor_stream, row_LoadFactor, col_LoadFactor, LoadFactor_91Side_IEEE118_data);
    
    //LoadForecastData_24Hour_IEEE118 Read
    double *LoadForecastData_24Hour_IEEE118_data;
    LoadForecastData_24Hour_IEEE118_data = calloc(row_LoadForecast*col_LoadForecast, sizeof(double));
    Data_Read( f_LoadForecast_stream, row_LoadForecast, col_LoadForecast, LoadForecastData_24Hour_IEEE118_data);

    //SFData_186Branch118Bus_IEEE118 Read
    double *SFData_186Branch118Bus_IEEE118_data;
    SFData_186Branch118Bus_IEEE118_data = calloc(row_SFData*col_SFData, sizeof(double));
    Data_Read( f_SFData_stream, row_SFData, col_SFData, SFData_186Branch118Bus_IEEE118_data);
    
    //SRandMSRData_54Unit_forIEEE118 Read
    double *SRandMSRData_54Unit_forIEEE118_data;
    SRandMSRData_54Unit_forIEEE118_data = calloc(row_SRandMSRData*col_SRandMSRData, sizeof(double));
    Data_Read( f_SRandMSRData_stream, row_SRandMSRData, col_SRandMSRData, SRandMSRData_54Unit_forIEEE118_data);
    
    //SRTData_24Hour_forIEEE118 Read
    double *SRTData_24Hour_forIEEE118_data;
    SRTData_24Hour_forIEEE118_data = calloc(row_SRTData*col_SRTData, sizeof(double));
    Data_Read( f_SRTData_stream, row_SRTData, col_SRTData, SRTData_24Hour_forIEEE118_data);
    
    
    /* **Data read correctiveness testing** */
    char *name1="test1";
    char *name2="test2";
    char *name3="test3";
    char *name4="test4";
    char *name5="test5";
    char *name6="test6";
    char *name7="test7";
    char *name8="test8";
    
    Data_Read_Corrective_Test(row_GenCorrective, col_GenCorrective, GenCorrectiveDispatchLimitData_54Unit_data, name1);
    printf("row = %d, col = %d\n", row_GenCorrective, col_GenCorrective);
    Data_Read_Corrective_Test(row_GenData, col_GenData, GenData_54Unit_IEEE118_data, name2);
    printf("row = %d, col = %d\n", row_GenData, col_GenData);
    Data_Read_Corrective_Test(row_LineData, col_LineData, LineData_186Branch_IEEE118_data, name3);
    printf("row = %d, col = %d\n", row_LineData, col_LineData);
    Data_Read_Corrective_Test(row_LoadFactor, col_LoadFactor, LoadFactor_91Side_IEEE118_data, name4);
    printf("row = %d, col = %d\n", row_LoadFactor, col_LoadFactor);
    
    Data_Read_Corrective_Test(row_LoadForecast, col_LoadForecast, LoadForecastData_24Hour_IEEE118_data, name5);
    printf("row = %d, col = %d\n", row_LoadForecast, col_LoadForecast);
    Data_Read_Corrective_Test(row_SFData, col_SFData, SFData_186Branch118Bus_IEEE118_data, name6);
    printf("row = %d, col = %d\n", row_SFData, col_SFData);
    Data_Read_Corrective_Test(row_SRandMSRData, col_SRandMSRData, SRandMSRData_54Unit_forIEEE118_data, name7);
    printf("row = %d, col = %d\n", row_SRandMSRData, col_SRandMSRData);
    Data_Read_Corrective_Test(row_SRTData, col_SRTData, SRTData_24Hour_forIEEE118_data, name8);
    printf("row = %d, col = %d\n", row_SRTData, col_SRTData);
    
    /*Free file pointers and data memory*/
    fclose(f_GenCorrective_stream);
    free(GenCorrectiveDispatchLimitData_54Unit_data);
    fclose(f_GenData_stream);
    free(GenData_54Unit_IEEE118_data);
    fclose(f_LineData_stream);
    free(LineData_186Branch_IEEE118_data);
    fclose(f_LoadFactor_stream);
    free(LoadFactor_91Side_IEEE118_data);
    
    
    fclose(f_LoadForecast_stream);
    free(LoadForecastData_24Hour_IEEE118_data);
    fclose(f_SFData_stream);
    free(SFData_186Branch118Bus_IEEE118_data);
    fclose(f_SRandMSRData_stream);
    free(SRandMSRData_54Unit_forIEEE118_data);
    fclose(f_SRTData_stream);
    free(SRTData_24Hour_forIEEE118_data);

    
    
    return 0;
}
