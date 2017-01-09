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


int main(int argc,  char * argv[]) {


    /*********************************************************************Common work for all nodes START,executing only 1 time**********************************************************************/    
    /*********************************************************************Common work for all nodes START,executing only 1 time**********************************************************************/    
    /*********************************************************************Common work for all nodes START,executing only 1 time**********************************************************************/    
    printf ("*************************************************************************************************************Common work (1st) START\n");

    /* ***Input unit and system paremeters*** */
    //call function to input unit & system data
    input_data();
    /*Note:
      by calling the function: input_data(), the following variables and arrays can be directly used:
      I. The quantities of rows & columns of originally input Excel data:
         (1)row_GenCorrective, col_GenCorrective; (2)row_GenData, col_GenData; (3)row_LineData, col_LineData; (4)row_LoadFactor, col_LoadFactor; 
         (5)row_LoadForecast, col_LoadForecast; (6)row_SFData, col_SFData; (7)row_SRandMSRData, col_SRandMSRData; (8)row_SRTData, col_SRTData;
      II.Each array of each originally input Excel data (in structure format): 
         GenCorrectiveDispatchLimitData_Set.Gen_CorrectiveDispatchLimit; //Array:[1 * NG]
    	 GenData_Set.Gen_BusNum; //Array:[1 * NG]
    	 GenData_Set.Gen_PriceA; //Array:[1 * NG]
    	 GenData_Set.Gen_PriceB; //Array:[1 * NG]
    	 GenData_Set.Gen_PriceC; //Array:[1 * NG]
    	 GenData_Set.Gen_Pmax;   //Array:[1 * NG]
    	 GenData_Set.Gen_Pmin;   //Array:[1 * NG]
    	 GenData_Set.Gen_InitialOn; //Array:[1 * NG]
    	 GenData_Set.Gen_MinOn; //Array:[1 * NG]
    	 GenData_Set.Gen_MinOff; //Array:[1 * NG]
    	 GenData_Set.Gen_RampUp; //Array:[1 * NG]
    	 GenData_Set.Gen_RampDown; //Array:[1 * NG]
    	 GenData_Set.Gen_StartUp;  //Array:[1 * NG]
    	 GenData_Set.Gen_ShutDown;  //Array:[1 * NG]
    	 GenData_Set.Gen_FuelPrice; //Array:[1 * NG]
    	 LineData_Set.Line_Num;  //Array:[1 * NL]
    	 LineData_Set.Line_FromBus; //Array:[1 * NL]
    	 LineData_Set.Line_ToBus; //Array:[1 * NL]
    	 LineData_Set.Line_Impedance; //Array:[1 * NL]
    	 LineData_Set.Line_PowerLimit; //Array:[1 * NL]
    	 LoadFactor_Set.LoadFactor_BusNum; //Array:[1 * NLd]
    	 LoadFactor_Set.LoadFactor_LoadDistribution; //Array:[1 * NLd]
    	 LoadFactor_Set.LoadFactor_LoadFactor; //Array:[1 * NLd]
    	 LoadForecastData_Set.LoadForecast_CorrectionLoad; //Array:[1 * NT]
    	 LoadForecastData_Set.LoadForecast_LoadIEEE; //Array:[1 * NT]
    	 LoadForecastData_Set.LoadForecast_LoadPJM; //Array:[1 * NT]
    	 SFData_Struct.SF;                                                                         					//SF structure is special???????????????????????????????????
    	 SRCostandMaxSustainedRateData_Set.SR_MSR_BusNum; //Array:[1 * NG]
    	 SRCostandMaxSustainedRateData_Set.SR_MSR_CostCoefficient; //Array:[1 * NG]
    	 SRCostandMaxSustainedRateData_Set.SR_MSR_MaxSustainedRate; //Array:[1 * NG]
    	 SRSystemRequirementData_Set.SRT_SpiningResreve; //Array:[1 * NT]
         Each element's value of each array can be read by the following way:
         for (int i = 0; i < 10; ++i)
	 {
	 printf("In Main: %f", SRCostandMaxSustainedRateData_Set.SR_MSR_MaxSustainedRate[i]);
	 printf("\n");
	 }
    End Note*/
					      
    
    /* ***Define and value the common constants*** */
    const int NLd = col_LoadFactor, NB = col_SFData, NL = row_SFData, NT = col_LoadForecast, NG = col_GenData; 				//lin test!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//col_LoadForecast


    /* ***Calculate basic varibales*** */
    //get generator's initial status
    int I_GENbase_Initial[NG];
    for (int i = 0; i < NG; ++i)
    {
        I_GENbase_Initial[i] = GenData_Set.Gen_InitialOn[i] >= 1;
    }


    /* ***Define MPI related variables*** */
    /* Process 0 is the "master/monitor". It chooses the inputs, and sends them to the workers. It waits for the outputs */
    int ierr_mpi;
    int id_mpi;
    int size_mpi;
    double wtime_mpi;
    /*ID and Tag: send to SLAVE NODES setting*/
    int id_mpi_gen_master = 0; 
    int tag_mpi_gen = 1; 
    int id_mpi_system_master = 0; 
    int tag_mpi_system = 2; 
    /*ID and Tag: send to MASTER NODES setting*/
    int id_mpi_gen_slave = 1; 
    int id_mpi_system_slave = 2;


    /* ***get the MPI environment information*** */
    ierr_mpi = MPI_Init(&argc, &argv);
    ierr_mpi = MPI_Comm_rank(MPI_COMM_WORLD, &id_mpi);
    ierr_mpi = MPI_Comm_size(MPI_COMM_WORLD, &size_mpi);
    

    /* ***Make sure we have enough processes*** */
    if ( size_mpi < 3 ) {
        printf ( "\n" );
        printf ( "MPI_MULTITASK - Fatal error!\n" );
        printf ( "  Number of available processes must be at least 3!\n" );
        ierr_mpi = MPI_Finalize ( );
        exit ( 1 );
    } 

    printf ("*************************************************************************************************************Common work (1st) END\n");
    /**********************************************************************Common work for all nodes END,executing only 1 time***********************************************************************/      
    /**********************************************************************Common work for all nodes END,executing only 1 time***********************************************************************/      
    /**********************************************************************Common work for all nodes END,executing only 1 time***********************************************************************/      



    /************************************************************************************Master node's work START*************************************************************************************/
    /************************************************************************************Master node's work START*************************************************************************************/
    /************************************************************************************Master node's work START*************************************************************************************/
    if (id_mpi == 0) 
    {

        /* ***time the total simulation time of the master node** */  
        wtime_mpi = MPI_Wtime ( );//MPI_Wtime returns a floating-point number of seconds, representing elapsed wall-clock time since some time in the past.


	/* ***Define the control constants*** */
    	const int    k_limit = 1000; 													//lin test***********************************************
    	const double Convergence_threshold_Coefficient = 0.2;
    	const double Convergence_threshold_PG_GENbase=Convergence_threshold_Coefficient;
    	const double Lambda_AllModule_Initial = 0;
    	const double C_AllModule_Initial = 0.05;
    	const double Beta_UpdateC = 1.01;
    	const int    RelaxGENBase_capacity = 0; //=0,not relax
    	const int    RelaxPowerBalance_base = 0; //=0,not relax
        
    
    	/* ***Define and initialize the variables*** */
    	double PG_GENbase_k[NG*NT];    //send from GEN module node to main node & main node to Syst module node
    	int    I_GENbase_k[NG*NT];
    	double SUD_GENbase_k[NG*NT];
    	double PG_Systembase_k[NG*NT]; //send from Syst module node to main node & main node to GEN module node
    
    	for (int i = 0; i < NG * NT; ++i){
            PG_GENbase_k[i] = 0;
            I_GENbase_k[i] = 0;
            SUD_GENbase_k[i] = 0;
            PG_Systembase_k[i] = 0;         
        }
    
    
    	/* ***Define and initialize the iteratively changed parameters*** */
    	int    k = 1;
    	int    Covergence_judge_PG_GENbase = 0;
    	double Lambda_PG_GENbase_k[NG*NT]; //in every iteration, send from the main node to the GEN module node
    	double C_PG_GENbase_k[NG*NT]; //in every iteration, send from the main node to the Syst module node
    	double Difference_PG_GENbase_k[NG*NT];
    	int    Convergence_ElementMatrix_PG_GENbase_k[NG*NT];
    	double Convergence_Percentage_PG_GENbase_k = 0;
               
    
    	for (int i = 0; i < NG * NT; ++i){
            Lambda_PG_GENbase_k[i] = Lambda_AllModule_Initial;
            C_PG_GENbase_k[i] = C_AllModule_Initial;
            Difference_PG_GENbase_k[i] = 0;
            Convergence_ElementMatrix_PG_GENbase_k[i] = 0; 
    	}
    

        /* ***define MPI related variables*** */  
        int stop_decision;


    	/****************************************Iterative calculation start****************************************/
    	/****************************************Iterative calculation start****************************************/
        while (Covergence_judge_PG_GENbase == 0 && k <= k_limit) //=0, not converge, so should continue
        {

            printf ("*************************************************************************************************************Master (K=%d) START\n",k);

    	    /* **master node send information to slave nodes** */
            //set & send stop decision (convergence judgement) to all slave nodes
            stop_decision = Covergence_judge_PG_GENbase;
            MasterNode_send_stopdecision( size_mpi, stop_decision);
            //send information to GEN module
            MasterNode_sendto_SlaveNode(Lambda_PG_GENbase_k, id_mpi_gen_slave, tag_mpi_gen, NG, NT);
            MasterNode_sendto_SlaveNode(C_PG_GENbase_k, id_mpi_gen_slave, tag_mpi_gen, NG, NT);
            MasterNode_sendto_SlaveNode(PG_Systembase_k, id_mpi_gen_slave, tag_mpi_gen, NG, NT);                  				
            //send information to Syst module
            MasterNode_sendto_SlaveNode(Lambda_PG_GENbase_k, id_mpi_system_slave, tag_mpi_system, NG, NT);
            MasterNode_sendto_SlaveNode(C_PG_GENbase_k, id_mpi_system_slave, tag_mpi_system, NG, NT);
            MasterNode_sendto_SlaveNode(PG_GENbase_k, id_mpi_system_slave, tag_mpi_system, NG, NT);               			
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/                    
                			for (int i = 0; i < NG*NT; ++i){
            	    	     		    printf("master send to slaves in %d: Lambda_PG_GENbase_k[%d]= %f",k, i,Lambda_PG_GENbase_k[i]); 
            	                	    printf("\n");
            	    	     		    printf("master send to slaves in %d: C_PG_GENbase_k[%d]= %f",k, i,C_PG_GENbase_k[i]); 
            	                	    printf("\n");
            	    	     		    printf("master send to slave 02 in %d: PG_GENbase_k[%d]= %f",k, i,PG_GENbase_k[i]); 
            	                	    printf("\n");
            	    	     		    printf("master send to slave 01 in %d: PG_Systembase_k[%d]= %f",k, i,PG_Systembase_k[i]); 
            	                	    printf("\n");
    	        			}
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    	    /* **master node recieve information from slave nodes** */
            //receive PG from GEN module
            MasterNode_receivefrom_SlaveNode(PG_GENbase_k, id_mpi_gen_slave, tag_mpi_gen, NG, NT);
            //receive PG from Syst module
            MasterNode_receivefrom_SlaveNode(PG_Systembase_k, id_mpi_system_slave, tag_mpi_system, NG, NT);
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/                    
                			for (int i = 0; i < NG*NT; ++i){
            	    	     		    printf("master receive from slave 01 in %d: PG_GENbase_k[%d]= %f",k, i,PG_GENbase_k[i]); 
            	                	    printf("\n");
            	    	     		    printf("master receive from slave 02 in %d: PG_Systembase_k[%d]= %f",k, i,PG_Systembase_k[i]); 
            	                	    printf("\n");
    	        			}
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					//double power_balance_check[NT];
                			//for (int t = 0; t < NT; ++t){
            	    	     		//    power_balance_check[t]=0;
    	        			//}  
                			//for (int t = 0; t < NT; ++t){
                			//    for (int i = 0; i < NG; ++i){
					//	power_balance_check[t]=power_balance_check[t]+PG_Systembase_k[i * NT + t];
    	        			//    }
            	    	     		//    printf("check power balance limit in %d: sum of PG_Systembase_k in hour %d= %f \n",k, t,power_balance_check[t]); 
					//}
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        				//get I_GENbase_k from PG_GENbase_k
					for (int i = 0; i < NG * NT; ++i)
            				{
                			    I_GENbase_k[i] = PG_GENbase_k[i] > 0.01;
    	    				}
                			for (int i = 0; i < NG*NT; ++i){
            	    	     		    printf("master receive from slave 01 in %d: I_GENbase_k[%d]= %d",k, i,I_GENbase_k[i]); 
            	                	    printf("\n");
    	        			}
        				double Total_OperationCost_GENbase = 0;
					for (int i = 0; i < NG; ++i)
        				{
	    				    for (int j = 0; j < NT; ++j)
            				    {
						Total_OperationCost_GENbase = Total_OperationCost_GENbase + GenData_Set.Gen_PriceA[i] * I_GENbase_k[i * NT + j] + GenData_Set.Gen_PriceB[i] * PG_GENbase_k[i 											* NT + j] + GenData_Set.Gen_PriceC[i] * PG_GENbase_k[i * NT + j] * PG_GENbase_k[i * NT + j];
            				    }
        				}
    					printf("Total generator operation cost in base case is %f \n", Total_OperationCost_GENbase);
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

 
            /* **analyze the convergence of PG_GENbase** */                                                     				// ************************************ Lin  to sub-func
	    //define a variable to store total no. of converged elem, in a new iteration, it should return to 0
	    int TotalQuantity_ConvergedPG_GENbase_k = 0;
            //cal diff betw elem, compare with threshold, cal total No. of converged elem
	    for (int i = 0; i < NG * NT; ++i)
            {
                Difference_PG_GENbase_k[i] = PG_GENbase_k[i] - PG_Systembase_k[i]; //calc the diff betw elements of 2 arrays
		Convergence_ElementMatrix_PG_GENbase_k[i] = abs(Difference_PG_GENbase_k[i]) <= (Convergence_threshold_PG_GENbase + 0.0000001); //compare the diff with threshold
                TotalQuantity_ConvergedPG_GENbase_k = TotalQuantity_ConvergedPG_GENbase_k + Convergence_ElementMatrix_PG_GENbase_k[i]; //cal total quantity of converged elements
    	    }
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/                    
                			for (int i = 0; i < NG*NT; ++i){
            	    	     		    printf("master in %d: Difference_PG_GENbase_k[%d]= %f",k, i,Difference_PG_GENbase_k[i]); 
            	                	    printf("\n");
            	    	     		    printf("master in %d: Convergence_ElementMatrix_PG_GENbase_k[%d]= %d", k, i,Convergence_ElementMatrix_PG_GENbase_k[i]); 
            	                	    printf("\n");
    	        			}
					printf("master in %d: TotalQuantity_ConvergedPG_GENbase_k = %d",k, TotalQuantity_ConvergedPG_GENbase_k); 
            				printf("\n");
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            //cal convergence percentage    
            Convergence_Percentage_PG_GENbase_k=TotalQuantity_ConvergedPG_GENbase_k * 100 / (NG * NT);


            /* **get the convergence judge variable** */                                                                		// ************************************ Lin change to sub-func
	    if (Convergence_Percentage_PG_GENbase_k == 100)
	    {
	        Covergence_judge_PG_GENbase = 1;
            }
            if (Convergence_Percentage_PG_GENbase_k != 100)
            {
                Covergence_judge_PG_GENbase = 0;
            }

	    printf("Convergence percentage in %d is %f (percent)\n", k, Convergence_Percentage_PG_GENbase_k);
    	    printf ("*************************************************************************************************************Master (K=%d) END\n",k); 


            /* **if not converge, update parameters** */                                                               			// ************************************ Lin change to sub-func
            if (Covergence_judge_PG_GENbase == 0) //if not converge
	    {
	        k=k+1;
                //Update_Penalty(); //call function to update Lagrangian penalties ?????????????????????lin later update??????????????????????????
	    	for (int i = 0; i < NG * NT; ++i)
            	{
                    Lambda_PG_GENbase_k[i] = Lambda_PG_GENbase_k[i] + C_PG_GENbase_k[i] * Difference_PG_GENbase_k[i]; //update Lambda
                    C_PG_GENbase_k[i] = C_PG_GENbase_k[i] * Beta_UpdateC; //update C
    	    	}
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/                    
            				//for (int i_printf = 0; i_printf < NG*NT; ++i_printf){
            				//    printf("Lambda_PG_GENbase_k %f", Lambda_PG_GENbase_k[i_printf]); 
            				//     printf("\n");
            				//     printf("C_PG_GENbase_k %f", C_PG_GENbase_k[i_printf]); 
            				//     printf("\n");
    	        			//}
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/ 
            }


            /* **if converge, jumpe out of the "while: loop and terminate** */ 
	    if (Covergence_judge_PG_GENbase == 1 || k > k_limit )
	    {
                break; 
            }               

	} //end of while loop
	/****************************************Iterative calculation end****************************************/
    	/****************************************Iterative calculation end****************************************/

        

    	/* ***Build and output optimal results*** */
        //get I_GENbase_k from PG_GENbase_k
	for (int i = 0; i < NG * NT; ++i)
            {
                I_GENbase_k[i] = PG_GENbase_k[i] > 0.01;
    	    }
        //get SUD_GENbase_k from I_GENbase_k
	for (int i = 0; i < NG; ++i) //for the 1st hour
            {
		if (I_GENbase_k[i * NT] - I_GENbase_Initial[i] == 1) //if startup
   		{
                   SUD_GENbase_k[i * NT] = GenData_Set.Gen_StartUp[i];
		}
		if (I_GENbase_k[i * NT] - I_GENbase_Initial[i] == -1) //if shutdown
   		{
                   SUD_GENbase_k[i * NT] = GenData_Set.Gen_ShutDown[i];
		}
    	    }        
	for (int i = 0; i < NG; ++i) //for the hours after the 1st hour
        {
	    for (int j = 0; j < NT; ++j)
            {
		if ( (I_GENbase_k[i * NT + (j + 1)] - I_GENbase_k[i * NT + j]) == 1) //if startup
   		{
                   SUD_GENbase_k[i * NT + (j + 1)] = GenData_Set.Gen_StartUp[i];
		}
		if ( (I_GENbase_k[i * NT + (j + 1)] - I_GENbase_k[i * NT + j]) == -1) //if shutdown
   		{
                   SUD_GENbase_k[i * NT + (j + 1)] = GenData_Set.Gen_ShutDown[i];
		} 
            }
        }   


        /* ***calculate total operation cost of all generators in all the studied time in base case*** */
        double Total_OperationCost_GENbase = 0;      
	for (int i = 0; i < NG; ++i)
        {
	    for (int j = 0; j < NT; ++j)
            {
		Total_OperationCost_GENbase = Total_OperationCost_GENbase + GenData_Set.Gen_PriceA[i] * I_GENbase_k[i * NT + j] + GenData_Set.Gen_PriceB[i] * PG_GENbase_k[i * NT + j]  					      + GenData_Set.Gen_PriceC[i] * PG_GENbase_k[i * NT + j] * PG_GENbase_k[i * NT + j] + SUD_GENbase_k[i * NT + j];
            }
        }


    	/* ***Print key parameters*** */
    	printf("Iteration Number is %d \n", k);
    	printf("Convergence percentage is %f (percent\n", Convergence_Percentage_PG_GENbase_k);
    	printf("Total generator operation cost in base case is %f \n", Total_OperationCost_GENbase);


    	/* ***Time the total simulation time of master node*** */
        wtime_mpi = MPI_Wtime ( ) - wtime_mpi;
        printf ( "Master node's total simulation time = %g\n", wtime_mpi );


    	/* ***finalize MPI*** */
        ierr_mpi = MPI_Finalize(); //All processors must call this routine before exiting. All processors will still exist but may not make any further MPI calls.

    } //END:if (id_mpi == 0)
    /************************************************************************************Master node's work END*************************************************************************************/
    /************************************************************************************Master node's work END*************************************************************************************/    
    /************************************************************************************Master node's work END*************************************************************************************/


    
    /*************************************************************************Slave-node-01's work in parallel manner START*************************************************************************/
    /*************************************************************************Slave-node-01's work in parallel manner START*************************************************************************/
    /*************************************************************************Slave-node-01's work in parallel manner START*************************************************************************/
    if (id_mpi == 1)
    { 

        //the following codes are only run once in a whole simulation process***********************************************************************************************
        /* ***time the simulation time of the slave node*** */
        wtime_mpi = MPI_Wtime( );


	/* ***define (constant) paremeter of variable quantity in certain module*** */
	//variable structure: [PG, I, SUD] *NT
	const int GenVarOneHourOneUnit_base = 3,
	  	  GenVarOneUnit_base = GenVarOneHourOneUnit_base * NT;


	/* ***define (constant) paremeter of total quantity of quadratic objective terms in a module*** */
	const int GenQuadraticTermOneUnit_base = NT;


	/* ***define (constant) offset of each variable in its smallest module*** */
	const int OffsetPG_base = 0, //should start from 0
	  	  OffsetI_base = 1,
	  	  OffsetSUD_base = 2;


    	/* ***Define and initialize variables resulted from this slave load*** */
    	double PG_GENbase_k_slave[NG*NT];
    	double I_GENbase_k_slave[NG*NT];
    	double SUD_GENbase_k_slave[NG*NT];
    	for (int i = 0; i < NG*NT; ++i)
	{
            PG_GENbase_k_slave[i]=0;
            I_GENbase_k_slave[i]=0; 
            SUD_GENbase_k_slave[i]=0;           
        }


        /* ***define arrays to receive information from the master node*** */
        double Lambda_PG_GENbase_k_slave[NG * NT],
               C_PG_GENbase_k_slave[NG * NT],      
               PG_Systembase_k_slave[NG * NT];    
        int    stop_decision_i;


	/* ***set GUROBI environment and model*** */
	/* *define GUROBI environment parameter* */
  	static GRBenv   *env   = NULL;
  	static GRBmodel *model = NULL;
	/* *define GUROBI parameter* */
	//variable structure: [PG, I, SUD] *NT
  	int       error = 0;                		//store error inf
  	double    sol[GenVarOneUnit_base];   		//store optimization solutions
  	int       ind[GenVarOneUnit_base];		//store the indexes of variables in each constraint
  	double    val[GenVarOneUnit_base];		//store the coefficients of variables in each constraint
  	char      vtype[GenVarOneUnit_base];		//store the types of all the variables
  	int       qrow[GenQuadraticTermOneUnit_base];	//store the indexes of variables in the "Row" of all the quadratic terms of the objective function
  	int       qcol[GenQuadraticTermOneUnit_base];	//store the indexes of variables in the "Column" of all the quadratic terms of the objective function
  	double    qval[GenQuadraticTermOneUnit_base];	//store the coefficients of all the quadratic terms of the objective function
  	double    objval;				//store the final optimal result of the objective function
  	int       optimstatus;				//store the optimization information


	/* *create GUROBI environment* */
  	error = GRBloadenv(&env, "GEN_Module.log");
  	if (error) goto QUIT_01;		    


        /* ***define variable to count iterative number*** */
        int k_slave_01=1; //count the iterative number of slave node
        //the above codes are only run once in a whole simulation process**********************************************************************************************
        

        /* ***loop (run multiple times) to reveive information, calculate optimal result, send information*** */ 
        while (1)
        {

            SlaveNode_receive_stopdecision(&stop_decision_i, id_mpi_gen_master, tag_mpi_gen); //receive convergence information to judge whether to continue
            //printf("stop decision received at slave node %d is %d \n", id_mpi_gen_master, stop_decision_i);
            
            if (stop_decision_i == 0) //=0, don't stop & go on (because not converge)
            {
    		printf ("*************************************************************************************************************Slave-01 (k=%d) START\n",k_slave_01);


                /* **time the simulation time of current iteration** */
                //wtime_mpi = MPI_Wtime ( ); //start to time
                //printf("the loop is no error at this moment at process %d\n", id_mpi_gen_master);
                

                /* **receive information from the master node** */
                SlaveNode_receivefrom_MasterNode(Lambda_PG_GENbase_k_slave, id_mpi_gen_master, tag_mpi_gen, NG, NT);
                SlaveNode_receivefrom_MasterNode(C_PG_GENbase_k_slave, id_mpi_gen_master, tag_mpi_gen, NG, NT);
                SlaveNode_receivefrom_MasterNode(PG_Systembase_k_slave, id_mpi_gen_master, tag_mpi_gen, NG, NT); 
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/                    
                			//for (int i = 0; i < NG*NT; ++i){
            	    			//     printf("slave 01 get from master, PG_Systembase_k_slave %f", PG_Systembase_k_slave[i]); 
            	        		//     printf("\n"); 
    	        			//}
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/  


                /* **calculate optiaml results by calling GUROBI** */
        	//Module_GENbase(); //call function to calculate

		for (int i = 0; i < NG; ++i) //repeatedly calculate optimal result of each unit
		{

  		    /* *Create an empty GUROBI model* */
  		    error = GRBnewmodel(env, &model, "GEN_Module", 0, NULL, NULL, NULL, NULL, NULL);
  		    if (error) goto QUIT_01;


  		    /* *Add GUROBI variables* */
  		    error = GRBaddvars(model, GenVarOneUnit_base, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  		    if (error) goto QUIT_01;


		    /* *build objective function for 1 generator in NT hours* */
		    //add quadratic objective terms
		    for (int t = 0; t < GenQuadraticTermOneUnit_base; ++t)
		    {
                        qrow[t] = GenVarOneHourOneUnit_base * t;
                        qcol[t] = GenVarOneHourOneUnit_base * t;
                        qval[t] = C_PG_GENbase_k_slave[i * NT + t] + GenData_Set.Gen_PriceC[i];
        	    }
  		    error = GRBaddqpterms(model, GenQuadraticTermOneUnit_base, qrow, qcol, qval);
  		    if (error) goto QUIT_01;
  		    //add linear objective terms
		    for (int t = 0; t < NT; ++t)
		    {
  		    	error = GRBsetdblattrelement(model, GRB_DBL_ATTR_OBJ, GenVarOneHourOneUnit_base * t + OffsetPG_base, 
                                                     GenData_Set.Gen_PriceB[i] + Lambda_PG_GENbase_k_slave[i * NT + t] 
                                                     - C_PG_GENbase_k_slave[i * NT + t] * (PG_GENbase_k_slave[i * NT + t] + PG_Systembase_k_slave[i * NT + t]));
  		    	if (error) goto QUIT_01;
  		    	error = GRBsetdblattrelement(model, GRB_DBL_ATTR_OBJ, GenVarOneHourOneUnit_base * t + OffsetI_base, GenData_Set.Gen_PriceA[i]);
  		    	if (error) goto QUIT_01;
  		    	error = GRBsetdblattrelement(model, GRB_DBL_ATTR_OBJ, GenVarOneHourOneUnit_base * t + OffsetSUD_base, 1);
  		    	if (error) goto QUIT_01;
        	    }


		    /* *build constraints for 1 generator in NT hours* */
		    //generator capacity limit
		    int Quantity_Nonzero_Coeff = 2;
		    for (int t = 0; t < NT; ++t)
		    {
                    	//upper bound limit
                        ind[0] = GenVarOneHourOneUnit_base * t + OffsetPG_base;
                        val[0] = 1;
                        ind[1] = GenVarOneHourOneUnit_base * t + OffsetI_base;
                        val[1] = (-1) * GenData_Set.Gen_Pmax[i];
  		    	error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, 0, NULL);
  		    	if (error) goto QUIT_01;
			//lower bound limit
                        ind[0] = GenVarOneHourOneUnit_base * t + OffsetPG_base;
                        val[0] = -1;
                        ind[1] = GenVarOneHourOneUnit_base * t + OffsetI_base;
                        val[1] = GenData_Set.Gen_Pmin[i];
  		        error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, 0, NULL);
  		        if (error) goto QUIT_01;
		    }


  		    /* *define variable types* */
    		    for (int t = 0; t < NT; ++t)
		    {
            		vtype[GenVarOneHourOneUnit_base * t + OffsetPG_base] = GRB_CONTINUOUS;
            		vtype[GenVarOneHourOneUnit_base * t + OffsetI_base] = GRB_BINARY; 
            		vtype[GenVarOneHourOneUnit_base * t + OffsetSUD_base] = GRB_CONTINUOUS;           
        	    }
  		    error = GRBsetcharattrarray(model, GRB_CHAR_ATTR_VTYPE, 0, GenVarOneUnit_base, vtype);
  		    if (error) goto QUIT_01;


		    /* *update model* */
		    error = GRBupdatemodel(model);
  		    if (error) goto QUIT_01;


		    /* *call GUROBI solver to conduct optimization calculation* */
  		    error = GRBoptimize(model);
  		    if (error) goto QUIT_01;


		    /* *write model to 'GEN_Module.lp/.sol'* */
  		    error = GRBwrite(model, "GEN_Module.lp");
  		    error = GRBwrite(model, "GEN_Module.sol");
  		    if (error) goto QUIT_01;


		    /* capture solution information */
  		    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
  		    if (error) goto QUIT_01;
  		    error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
  		    if (error) goto QUIT_01;
  		    error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, GenVarOneUnit_base, sol);
  		    if (error) goto QUIT_01;

  		    if (optimstatus == GRB_OPTIMAL) 
		    {
  		    	printf("Optimization is completed & feasible\n");
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/ 
					printf("Optimal objective of the current generator: %.4e\n", objval);                                       
            				//printf("\n");                    
            				//for (int i_printf = 0; i_printf < GenVarOneUnit_base; ++i_printf){
            				//     printf("In slave 01, [PG, I, SUD]= %.4f", sol[i_printf]); 
            				//     printf("\n"); 
    	        			//}
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  		    } 
		    else if (optimstatus == GRB_INF_OR_UNBD) 
		    {
    		        printf("Model is infeasible or unbounded\n");
  		    } 
		    else 
  		    {
    		        printf("Optimization was stopped early\n");
  		    }


		    /* *sotre optimization solution of each generator* */
    		    for (int t = 0; t < NT; ++t)
		    {
            		PG_GENbase_k_slave[i * NT + t] = sol[GenVarOneHourOneUnit_base * t + OffsetPG_base];
            		I_GENbase_k_slave[i * NT + t] = sol[GenVarOneHourOneUnit_base * t + OffsetI_base]; 
            		SUD_GENbase_k_slave[i * NT + t] = sol[GenVarOneHourOneUnit_base * t + OffsetSUD_base];           
        	    }


  		    /* *Free GUROBI model* */
  		    GRBfreemodel(model);

	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/                    
            				printf("slave 01 in %d iteration for generator %d*******************************************************************************",k_slave_01, i); 
            				printf("\n"); 
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

                 }//END:for(int i = 0; i < NG; ++i)


                /* **send information to the master node** */
                SlaveNode_sendto_MasterNode(PG_GENbase_k_slave, id_mpi_gen_master, tag_mpi_gen, NG, NT);


                /* **time the simulation time of current iteration** */
                //wtime_mpi = MPI_Wtime ( ) - wtime_mpi;
                //printf ( "  Slave node %d's simulation time in the iteration %d = %g\n",id_mpi, wtime_mpi, k_slave_01);


                /* **update the counter** */
                k_slave_01++;//count the iterative number of slave 01


    		printf ("*************************************************************************************************************Slave-01 (k=%d) END\n",k_slave_01-1); 
            }//END:(stop_decision_i == 0)
            
            else //if converged,just terminate
            {
                //printf("MPI communication will terminate, because the convergence is down.\n");
                ierr_mpi = MPI_Finalize ( );//All processes must call this routine before exiting. All processes will still exist but may not make any further MPI calls.
                break; //if converged, jumpe out of the "while: loop and terminate
            }//END:else

        }//END:while(1)


  	/* ***if GUROBI error, just stop and report error*** */
	QUIT_01:
	/* Error reporting */
	if (error) 
	{
  	    printf("ERROR: %s\n", GRBgeterrormsg(env));
  	    exit(1);
	}


  	/* *Free GUROBI environment* */
  	GRBfreeenv(env);


        //the following codes are only run once in a whole simulation process***********************************************************************************************
    	/* ***Time the total simulation time of slave node*** */
        wtime_mpi = MPI_Wtime ( ) - wtime_mpi;
        printf ( "Slave %d's total simulation time = %g\n",id_mpi, wtime_mpi );

    }//END:else if (id_mpi == 1)
    /*************************************************************************Slave-node-01's work in parallel manner END*************************************************************************/
    /*************************************************************************Slave-node-01's work in parallel manner END*************************************************************************/
    /*************************************************************************Slave-node-01's work in parallel manner END*************************************************************************/




    /************************************************************************Slave-node-02's work in parallel manner START************************************************************************/
    /************************************************************************Slave-node-02's work in parallel manner START************************************************************************/
    /************************************************************************Slave-node-02's work in parallel manner START************************************************************************/
    if (id_mpi == 2)
    {
        //the following codes are only run once in a whole simulation process***********************************************************************************************
        /* ***time the simulation time of the slave node** */
        wtime_mpi = MPI_Wtime ( );


	/* ***define (constant) paremeter of variable quantity in certain module*** */
	//variable structure: PG1,PG2,...,PGng
	const int SystemVarOneHour_GENbase = NG;


	/* ***define (constant) paremeter of total quantity of quadratic objective terms in a module*** */
	const int SystemQuadraticTermOneHour_GENbase = NG;


	/* ***define (constant) offset of each variable in its smallest module*** */
	//const int OffsetPG_Systembase = 0, //should start from 0


    	/* ***Define and initialize variables resulted from this slave load*** */
    	double PG_Systembase_k_slave[NG*NT];
    	double I_Systembase_k_slave[NG*NT];
    	for (int i = 0; i < NG*NT; ++i)
        {
            PG_Systembase_k_slave[i]=0;
            I_Systembase_k_slave[i]=0;           
        }


        /* ***define arrays to receive information from the master node*** */
        double Lambda_PG_GENbase_k_slave[NG*NT], 
               C_PG_GENbase_k_slave[NG*NT], 
               PG_GENbase_k_slave[NG*NT];  
        int stop_decision_i;


	/* ***set GUROBI environment and model*** */
	/* *define GUROBI environment parameter* */
  	static GRBenv   *env   = NULL;
  	static GRBmodel *model = NULL;
	/* *define GUROBI parameter* */
	//variable structure: [PG, I, SUD] *NT
  	int       error = 0;                			//store error inf
  	double    sol[SystemVarOneHour_GENbase];   		//store optimization solutions
  	int       ind[SystemVarOneHour_GENbase];		//store the indexes of variables in each constraint
  	double    val[SystemVarOneHour_GENbase];		//store the coefficients of variables in each constraint
  	char      vtype[SystemVarOneHour_GENbase];		//store the types of all the variables
  	int       qrow[SystemQuadraticTermOneHour_GENbase];	//store the indexes of variables in the "Row" of all the quadratic terms of the objective function
  	int       qcol[SystemQuadraticTermOneHour_GENbase];	//store the indexes of variables in the "Column" of all the quadratic terms of the objective function
  	double    qval[SystemQuadraticTermOneHour_GENbase];	//store the coefficients of all the quadratic terms of the objective function
  	double    objval;					//store the final optimal result of the objective function
  	int       optimstatus;					//store the optimization information


	/* ***create GUROBI environment*** */
  	error = GRBloadenv(&env, "System_Module.log");
  	if (error) goto QUIT_02;


        /* ***define variable to count iterative number*** */
        int k_slave_02=1; //count the iterative number of slave node
        //the above codes are only run once in a whole simulation process***********************************************************************************************        


        /* ***loop (run multiple times) to reveive information, calculate optimal result, send information*** */
        while (1)
        {

            SlaveNode_receive_stopdecision(&stop_decision_i, id_mpi_system_master, tag_mpi_system);
            //printf("stop decision received at subproblem %d is %d \n", id_mpi_system_master, stop_decision_i);
            
            if (stop_decision_i == 0) //=0, don't stop & go on (because not converge)
            {
    		printf ("*************************************************************************************************************Slave-02 (k=%d) START\n",k_slave_02); 
 
                /* **time the simulation time of current iteration** */
                //wtime_mpi = MPI_Wtime ( );
                //printf("the loop is no error at this moment at process %d\n", id_mpi_system_master);
                
                /* **receive information from the master node** */
                SlaveNode_receivefrom_MasterNode(Lambda_PG_GENbase_k_slave, id_mpi_system_master, tag_mpi_system, NG, NT);
                SlaveNode_receivefrom_MasterNode(C_PG_GENbase_k_slave, id_mpi_system_master, tag_mpi_system, NG, NT);
                SlaveNode_receivefrom_MasterNode(PG_GENbase_k_slave, id_mpi_system_master, tag_mpi_system, NG, NT);

              	/* **calculate optiaml results by calling GUROBI** */
        	//Module_Systembase(); //call function to calculate
		for (int t = 0; t < NT; ++t) //repeatedly calculate optimal result of each hour
		{


  		    /* *Create an empty GUROBI model* */
  		    error = GRBnewmodel(env, &model, "System_Module", 0, NULL, NULL, NULL, NULL, NULL);
  		    if (error) goto QUIT_02;


  		    /* *Add GUROBI variables* */
  		    error = GRBaddvars(model, SystemVarOneHour_GENbase, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  		    if (error) goto QUIT_02;


		    /* *build objective function for 1 hour involving multiple units* */
		    //add quadratic objective terms
		    for (int i = 0; i < SystemQuadraticTermOneHour_GENbase; ++i)
		    {
                        qrow[i] = i;
                        qcol[i] = i;
                        qval[i] = C_PG_GENbase_k_slave[i * NT + t];
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            	    			    printf("slave 02 in %d for hour %d: qval[%d]=%f",k_slave_02, t, i, qval[i]); 
            	        		    printf("\n"); 
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        	    }
  		    error = GRBaddqpterms(model, SystemQuadraticTermOneHour_GENbase, qrow, qcol, qval);
  		    if (error) goto QUIT_02;
  		    //add linear objective terms
		    for (int i = 0; i < SystemVarOneHour_GENbase; ++i)
		    {
  		    	error = GRBsetdblattrelement(model, GRB_DBL_ATTR_OBJ, i, (-1) * (Lambda_PG_GENbase_k_slave[i * NT + t] 
                                                     + C_PG_GENbase_k_slave[i * NT + t] * (PG_GENbase_k_slave[i * NT + t] + PG_Systembase_k_slave[i * NT + t])));
  		    	if (error) goto QUIT_02;
        	    }


		    /* *build constraints for 1 hour involving multiple units* */
		    //power balance limit
		    int Quantity_Nonzero_Coeff = NG;
		    for (int i = 0; i < Quantity_Nonzero_Coeff; ++i)
		    {
                        ind[i] = i;
                        val[i] = 1;
		    }
  		    error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_EQUAL, LoadForecastData_Set.LoadForecast_CorrectionLoad[t] * 0.85, "power_balance_limit"); 
  											     //Lin Note: Load*0.85 to make peak hourly load around 6000MW !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  		    if (error) goto QUIT_02;


  		    /* *define variable types* */
    		    for (int i = 0; i < NG; ++i)
		    {
            		vtype[i] = GRB_CONTINUOUS;         
        	    }
  		    error = GRBsetcharattrarray(model, GRB_CHAR_ATTR_VTYPE, 0, SystemVarOneHour_GENbase, vtype);
  		    if (error) goto QUIT_02;


		    /* *update model* */
		    error = GRBupdatemodel(model);
  		    if (error) goto QUIT_02;


		    /* *call GUROBI solver to conduct optimization calculation* */
  		    error = GRBoptimize(model);
  		    if (error) goto QUIT_02;


		    /* *write model to 'System_Module.lp'* */
  		    error = GRBwrite(model, "System_Module.lp");
  		    error = GRBwrite(model, "System_Module.sol");
  		    if (error) goto QUIT_02;


		    /* capture solution information */
  		    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
  		    if (error) goto QUIT_02;
  		    error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
  		    if (error) goto QUIT_02;
  		    error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, SystemVarOneHour_GENbase, sol);
  		    if (error) goto QUIT_02;

  		    if (optimstatus == GRB_OPTIMAL) 
		    {
  		    	printf("Optimization is completed & feasible\n");
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/ 
					printf("Optimal objective of the current hour: %.4e\n", objval);                                       
            				//printf("\n");                    
            				//for (int i = 0; i < GenVarOneUnit_base; ++i){
            				//     printf("In slave 01, [PG, I, SUD]= %.4f", sol[i]); 
            				//     printf("\n"); 
    	        			//}
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  		    } 
		    else if (optimstatus == GRB_INF_OR_UNBD) 
		    {
    		        printf("Model is infeasible or unbounded\n");
  		    } 
		    else 
  		    {
    		        printf("Optimization was stopped early\n");
  		    }


		    /* *sotre optimization solution of each generator* */
    		    for (int i = 0; i < NG; ++i)
		    {
            		PG_Systembase_k_slave[i * NT + t] = sol[i];
        	    }


  		    /* *Free GUROBI model* */
  		    GRBfreemodel(model);

	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/                    
            				printf("slave 02 in %d iteration for hour %d*******************************************************************************",k_slave_02, t); 
            				printf("\n"); 
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


                }//END:for(int i = 0; i < NT; ++i)

                /* **send information to the master node** */
                SlaveNode_sendto_MasterNode(PG_Systembase_k_slave, id_mpi_system_master, tag_mpi_system, NG, NT);
                
                /* **time the simulation time of current iteration** */
                //wtime_mpi = MPI_Wtime ( ) - wtime_mpi;
                //printf ( "  Slave node %d's simulation time in the iteration %d = %g\n",id_mpi, wtime_mpi, k_slave_01);

                /* **update the counter** */
                k_slave_02++;//count the iterative number of slave 02           
                
    		printf ("*************************************************************************************************************Slave-02 (k=%d) END\n",k_slave_02-1); 
            }//END:if (stop_decision_i == 0)
            
            else //if converged,just terminate
            {
                //printf("MPI communication will terminate, because the convergence is down.\n");
                ierr_mpi = MPI_Finalize ( );//All processes must call this routine before exiting. All processes will still exist but may not make any further MPI calls.
                break; //if converged, jumpe out of the "while: loop and terminate
            }//END:else

        }//END:while (1)



	/* ***if GUROBI error, just stop and report error*** */
	QUIT_02:
 	/* Error reporting */
  	if (error) 
	{
    	    printf("ERROR: %s\n", GRBgeterrormsg(env));
    	    exit(1);
 	}



  	/* ***Free GUROBI environment*** */
  	GRBfreeenv(env);



        //the following codes are only run once in a whole simulation process***********************************************************************************************
    	/* ***Time the total simulation time of slave node*** */
        wtime_mpi = MPI_Wtime ( ) - wtime_mpi;
        printf ( "Slave %d's total simulation time = %g\n",id_mpi, wtime_mpi );

    }//END:else if (id_mpi == 2)
    /**************************************************************************Slave-node-02's work in parallel manner END**************************************************************************/
    /**************************************************************************Slave-node-02's work in parallel manner END**************************************************************************/        
    /**************************************************************************Slave-node-02's work in parallel manner END**************************************************************************/



    /*********************************************************************Common work for all nodes START,executing only 1 time*********************************************************************/  
    /*********************************************************************Common work for all nodes START,executing only 1 time*********************************************************************/  
    /*********************************************************************Common work for all nodes START,executing only 1 time*********************************************************************/  
    printf ("*************************************************************************************************************Common work (2nd) START\n"); //executing only 1 time     
    /* ***Free pointers and data memory, at the end of each similation*** */
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
    printf ("*************************************************************************************************************Common work (2nd) END\n");
    /**********************************************************************Common work for all nodes END,executing only 1 time**********************************************************************/    
    /**********************************************************************Common work for all nodes END,executing only 1 time**********************************************************************/    
    /**********************************************************************Common work for all nodes END,executing only 1 time**********************************************************************/    


    
    return 0;

}//END:Main codes





