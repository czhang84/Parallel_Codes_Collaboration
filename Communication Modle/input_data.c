//
//  input_data.c
//  Parallel_C_Codes
//
//  Created by zhangcaihua on 11/21/16.
//  Copyright © 2016 zhangcaihua. All rights reserved.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "declare_function.h"
#include "declare_variable.h" //******** zhang ******* 1/16/2017
#include "define_macros.h"//******** zhang ******* 1/16/2017

/************************************************** Define Variable This should be done ONLY ONCE**************************************************/ //******** zhang ******* 1/16/2017
/*Define Structure*/
SRSystemRequirementData_Struct SRSystemRequirementData_Set; //******** zhang ******* 1/16/2017
SRCostandMaxSustainedRateData_Struct SRCostandMaxSustainedRateData_Set; //******** zhang ******* 1/16/2017
LoadForecastData_Struct LoadForecastData_Set; //******** zhang ******* 1/16/2017
LoadFactor_Struct LoadFactor_Set; //******** zhang ******* 1/16/2017
LineData_Struct LineData_Set; //******** zhang ******* 1/16/2017
GenData_Struct GenData_Set; //******** zhang ******* 1/16/2017
GenCorrectiveDispatchLimitData_Struct GenCorrectiveDispatchLimitData_Set; //******** zhang ******* 1/16/2017

/*Define variables (constant) indicating rows and column of original CSV generator & system data*/
int row_GenCorrective, col_GenCorrective, //******** zhang ******* 1/16/2017
row_GenData, col_GenData,
row_LineData, col_LineData,
row_LoadFactor, col_LoadFactor,
row_LoadForecast, col_LoadForecast,
row_SFData, col_SFData,
row_SRandMSRData, col_SRandMSRData,
row_SRTData, col_SRTData;

/*Define pointers (constant) of arrays of sorts of parameters of generator & system*/
double *GenCorrectiveDispatchLimitData, //******** zhang ******* 1/16/2017
*GenData,
*LineData,
*LoadFactor,
*LoadForecastData,
*ShiftFactorData,
*SRCostandMaxSustainedRateData,
*SRSystemRequirementData;


