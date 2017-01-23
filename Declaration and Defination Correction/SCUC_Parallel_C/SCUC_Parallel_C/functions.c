//
//  functions.c
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
#include "declare_function.h"
//#include "declare_variable.h"


/*Initialize array of type Double (value each element of the array)*/
void Array_initial_double(double *Array_initialition, double length)
{
    int l = (int) length;
    for (int i = 0; i < l; i++) {
        Array_initialition[i] = 0;
    }
}


/*Initialize array of type Int (value each element of the array)*/
void Array_initial_int(int *Array_initialition, double length)
{
    int l = (int) length;
    for (int i = 0; i < l; i++) {
        Array_initialition[i] = (int)0;
    }
}


/* TIMESTAMP prints the current YMDHMS date as a time stamp */
void timestamp ( )
{
# define TIME_SIZE 40
    
    static char time_buffer[TIME_SIZE];//it exist through program execution. static storage duration, block scope, and no linkage. it's initialize just onece, when timestamp compiled. And set to 0 if not initialize it.
    const struct tm *tm; //value can't be modified by assignment or by incrementing or decrementing
    //the value of tm can be changed but tm points to a value that must remain constant
    time_t now;//type suitable for storing the calendar time
    
    now = time ( NULL );//calculate the current calender time and encodes it into time_t format
    tm = localtime ( &now );//the value of timer is broken up into the structure tm and expressed in the local time zone
    
    strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm );//Formats the time represented in the structure timeptr according to the formatting rules defined in format and stored into str.
    
    printf ( "%s\n", time_buffer );
# undef TIME_SIZE
}


