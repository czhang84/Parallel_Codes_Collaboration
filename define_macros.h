//
//  define_macros.h
//  SCUC_Parallel_C
//
//  Created by zhangcaihua on 1/16/17.
//  Copyright © 2017 zhangcaihua. All rights reserved.
//

#ifndef define_macros_h
#define define_macros_h

#define BUFFER_MAX 2048 //define a constant:BUFFER_MAX ???????????????????????????????????????????? This is due to the read data should first stroe at an array. This is the array size.//******** zhang ******* 1/17/2017


#define TASK_LENGTH 15 //******** zhang ******* 1/17/2017
//According to discussion, each task can be represented by 15 integers.
//The first 5 integers indicate task will run on which node. Example: 00001 will run on Node 1.
//The second 5 integers is devided into two parts.
//  First 3 integers indicate scenario, and second 2 integers indicate module.Example: 000 00 indicate base case which define as 000, and generation module which define as 00.
//The last 5 integer indicate task number. Example: 00000 shows that it is the first task of module.

#define MAX_NODE_NUMBER 99999 // This define the maximum numbers of node problem can use
#define MAX_NODE_DIGIT 5 // This define the maximum digits to represent a node

#define MAX_SCENARIO_NUMBER 999 // This define the maximum scenarios problem can have
#define MAX_SCENARIO_DIGIT 3 // This define the maximum digits to represent a scenario

#define MAX_MODULE_NUMBER 99 // This define the maximum module problem can have
#define MAX_MODULE_DIGIT 2 // This define the maximum digits to represent a module

#define MAX_TASK_NUMBER 99999 // This define the maximum task problem can have
#define MAX_TASK_DIGIT 5 // This define the maximum digits to represent a task

#endif /* define_macros_h */