/**************************************************Function for reading data from CSV files**************************************************/
int input_data() 
{

    /* **Read size of CSV files ** */
    //GenCorrectiveDispatchLimitData Size
    FILE *f_GenCorrective_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV_updated/GenCorrectiveDispatchLimitData.csv", "r");
    File_Size(f_GenCorrective_stream, &row_GenCorrective, &col_GenCorrective);// get rows and cols from file
    
    //GenData Size
    FILE *f_GenData_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV_updated/GenData.csv", "r");
    File_Size(f_GenData_stream, &row_GenData, &col_GenData);// get rows and cols from file
    
    //LineData Size
    FILE *f_LineData_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV_updated/LineData.csv", "r");
    File_Size(f_LineData_stream, &row_LineData, &col_LineData);// get rows and cols from file
    
    //LoadFactor Size
    FILE *f_LoadFactor_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV_updated/LoadFactor.csv", "r");
    File_Size(f_LoadFactor_stream, &row_LoadFactor, &col_LoadFactor);// get rows and cols from file
    
    //LoadForecastData Size
    FILE *f_LoadForecast_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV_updated/LoadForecastData.csv", "r");
    File_Size(f_LoadForecast_stream, &row_LoadForecast, &col_LoadForecast);// get rows and cols from file
    
    //ShiftFactorData Size
    FILE *f_SFData_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV_updated/ShiftFactorData.csv", "r");
    File_Size(f_SFData_stream, &row_SFData, &col_SFData);// get rows and cols from file

    //SRCostandMaxSustainedRateData Size
    FILE *f_SRandMSRData_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV_updated/SRCostandMaxSustainedRateData.csv", "r");
    File_Size(f_SRandMSRData_stream, &row_SRandMSRData, &col_SRandMSRData);// get rows and cols from file
    
    //SRSystemRequirementData Size
    FILE *f_SRTData_stream = fopen("/Users/zhangcaihua/Desktop/Parallel_Codes_Collaboration/Data_CSV_updated/SRSystemRequirementData.csv", "r");
    File_Size(f_SRTData_stream, &row_SRTData, &col_SRTData);// get rows and cols from file
    //printf("row=%d\t col=%d\t\n",row_GenData,col_GenData);
    

    /* **Read data according to the size of CSV files** */
    //GenCorrectiveDispatchLimitData Read
    GenCorrectiveDispatchLimitData = calloc(row_GenCorrective*col_GenCorrective, sizeof(double));
    Data_Read( f_GenCorrective_stream, row_GenCorrective, col_GenCorrective, GenCorrectiveDispatchLimitData);
    
    //GenData Read
    GenData = calloc(row_GenData*col_GenData, sizeof(double));
    Data_Read( f_GenData_stream, row_GenData, col_GenData, GenData);

    //LineData Read
    LineData = calloc(row_LineData*col_LineData, sizeof(double));
    Data_Read( f_LineData_stream, row_LineData, col_LineData, LineData);
    
    //LoadFactor Read
    LoadFactor = calloc(row_LoadFactor*col_LoadFactor, sizeof(double));
    Data_Read( f_LoadFactor_stream, row_LoadFactor, col_LoadFactor, LoadFactor);
    
    //LoadForecastData Read
    LoadForecastData = calloc(row_LoadForecast*col_LoadForecast, sizeof(double));
    Data_Read( f_LoadForecast_stream, row_LoadForecast, col_LoadForecast, LoadForecastData);

    //ShiftFactorData Read
    ShiftFactorData = calloc(row_SFData*col_SFData, sizeof(double));
    Data_Read( f_SFData_stream, row_SFData, col_SFData, ShiftFactorData);
    
    //SRCostandMaxSustainedRateData Read
    SRCostandMaxSustainedRateData = calloc(row_SRandMSRData*col_SRandMSRData, sizeof(double));
    Data_Read( f_SRandMSRData_stream, row_SRandMSRData, col_SRandMSRData, SRCostandMaxSustainedRateData);
    
    //SRSystemRequirementData Read
    SRSystemRequirementData = calloc(row_SRTData*col_SRTData, sizeof(double));
    Data_Read( f_SRTData_stream, row_SRTData, col_SRTData, SRSystemRequirementData);
    

    /* **Read data to structure** */
    //GenCorrectiveDispatchLimitData Read
    Data_StoreTo_Structure (&GenCorrectiveDispatchLimitData_Set.Gen_BusNum, GenCorrectiveDispatchLimitData, row_GenCorrective, col_GenCorrective) ; //It only accept the first element address of the structure
    
    //GenData Read
    Data_StoreTo_Structure (&GenData_Set.Gen_BusNum, GenData, row_GenData, col_GenData) ;//It only accept the first element address of the structure
    
    //LineData Read
    Data_StoreTo_Structure (&LineData_Set.Line_Num, LineData, row_LineData, col_LineData) ;//It only accept the first element address of the structure
    
    //LoadFactor Read
    Data_StoreTo_Structure (&LoadFactor_Set.LoadFactor_BusNum, LoadFactor, row_LoadFactor, col_LoadFactor) ;//It only accept the first element address of the structure
    
    //LoadForecastData Read
    Data_StoreTo_Structure (&LoadForecastData_Set.LoadForecast_CorrectionLoad, LoadForecastData, row_LoadForecast, col_LoadForecast) ;//It only accept the first element address of the structure
    
    //SRCostandMaxSustainedRateData Read
    Data_StoreTo_Structure (&SRCostandMaxSustainedRateData_Set.SR_MSR_BusNum, SRCostandMaxSustainedRateData, row_SRandMSRData, col_SRandMSRData) ;//It only accept the first element address of the structure
    
    //SRSystemRequirementData Read
    Data_StoreTo_Structure (&SRSystemRequirementData_Set.SRT_SpiningResreve, SRSystemRequirementData, row_SRTData, col_SRTData) ;//It only accept the first element address of the structure
    
    //SFData  is not formed as structure data.  										//Lin change!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~The following are just for testing~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/    
//    /* **Data read correctiveness testing** */
//    char *name1="test1";
//    char *name2="test2";
//    char *name3="test3";
//    char *name4="test4";
//    char *name5="test5";
//    char *name6="test6";
//    char *name7="test7";
//    char *name8="test8";   
//    Data_Read_Corrective_Test(row_GenCorrective, col_GenCorrective, GenCorrectiveDispatchLimitData, name1);
//    printf("row = %d, col = %d\n", row_GenCorrective, col_GenCorrective);
//    Data_Read_Corrective_Test(row_GenData, col_GenData, GenData, name2);
//    printf("row = %d, col = %d\n", row_GenData, col_GenData);
//    Data_Read_Corrective_Test(row_LineData, col_LineData, LineData, name3);
//    printf("row = %d, col = %d\n", row_LineData, col_LineData);
//    Data_Read_Corrective_Test(row_LoadFactor, col_LoadFactor, LoadFactor, name4);
//    Data_Read_Corrective_Test(row_LoadForecast, col_LoadForecast, LoadForecastData, name5);
//    printf("row = %d, col = %d\n", row_LoadForecast, col_LoadForecast);
//    Data_Read_Corrective_Test(row_SFData, col_SFData, ShiftFactorData, name6);
//    printf("row = %d, col = %d\n", row_SFData, col_SFData);
//    Data_Read_Corrective_Test(row_SRandMSRData, col_SRandMSRData, SRCostandMaxSustainedRateData, name7);
//    printf("row = %d, col = %d\n", row_SRandMSRData, col_SRandMSRData);
//    Data_Read_Corrective_Test(row_SRTData, col_SRTData, SRSystemRequirementData, name8);
//    printf("row = %d, col = %d\n", row_SRTData, col_SRTData);
   
//    printf("In Func: row_GenData = %d, col_GenData = %d, row_LineData = %d, col_LineData = %d, row_LoadFactor = %d, col_LoadFactor = %d, row_LoadForecast = %d, col_LoadForecast = %d, row_SFData = %d, //col_SFData = %d, row_SRandMSRData = %d, col_SRandMSRData = %d, row_SRTData = %d, col_SRTData = %d \n", row_GenData, col_GenData, row_LineData, col_LineData, row_LoadFactor, col_LoadFactor, //row_LoadForecast, col_LoadForecast, row_SFData, col_SFData, row_SRandMSRData, col_SRandMSRData, row_SRTData, col_SRTData);

//    printf("In Func:Address of GenCorrectiveDispatchLimitData = %p \n", GenCorrectiveDispatchLimitData); 
/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~The above are just for testing~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/    


    /* **Free file pointers and data memory** */
//    fclose(f_GenCorrective_stream);
//    free(GenCorrectiveDispatchLimitData);
//    fclose(f_GenData_stream);
//    free(GenData);
//    fclose(f_LineData_stream);
//    free(LineData);
//    fclose(f_LoadFactor_stream);
//    free(LoadFactor);
//    fclose(f_LoadForecast_stream);
//    free(LoadForecastData);
//    fclose(f_SFData_stream);
//    free(ShiftFactorData);
//    fclose(f_SRandMSRData_stream);
//    free(SRCostandMaxSustainedRateData);
//    fclose(f_SRTData_stream);
//    free(SRSystemRequirementData);

   
    return 0;

}



/**************************************************The following are sub-functions called by the above function**************************************************/

/* **CSV file size
    This will not count the label row at the top of CSV file
** */
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


/* **Data read function from CSV file** */
// First, the reading sequence is row by row. So, when output data, please use the same order.
// Second, all data come from CSV file are converted to type double.
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

/* **Data read function from CSV file. But this time will read int type data** */
// First, the reading sequence is row by row. So, when output data, please use the same order.
// Second, all data come from CSV file are converted to type Int.
int Data_Read_Int( FILE * fstream, int row, int col, int* info) // **********zhang********** //16/1/2017
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
            info[j + i*col] = atoi(record);
            ++j;
            record = strtok(NULL, ",");
        }
        ++i;
    }
    
    printf("\n");
    rewind(fstream_inside);
    return 0;
}

/* **Structure Read** */
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


/* **Data read correctiveness testing** */
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



