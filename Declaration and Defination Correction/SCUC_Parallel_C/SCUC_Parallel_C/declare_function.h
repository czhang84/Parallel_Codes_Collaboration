//
//  declare_function.h
//  Declare all the functions used in the C_Codes: parallel computation of SCUC
//
//  Created by Dr.Fu's group on 12/16.
//  Copyright © 2016 Dr.Fu's group. All rights reserved.
//


#ifndef declare_function_h
#define declare_function_h

#include <stdio.h>


/*Raw_Data read from file related functions*/
int input_data();
int File_Size(FILE * fstream, int *row, int *col);
int Data_Read( FILE * fstream, int row, int col, double* info);
int Data_Read_Corrective_Test(int row, int col, double *testing_array, char*name);


/*Readed_Data store to structure functions*/
void Data_StoreTo_Structure (double ** struct_member_ptr, double * array_data_ptr,int row, int col) ;


/*Array (Matrix in Matlab) operation functions*/
void Array_initial_double(double *Array_initialition, double length);
void Array_initial_int(int *Array_initialition, double length);


/*Time related functions*/
void timestamp ( );


#endif /* declare_function_h */
