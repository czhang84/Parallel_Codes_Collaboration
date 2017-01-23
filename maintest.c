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
    	 LoadFactor_Set.LoadFactor_LoadDistribution; //Array:[1 * NLd], do NOT need
    	 LoadFactor_Set.LoadFactor_LoadFactor; //Array:[1 * NLd]
    	 LoadForecastData_Set.LoadForecast_CorrectionLoad; //Array:[1 * NT]
    	 LoadForecastData_Set.LoadForecast_LoadIEEE; //Array:[1 * NT]
    	 LoadForecastData_Set.LoadForecast_LoadPJM; //Array:[1 * NT]
         //ShiftFactorData (SFData is not formed as structure data.)   									
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
    const int NLd = col_LoadFactor, NB = col_SFData, NL = row_SFData, NT = col_LoadForecast, NG = col_GenData; 				


    /* ***Input the No. of switchable lines in system & the total quantity of switchable lines*** */					
    int No_SwitchableLineinSystem[] = {1, 3, 5, 7, 9};											//lin test**********************************
    int NL_switchable = sizeof(No_SwitchableLineinSystem) / sizeof(No_SwitchableLineinSystem[0]);					//lin test**********************************
    int NL_unswitchable;														
	
   
    /* ***Reform shiftfactor data*** */													
    //Note:(the line l & bus i)'s shiftfactor vector is equal to *(SFData_Set[l]+(i-1))
    double *SFData_Set[NL];
    for (int i = 0; i < NL; i++) 
    {
        SFData_Set[i] = &ShiftFactorData[i*NB];
    }


    /* ***Adjust hourly load*** */													
    for (int t = 0; t < NT; t++) 
    {
        LoadForecastData_Set.LoadForecast_CorrectionLoad[t] = LoadForecastData_Set.LoadForecast_CorrectionLoad[t] * 0.85;  		//Lin Note: Load*0.85 to make peak hourly load around 6000MW
    }


    /* ***Calculate basic varibales*** */
    //get generator's initial status
    int I_GENbase_Initial[NG];
    for (int i = 0; i < NG; ++i)
    {
        I_GENbase_Initial[i] = GenData_Set.Gen_InitialOn[i] >= 1;
    }
    //set generator's initial power output is equal to min capacity if it's on 								
    int PG_GENbase_Initial[NG];
    for (int i = 0; i < NG; ++i)
    {
        PG_GENbase_Initial[i] = I_GENbase_Initial[i] * GenData_Set.Gen_Pmin[i];
    }
    //get the load relationship between all buses and load buses									
    // in order to ditribute load factor to each bus at each hour
    double LoadFactor_allbus[NB];
    for (int i = 0; i < NB; ++i) 
    {
	LoadFactor_allbus[i] = 0;
        for (int j = 0; j < NLd; ++j)
        { 
            if ((LoadFactor_Set.LoadFactor_BusNum[j] - 1) == i)
	    {
               LoadFactor_allbus[i] = LoadFactor_Set.LoadFactor_LoadFactor[j];
            }
        }
    }


    /* ***Define the control parameters (used in master node)*** */
    const int    k_limit = 80;                             										//lin test***********************************************
    const double Convergence_threshold_Coefficient = 0.2;
    const double Convergence_threshold_PG_GENbase=Convergence_threshold_Coefficient;
    const double Convergence_threshold_SR_GENbase=Convergence_threshold_Coefficient;							
    const double Convergence_threshold_PT_TSbase=Convergence_threshold_Coefficient;							
    const double Convergence_threshold_PLswitchable_TSbase=Convergence_threshold_Coefficient;						
    const double Lambda_AllModule_Initial = 0;
    const double C_AllModule_Initial = 0.05;
    const double Beta_UpdateC = 1.01;


    /* ***Define the control parameters to relax module&constrint (used in master&slave node)*** */					
    //0=relax;1=not relax
    //control variable to relax module
    const int RelaxGENbase_Module=0;
    const int RelaxTSbase_Module=0;                                    
    const int RelaxSystembase_Module=0;
    //control variable to relax constraint
    //relax Gen_base constraints
    const int RelaxGENbase_capacity=0;
    const int RelaxGENbase_ramping=0;
    const int RelaxGENbase_minonoff=0;
    const int RelaxGENbase_startshut=0;
    //relax SR_base constraints
    const int RelaxSRbase_maxoutput=0;
    const int RelaxSRbase_maxsustained=0;
    //relax TS_base constraints
    const int RelaxTSbase_PTcapacity=0;                                    
    const int RelaxTSbase_PLlimit=0;                                        
    const int RelaxTSbase_PT2PLrelation=0;                                    
    //relax System_base constraints    
    const int RelaxPowerBalance_base=0;
    const int RelaxSystemSR_base=0;
    const int RelaxPLunswitchablelimit_base=1;                                    
    const int RelaxPLswitchablelimit_base=0;                                    
    const int RelaxPLswitchableEquation_base=0;    


    /* ***Define the parameters (used in certain slave node)*** */									
    const double M_limit = 5000; //used in TS module 


    /* ***divide switchable & unswitchable Line data*** */										
    //define switchable & unswitchable Line data
    if (RelaxTSbase_Module == 1) //TS base module is relaxed,then no switchable line														
    {
        NL_switchable = 0;
        NL_unswitchable = NL - NL_switchable;
    }
    if (RelaxTSbase_Module == 0) //TS base module is NOT relaxed														
    {
        NL_unswitchable = NL - NL_switchable;
    }
    int LineNo_unswitchable[NL_unswitchable];
    int LineFromBusNo_unswitchable[NL_unswitchable];
    int LineToBusNo_unswitchable[NL_unswitchable];
    double PLlimit_unswitchable[NL_unswitchable];
    double *SF_unswitchable_PointerSet[NL_unswitchable];
    int LineNo_switchable[NL_switchable];
    int LineFromBusNo_switchable[NL_switchable];
    int LineToBusNo_switchable[NL_switchable];
    double PLlimit_switchable[NL_switchable];
    double *SF_switchable_PointerSet[NL_switchable];
    int Bus_withPT_positive[NL_switchable]; //bus No. a positive visual gen is connected to
    int Bus_withPT_negative[NL_switchable]; //bus No. a negative visual gen is connected to
    //for the scenario:without switchable line
    if (RelaxTSbase_Module == 1) //TS base module is relaxed,then no switchable line														
    {
	for (int i = 0; i < NL_unswitchable; ++i)
	{
            LineNo_unswitchable[i] = LineData_Set.Line_Num[i];
            LineFromBusNo_unswitchable[i] = LineData_Set.Line_FromBus[i];
            LineToBusNo_unswitchable[i] = LineData_Set.Line_ToBus[i];
            PLlimit_unswitchable[i] = LineData_Set.Line_PowerLimit[i];
	    SF_unswitchable_PointerSet[i] = SFData_Set[i]; //Note:(the unswitchable line l & bus i)'s shiftfactor vector is equal to *(SF_unswitchable_PointerSet[l]+(i-1))
        }
    }
    //for the scenario:with switchable line
    if (RelaxTSbase_Module == 0) //TS base module is NOT relaxed														
    {
	//collect switchable line data
	for (int j = 0; j < NL_switchable; ++j)
	{
	    for (int i = 0; i < NL; ++i)
	    {
    		if ((No_SwitchableLineinSystem[j] - 1) == i) 
		{	
            	   LineNo_switchable[j] = LineData_Set.Line_Num[i];
            	   LineFromBusNo_switchable[j] = LineData_Set.Line_FromBus[i];
            	   LineToBusNo_switchable[j] = LineData_Set.Line_ToBus[i];
            	   PLlimit_switchable[j] = LineData_Set.Line_PowerLimit[i];
	    	   SF_switchable_PointerSet[j] = SFData_Set[i]; //Note:(the line l & bus i)'s shiftfactor vector is equal to *(SF_switchable_PointerSet[l+(i-1))
		}
	    }
        }
	//collect unswitchable line data
	int temporaryvar_unswitchableCounter = -1;
	int temporaryarray_judgeswitchable[NL_switchable];
	int sum_temporaryarray_judgeswitchable;
    	for (int i = 0; i < NL; ++i) 
	{
            for (int j = 0; j < NL_switchable; ++j)
	    {
		temporaryarray_judgeswitchable[j] = (i != (No_SwitchableLineinSystem[j] - 1)); //if not equal, temporaryarray_judgeswitchable[j]=1
 	    }
            sum_temporaryarray_judgeswitchable = 0;
	    for (int j = 0; j < NL_switchable; ++j)
	    {
		sum_temporaryarray_judgeswitchable = sum_temporaryarray_judgeswitchable + temporaryarray_judgeswitchable[j];
	    } 
    	    if (sum_temporaryarray_judgeswitchable == NL_switchable) //if line (i+1) is NOT switchable
	    {
	       if (temporaryvar_unswitchableCounter + 1 < NL_unswitchable)
	       {	
	          temporaryvar_unswitchableCounter = temporaryvar_unswitchableCounter + 1;
            	  LineNo_unswitchable[temporaryvar_unswitchableCounter] = LineData_Set.Line_Num[i];
            	  LineFromBusNo_unswitchable[temporaryvar_unswitchableCounter] = LineData_Set.Line_FromBus[i];
            	  LineToBusNo_unswitchable[temporaryvar_unswitchableCounter] = LineData_Set.Line_ToBus[i];
            	  PLlimit_unswitchable[temporaryvar_unswitchableCounter] = LineData_Set.Line_PowerLimit[i];
	    	  SF_unswitchable_PointerSet[temporaryvar_unswitchableCounter] = SFData_Set[i]; //Note:(the line l & bus i)'s shiftfactor vector is equal to *(SF_unswitchable_PointerSet[l+(i-1)) 
	       }
	    }
	 }
	//get PT connected bus No.
	for (int i = 0; i < NL_switchable; ++i)
	{
	    Bus_withPT_positive[i] = LineFromBusNo_switchable[i];
	    Bus_withPT_negative[i] = LineToBusNo_switchable[i];
	}
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
    int id_mpi_ts_master = 0; 														
    int tag_mpi_ts = 3;															
    /*ID and Tag: send to MASTER NODES setting*/
    int id_mpi_gen_slave = 1; 
    int id_mpi_system_slave = 2;
    int id_mpi_ts_slave = 3;														


    /* ***get the MPI environment information*** */
    ierr_mpi = MPI_Init(&argc, &argv);
    ierr_mpi = MPI_Comm_rank(MPI_COMM_WORLD, &id_mpi);
    ierr_mpi = MPI_Comm_size(MPI_COMM_WORLD, &size_mpi);
    

    /* ***Make sure we have enough processes*** */
    if ( size_mpi < 4 ) {														
        printf ( "\n" );
        printf ( "MPI_MULTITASK - Fatal error!\n" );
        printf ( "Number of available processes must be at least 3!\n" );
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
        
    
    	/* ***Define and initialize the variables*** */
    	double PG_GENbase_k[NG*NT];    //send from GEN module node to main node & main node to Syst module node
    	int    I_GENbase_k[NG*NT];
    	double SUD_GENbase_k[NG*NT];
    	double PG_Systembase_k[NG*NT]; //send from Syst module node to main node & main node to GEN module node
    	double SR_GENbase_k[NG*NT];    //send from GEN module node to main node & main node to Syst module node				
    	double SR_Systembase_k[NG*NT]; //send from Syst module node to main node & main node to GEN module node				
    	for (int i = 0; i < NG * NT; ++i){
            PG_GENbase_k[i] = 0;
            I_GENbase_k[i] = 0;
            SUD_GENbase_k[i] = 0;
            PG_Systembase_k[i] = 0;
            SR_GENbase_k[i] = 0;													
            SR_Systembase_k[i] = 0;													
        }
    	double PT_TSbase_k[NL_switchable*NT];    //send from TS module node to main node & main node to Syst module node		
    	double PLswitchable_TSbase_k[NL_switchable*NT];    //send from TS module node to main node & main node to Syst module node	
    	double Z_TSbase_k[NL_switchable*NT];												
    	double PT_Systembase_k[NL_switchable*NT]; //send from Syst module node to main node & main node to TS module node		
    	double PLswitchable_Systembase_k[NL_switchable*NT]; //send from Syst module node to main node & main node to TS module node	
    	for (int i = 0; i < NL_switchable * NT; ++i){
            PT_TSbase_k[i] = 0;														
            PLswitchable_TSbase_k[i] = 0;												
            Z_TSbase_k[i] = 0;														
            PT_Systembase_k[i] = 0;													
            PLswitchable_Systembase_k[i] = 0;												
        }


    	/* ***Define and initialize the iteratively changed parameters*** */
    	int    k = 1;
    	int    Covergence_judge_Allmodule = 0;												
    	int    Covergence_judge_PG_GENbase = 0;
    	double Lambda_PG_GENbase_k[NG*NT]; //in every iteration, send from the main node to the GEN module node
    	double C_PG_GENbase_k[NG*NT]; //in every iteration, send from the main node to the Syst module node
    	double Difference_PG_GENbase_k[NG*NT];
    	int    Convergence_ElementMatrix_PG_GENbase_k[NG*NT];
    	double Convergence_Percentage_PG_GENbase_k = 0;
    	int    Covergence_judge_SR_GENbase = 0;												
    	double Lambda_SR_GENbase_k[NG*NT]; //in every iteration, send from the main node to the GEN module node				
    	double C_SR_GENbase_k[NG*NT]; //in every iteration, send from the main node to the Syst module node				
    	double Difference_SR_GENbase_k[NG*NT];												
    	int    Convergence_ElementMatrix_SR_GENbase_k[NG*NT];										
    	double Convergence_Percentage_SR_GENbase_k = 0;											
    	double Convergence_Percentage_Allmodule_k = 0;											
    	for (int i = 0; i < NG * NT; ++i){
            Lambda_PG_GENbase_k[i] = Lambda_AllModule_Initial;
            C_PG_GENbase_k[i] = C_AllModule_Initial;
            Difference_PG_GENbase_k[i] = 0;
            Convergence_ElementMatrix_PG_GENbase_k[i] = 0; 
            Lambda_SR_GENbase_k[i] = Lambda_AllModule_Initial;										
            C_SR_GENbase_k[i] = C_AllModule_Initial;											
            Difference_SR_GENbase_k[i] = 0;												
            Convergence_ElementMatrix_SR_GENbase_k[i] = 0;										
    	}
    	int    Covergence_judge_PT_TSbase = 0;												
    	double Lambda_PT_TSbase_k[NL_switchable*NT]; //in every iteration, send from the main node to the TS module node		
    	double C_PT_TSbase_k[NL_switchable*NT]; //in every iteration, send from the main node to the TS module node			
    	double Difference_PT_TSbase_k[NL_switchable*NT];										
    	int    Convergence_ElementMatrix_PT_TSbase_k[NL_switchable*NT];								
    	double Convergence_Percentage_PT_TSbase_k = 0;										
    	int    Covergence_judge_PLswitchable_TSbase = 0;										
    	double Lambda_PLswitchable_TSbase_k[NL_switchable*NT]; //in every iteration, send from the main node to the TS module node	
    	double C_PLswitchable_TSbase_k[NL_switchable*NT]; //in every iteration, send from the main node to the TS module node		
    	double Difference_PLswitchable_TSbase_k[NL_switchable*NT];									
    	int    Convergence_ElementMatrix_PLswitchable_TSbase_k[NL_switchable*NT];							
    	double Convergence_Percentage_PLswitchable_TSbase_k = 0;									
    	for (int i = 0; i < NL_switchable * NT; ++i){											
            Lambda_PT_TSbase_k[i] = Lambda_AllModule_Initial;										
            C_PT_TSbase_k[i] = C_AllModule_Initial;											
            Difference_PT_TSbase_k[i] = 0;												
            Convergence_ElementMatrix_PT_TSbase_k[i] = 0; 										
            Lambda_PLswitchable_TSbase_k[i] = Lambda_AllModule_Initial;									
            C_PLswitchable_TSbase_k[i] = C_AllModule_Initial;										
            Difference_PLswitchable_TSbase_k[i] = 0;											
            Convergence_ElementMatrix_PLswitchable_TSbase_k[i] = 0;									
    	}
    

        /* ***define MPI related variables*** */  
        int stop_decision;


    	/****************************************Iterative calculation start****************************************/
    	/****************************************Iterative calculation start****************************************/
        while (Covergence_judge_Allmodule == 0 && k <= k_limit) //=0, not converge, so should continue				
        {

            printf ("*************************************************************************************************************Master (K=%d) START\n",k);

    	    /* **master node send information to slave nodes** */
            //set & send stop decision (convergence judgement) to all slave nodes
            stop_decision = Covergence_judge_Allmodule;										
            MasterNode_send_stopdecision( size_mpi, stop_decision);
            //send information to GEN module
            MasterNode_sendto_SlaveNode(Lambda_PG_GENbase_k, id_mpi_gen_slave, tag_mpi_gen, NG, NT);
            MasterNode_sendto_SlaveNode(C_PG_GENbase_k, id_mpi_gen_slave, tag_mpi_gen, NG, NT);
            MasterNode_sendto_SlaveNode(PG_Systembase_k, id_mpi_gen_slave, tag_mpi_gen, NG, NT);
            MasterNode_sendto_SlaveNode(Lambda_SR_GENbase_k, id_mpi_gen_slave, tag_mpi_gen, NG, NT);					
            MasterNode_sendto_SlaveNode(C_SR_GENbase_k, id_mpi_gen_slave, tag_mpi_gen, NG, NT);					
            MasterNode_sendto_SlaveNode(SR_Systembase_k, id_mpi_gen_slave, tag_mpi_gen, NG, NT); 				
            //send information to TS module
            MasterNode_sendto_SlaveNode(Lambda_PT_TSbase_k, id_mpi_ts_slave, tag_mpi_ts, NL_switchable, NT);			
            MasterNode_sendto_SlaveNode(C_PT_TSbase_k, id_mpi_ts_slave, tag_mpi_ts, NL_switchable, NT);				
            MasterNode_sendto_SlaveNode(PT_Systembase_k, id_mpi_ts_slave, tag_mpi_ts, NL_switchable, NT);				
            MasterNode_sendto_SlaveNode(Lambda_PLswitchable_TSbase_k, id_mpi_ts_slave, tag_mpi_ts, NL_switchable, NT);			
            MasterNode_sendto_SlaveNode(C_PLswitchable_TSbase_k, id_mpi_ts_slave, tag_mpi_ts, NL_switchable, NT);		
            MasterNode_sendto_SlaveNode(PLswitchable_Systembase_k, id_mpi_ts_slave, tag_mpi_ts, NL_switchable, NT); 		
            //send information to Syst module
            MasterNode_sendto_SlaveNode(Lambda_PG_GENbase_k, id_mpi_system_slave, tag_mpi_system, NG, NT);		
            MasterNode_sendto_SlaveNode(C_PG_GENbase_k, id_mpi_system_slave, tag_mpi_system, NG, NT);		
            MasterNode_sendto_SlaveNode(PG_GENbase_k, id_mpi_system_slave, tag_mpi_system, NG, NT);		
            MasterNode_sendto_SlaveNode(Lambda_SR_GENbase_k, id_mpi_system_slave, tag_mpi_system, NG, NT);			
            MasterNode_sendto_SlaveNode(C_SR_GENbase_k, id_mpi_system_slave, tag_mpi_system, NG, NT);					
            MasterNode_sendto_SlaveNode(SR_GENbase_k, id_mpi_system_slave, tag_mpi_system, NG, NT);    				
            MasterNode_sendto_SlaveNode(Lambda_PT_TSbase_k, id_mpi_system_slave, tag_mpi_system, NL_switchable, NT);			
            MasterNode_sendto_SlaveNode(C_PT_TSbase_k, id_mpi_system_slave, tag_mpi_system, NL_switchable, NT);				
            MasterNode_sendto_SlaveNode(PT_TSbase_k, id_mpi_system_slave, tag_mpi_system, NL_switchable, NT);				
            MasterNode_sendto_SlaveNode(Lambda_PLswitchable_TSbase_k, id_mpi_system_slave, tag_mpi_system, NL_switchable, NT);	
            MasterNode_sendto_SlaveNode(C_PLswitchable_TSbase_k, id_mpi_system_slave, tag_mpi_system, NL_switchable, NT);		
            MasterNode_sendto_SlaveNode(PLswitchable_TSbase_k, id_mpi_system_slave, tag_mpi_system, NL_switchable, NT);    		
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/                    
                			for (int i = 0; i < NG*NT; ++i){
            	    	     		    printf("In iteration %d, master send to slaves: Lambda_PG_GENbase_k[%d]= %f",k, i,Lambda_PG_GENbase_k[i]); 
            	                	    printf("\n");
            	    	     		    printf("In iteration %d, master send to slaves: C_PG_GENbase_k[%d]= %f",k, i,C_PG_GENbase_k[i]); 
            	                	    printf("\n");
            	    	     		//    printf("In iteration %d, master send to slave 01: PG_GENbase_k[%d]= %f",k, i,PG_GENbase_k[i]); 
            	                	//    printf("\n");
            	    	     		//    printf("In iteration %d, master send to slave 02: PG_Systembase_k[%d]= %f",k, i,PG_Systembase_k[i]); 
            	                	//    printf("\n");
            	    	     		    printf("In iteration %d, master send to slaves: Lambda_SR_GENbase_k[%d]= %f",k, i,Lambda_SR_GENbase_k[i]); 
            	                	    printf("\n");
            	    	     		    printf("In iteration %d, master send to slaves: C_SR_GENbase_k[%d]= %f",k, i,C_SR_GENbase_k[i]); 		
            	                	    printf("\n");
            	    	     		//    printf("In iteration %d, master send to slave 01: SR_GENbase_k[%d]= %f",k, i,SR_GENbase_k[i]); 		
            	                	//    printf("\n");
            	    	     		//    printf("In iteration %d, master send to slave 02: SR_Systembase_k[%d]= %f",k, i,SR_Systembase_k[i]); 	
            	                	//   printf("\n");
    	        			}
                			for (int i = 0; i < NL_switchable*NT; ++i){
            	    	     		    printf("In iteration %d, master send to slaves: Lambda_PT_TSbase_k[%d]= %f",k, i,Lambda_PT_TSbase_k[i]); 		
            	                	    printf("\n");
            	    	     		    printf("In iteration %d, master send to slaves: C_PT_TSbase_k[%d]= %f",k, i,C_PT_TSbase_k[i]); 			
            	                	    printf("\n");
            	    	     		//    printf("In iteration %d, master send to slave 03: PT_TSbase_k[%d]= %f",k, i,PT_TSbase_k[i]); 		
            	                	//    printf("\n");
            	    	     		//    printf("In iteration %d, master send to slave 02: PT_Systembase_k[%d]= %f",k, i,PT_Systembase_k[i]); 		
            	                	//    printf("\n");
            	    	     		    printf("In iteration %d, master send to slaves: Lambda_PLswitchable_TSbase_k[%d]= %f",k, i,Lambda_PLswitchable_TSbase_k[i]); 
            	                	    printf("\n");
            	    	     		    printf("In iteration %d, master send to slaves: C_PLswitchable_TSbase_k[%d]= %f",k, i,C_PLswitchable_TSbase_k[i]); 	
            	                	    printf("\n");
            	    	     		//    printf("In iteration %d, master send to slave 03: PLswitchable_TSbase_k[%d]= %f",k, i,PLswitchable_TSbase_k[i]); 		
            	                	//    printf("\n");
            	    	     		//    printf("In iteration %d, master send to slave 02: PLswitchable_Systembase_k[%d]= %f",k, i,PLswitchable_Systembase_k[i]); 
            	                	//    printf("\n");
    	        			}
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/	    			


    	    /* **master node recieve information from slave nodes** */
            //receive PG&SR from GEN module
            MasterNode_receivefrom_SlaveNode(PG_GENbase_k, id_mpi_gen_slave, tag_mpi_gen, NG, NT);
            MasterNode_receivefrom_SlaveNode(SR_GENbase_k, id_mpi_gen_slave, tag_mpi_gen, NG, NT);					
            //receive PT&PLswitchable from TS module
            MasterNode_receivefrom_SlaveNode(PT_TSbase_k, id_mpi_ts_slave, tag_mpi_ts, NL_switchable, NT);				
            MasterNode_receivefrom_SlaveNode(PLswitchable_TSbase_k, id_mpi_ts_slave, tag_mpi_ts, NL_switchable, NT);			
            //receive PG&SR&PT&PLswitchable from Syst module
            MasterNode_receivefrom_SlaveNode(PG_Systembase_k, id_mpi_system_slave, tag_mpi_system, NG, NT);
            MasterNode_receivefrom_SlaveNode(SR_Systembase_k, id_mpi_system_slave, tag_mpi_system, NG, NT);				
            MasterNode_receivefrom_SlaveNode(PT_Systembase_k, id_mpi_system_slave, tag_mpi_system, NL_switchable, NT);			
            MasterNode_receivefrom_SlaveNode(PLswitchable_Systembase_k, id_mpi_system_slave, tag_mpi_system, NL_switchable, NT);	
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/                    
                			for (int i = 0; i < NG*NT; ++i){
            	    	     		    printf("In iteration %d, master receive from slave 01: PG_GENbase_k[%d]= %f",k, i,PG_GENbase_k[i]); 
            	                	    printf("\n");
            	    	     		    printf("In iteration %d, master receive from slave 02: PG_Systembase_k[%d]= %f",k, i,PG_Systembase_k[i]); 
            	                	    printf("\n");
            	    	     		    printf("In iteration %d, master receive from slave 01: SR_GENbase_k[%d]= %f",k, i,SR_GENbase_k[i]); 		
            	                	    printf("\n");
            	    	     		    printf("In iteration %d, master receive from slave 02: SR_Systembase_k[%d]= %f",k, i,SR_Systembase_k[i]); 	
            	                	    printf("\n");
    	        			}
                			for (int i = 0; i < NL_switchable*NT; ++i){
            	    	     		    printf("In iteration %d, master receive from slave 03: PT_TSbase_k[%d]= %f",k, i,PT_TSbase_k[i]); 		
            	                	    printf("\n");
            	    	     		    printf("In iteration %d, master receive from slave 02: PT_Systembase_k[%d]= %f",k, i,PT_Systembase_k[i]); 		
            	                	    printf("\n");
            	    	     		    printf("In iteration %d, master receive from slave 03: PLswitchable_TSbase_k[%d]= %f",k, i,PLswitchable_TSbase_k[i]); 
            	                	    printf("\n");
            	    	     		    printf("In iteration %d, master receive from slave 02: PLswitchable_Systembase_k[%d]= %f",k, i,PLswitchable_Systembase_k[i]); 
            	                	    printf("\n");
    	        			}
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


            /* **analyze the convergence of PG_GENbase** */                                                     				
	    //define a variable to store total no. of converged elem, in a new iteration, it should return to 0
	    int TotalQuantity_ConvergedPG_GENbase_k = 0;
            //cal diff betw elem, compare with threshold, cal total No. of converged elem
	    for (int i = 0; i < NG * NT; ++i)
            {
                Difference_PG_GENbase_k[i] = PG_GENbase_k[i] - PG_Systembase_k[i]; //calc the diff betw elements of 2 arrays
		Convergence_ElementMatrix_PG_GENbase_k[i] = abs(Difference_PG_GENbase_k[i]) <= (Convergence_threshold_PG_GENbase + 0.0001); //compare the diff with threshold
                TotalQuantity_ConvergedPG_GENbase_k = TotalQuantity_ConvergedPG_GENbase_k + Convergence_ElementMatrix_PG_GENbase_k[i]; //cal total quantity of converged elements
    	    }
            //cal convergence percentage for PG     
            Convergence_Percentage_PG_GENbase_k=TotalQuantity_ConvergedPG_GENbase_k * 100 / (NG * NT);
            //get the convergence judge variable  for PG                                                               		
	    if (Convergence_Percentage_PG_GENbase_k == 100)
	    {
	        Covergence_judge_PG_GENbase = 1;
            }
            if (Convergence_Percentage_PG_GENbase_k != 100)
            {
                Covergence_judge_PG_GENbase = 0;
            }


	    /* **analyze the convergence of SR_GENbase** */           									
	    //define a variable to store total no. of converged elem, in a new iteration, it should return to 0
	    int TotalQuantity_ConvergedSR_GENbase_k = 0;
            //cal diff betw elem, compare with threshold, cal total No. of converged elem
	    for (int i = 0; i < NG * NT; ++i)
            {
                Difference_SR_GENbase_k[i] = SR_GENbase_k[i] - SR_Systembase_k[i]; //calc the diff betw elements of 2 arrays
		Convergence_ElementMatrix_SR_GENbase_k[i] = abs(Difference_SR_GENbase_k[i]) <= (Convergence_threshold_SR_GENbase + 0.0001); //compare the diff with threshold
                TotalQuantity_ConvergedSR_GENbase_k = TotalQuantity_ConvergedSR_GENbase_k + Convergence_ElementMatrix_SR_GENbase_k[i]; //cal total quantity of converged elements
    	    }
            //cal convergence percentage for SR     
            Convergence_Percentage_SR_GENbase_k=TotalQuantity_ConvergedSR_GENbase_k * 100 / (NG * NT);
            //get the convergence judge variable for SR                                                               		
	    if (Convergence_Percentage_SR_GENbase_k == 100)
	    {
	        Covergence_judge_SR_GENbase = 1;
            }
            if (Convergence_Percentage_SR_GENbase_k != 100)
            {
                Covergence_judge_SR_GENbase = 0;
            }


            /* **analyze the convergence of PT_TSbase** */    									                        		    
            //define a variable to store total no. of converged elem, in a new iteration, it should return to 0
	    int TotalQuantity_ConvergedPT_TSbase_k = 0;
            //cal diff betw elem, compare with threshold, cal total No. of converged elem
	    for (int i = 0; i < NL_switchable * NT; ++i)
            {
                Difference_PT_TSbase_k[i] = PT_TSbase_k[i] - PT_Systembase_k[i]; //calc the diff betw elements of 2 arrays
		Convergence_ElementMatrix_PT_TSbase_k[i] = abs(Difference_PT_TSbase_k[i]) <= (Convergence_threshold_PT_TSbase + 0.0001); //compare the diff with threshold
                TotalQuantity_ConvergedPT_TSbase_k = TotalQuantity_ConvergedPT_TSbase_k + Convergence_ElementMatrix_PT_TSbase_k[i]; //cal total quantity of converged elements
    	    }
            //cal convergence percentage for PT     
            Convergence_Percentage_PT_TSbase_k=TotalQuantity_ConvergedPT_TSbase_k * 100 / (NL_switchable * NT);
            //get the convergence judge variable  for PT                                                               		
	    if (Convergence_Percentage_PT_TSbase_k == 100)
	    {
	        Covergence_judge_PT_TSbase = 1;
            }
            if (Convergence_Percentage_PT_TSbase_k != 100)
            {
                Covergence_judge_PT_TSbase = 0;
            }


            /* **analyze the convergence of PLswitchable_TSbase** */    							
            //define a variable to store total no. of converged elem, in a new iteration, it should return to 0
	    int TotalQuantity_ConvergedPLswitchable_TSbase_k = 0;
            //cal diff betw elem, compare with threshold, cal total No. of converged elem
	    for (int i = 0; i < NL_switchable * NT; ++i)
            {
                Difference_PLswitchable_TSbase_k[i] = PLswitchable_TSbase_k[i] - PLswitchable_Systembase_k[i]; //calc the diff betw elements of 2 arrays
		Convergence_ElementMatrix_PLswitchable_TSbase_k[i] = abs(Difference_PLswitchable_TSbase_k[i]) <= (Convergence_threshold_PLswitchable_TSbase + 0.0001); //compare the diff with threshold
                TotalQuantity_ConvergedPLswitchable_TSbase_k = TotalQuantity_ConvergedPLswitchable_TSbase_k + Convergence_ElementMatrix_PLswitchable_TSbase_k[i]; //cal total quantity of converged elements
    	    }
            //cal convergence percentage for PT     
            Convergence_Percentage_PLswitchable_TSbase_k=TotalQuantity_ConvergedPLswitchable_TSbase_k * 100 / (NL_switchable * NT);
            //get the convergence judge variable  for PT                                                               		
	    if (Convergence_Percentage_PLswitchable_TSbase_k == 100)
	    {
	        Covergence_judge_PLswitchable_TSbase = 1;
            }
            if (Convergence_Percentage_PLswitchable_TSbase_k != 100)
            {
                Covergence_judge_PLswitchable_TSbase = 0;
            }
	    /* **analyze the convergence of all variables in all modules** */           						
	    Convergence_Percentage_Allmodule_k = (TotalQuantity_ConvergedPG_GENbase_k + TotalQuantity_ConvergedSR_GENbase_k 
                                                  + TotalQuantity_ConvergedPT_TSbase_k + TotalQuantity_ConvergedPLswitchable_TSbase_k) 
                                                  * 100 / ( NG * NT + NG * NT + NL_switchable * NT + NL_switchable * NT);


	    /* **update the convergence judge variable for all variables in all modules** */ 								
	    if (Covergence_judge_PG_GENbase + Covergence_judge_SR_GENbase + Covergence_judge_PT_TSbase + Covergence_judge_PLswitchable_TSbase == 4)
	    {
	        Covergence_judge_Allmodule = 1;
            }
            if (Covergence_judge_PG_GENbase + Covergence_judge_SR_GENbase + Covergence_judge_PT_TSbase + Covergence_judge_PLswitchable_TSbase <= 3)
            {
                Covergence_judge_Allmodule = 0;
            }


    	    printf ("*************************************************************************************************************Master (K=%d) END\n",k); 

            /* **if not converge, update parameters** */                                                               			
            if (Covergence_judge_Allmodule == 0) //if not converge									
	    {
	        k=k+1;
                //Update_Penalty(); //call function to update Lagrangian penalties
	    	for (int i = 0; i < NG * NT; ++i)
            	{
                    Lambda_PG_GENbase_k[i] = Lambda_PG_GENbase_k[i] + C_PG_GENbase_k[i] * Difference_PG_GENbase_k[i]; //update Lambda
                    C_PG_GENbase_k[i] = C_PG_GENbase_k[i] * Beta_UpdateC; //update C	
                    Lambda_SR_GENbase_k[i] = Lambda_SR_GENbase_k[i] + C_SR_GENbase_k[i] * Difference_SR_GENbase_k[i]; //update Lambda	
                    C_SR_GENbase_k[i] = C_SR_GENbase_k[i] * Beta_UpdateC; //update C 							
    	    	}
	    	for (int i = 0; i < NL_switchable * NT; ++i)
            	{
                    Lambda_PT_TSbase_k[i] = Lambda_PT_TSbase_k[i] + C_PT_TSbase_k[i] * Difference_PT_TSbase_k[i]; //update Lambda				
                    C_PT_TSbase_k[i] = C_PT_TSbase_k[i] * Beta_UpdateC; //update C										
                    Lambda_PLswitchable_TSbase_k[i] = Lambda_PLswitchable_TSbase_k[i] + C_PLswitchable_TSbase_k[i] * Difference_PLswitchable_TSbase_k[i]; 
                    C_PLswitchable_TSbase_k[i] = C_PLswitchable_TSbase_k[i] * Beta_UpdateC; //update C 								
    	    	}
            }


            /* **if converge, jumpe out of the "while: loop and terminate** */ 
	    if (Covergence_judge_Allmodule == 1 || k > k_limit )									
	    {
	        stop_decision = Covergence_judge_PG_GENbase;  							//zhangupdate
		MasterNode_send_stopdecision( size_mpi, stop_decision);						//zhangupdate
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
	//power generation cost
        double Total_OperationCost_GENbase = 0;      											
	for (int i = 0; i < NG; ++i)
        {
	    for (int j = 0; j < NT; ++j)
            {
		Total_OperationCost_GENbase = Total_OperationCost_GENbase + GenData_Set.Gen_PriceA[i] * I_GENbase_k[i * NT + j] + GenData_Set.Gen_PriceB[i] * PG_GENbase_k[i * NT + j]  					      + GenData_Set.Gen_PriceC[i] * PG_GENbase_k[i * NT + j] * PG_GENbase_k[i * NT + j] + SUD_GENbase_k[i * NT + j];
            }
        }
	//spinning reserve cost
        double Total_OperationCost_SRbase = 0;      											
	for (int i = 0; i < NG; ++i)
        {
	    for (int j = 0; j < NT; ++j)
            {
		Total_OperationCost_SRbase = Total_OperationCost_SRbase + SRCostandMaxSustainedRateData_Set.SR_MSR_CostCoefficient[i] * SR_GENbase_k[i * NT + j];
            }
        }
	//total operation cost														
        double Total_OperationCost_Allbase = Total_OperationCost_GENbase + Total_OperationCost_SRbase; 



    	/* ***Print key parameters*** */
    	printf("Iteration Number is %d \n", k);
    	printf("Convergence percentage is %f (percent)\n", Convergence_Percentage_Allmodule_k);						
    	printf("Total_OperationCost_GENbase = %f \n", Total_OperationCost_GENbase);							
    	printf("Total_OperationCost_SRbase = %f \n", Total_OperationCost_SRbase);						
    	printf("Total_OperationCost_Allbase = %f \n", Total_OperationCost_Allbase);						
			


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
	//variable structure: [PG, I, SUD, SR] *NT
	const int GenVarOneHourOneUnit_base = 4;										
	const int GenVarOneUnit_base = GenVarOneHourOneUnit_base * NT;


	/* ***define (constant) paremeter of total quantity of quadratic objective terms in a module*** */
	const int GenQuadraticTermOneUnit_base = 2* NT;										


	/* ***define (constant) offset of each variable in its smallest module*** */
	const int OffsetPG_base = 0; //should start from 0
	const int OffsetI_base = 1;
	const int OffsetSUD_base = 2;
	const int OffsetSR_base = 3;												


    	/* ***Define and initialize variables resulted from this slave load*** */
    	double PG_GENbase_k_slave[NG*NT];
    	double I_GENbase_k_slave[NG*NT];
    	double SUD_GENbase_k_slave[NG*NT];
    	double SR_GENbase_k_slave[NG*NT];										
    	for (int i = 0; i < NG*NT; ++i)
	{
            PG_GENbase_k_slave[i]=0;
            I_GENbase_k_slave[i]=0; 
            SUD_GENbase_k_slave[i]=0; 
            SR_GENbase_k_slave[i]=0; 												
        }


        /* ***define arrays to receive information from the master node*** */
        int    stop_decision_i;
        double Lambda_PG_GENbase_k_slave[NG * NT];
        double C_PG_GENbase_k_slave[NG * NT];      
        double PG_Systembase_k_slave[NG * NT];
        double Lambda_SR_GENbase_k_slave[NG * NT];											
        double C_SR_GENbase_k_slave[NG * NT];      											
        double SR_Systembase_k_slave[NG * NT];    											


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
            //printf("stop decision received at slave node %d is %d \n", tag_mpi_gen, stop_decision_i);
            
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
                SlaveNode_receivefrom_MasterNode(Lambda_SR_GENbase_k_slave, id_mpi_gen_master, tag_mpi_gen, NG, NT);				
                SlaveNode_receivefrom_MasterNode(C_SR_GENbase_k_slave, id_mpi_gen_master, tag_mpi_gen, NG, NT);					
                SlaveNode_receivefrom_MasterNode(SR_Systembase_k_slave, id_mpi_gen_master, tag_mpi_gen, NG, NT); 			


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
		    for (int t = 0; t < NT; ++t)									
		    {
                           qrow[2 * t] = GenVarOneHourOneUnit_base * t + OffsetPG_base;
                           qcol[2 * t] = GenVarOneHourOneUnit_base * t + OffsetPG_base; 
                           qval[2 * t] = C_PG_GENbase_k_slave[i * NT + t] + GenData_Set.Gen_PriceC[i];							
                           qrow[2 * t + 1] = GenVarOneHourOneUnit_base * t + OffsetSR_base;
                           qcol[2 * t + 1] = GenVarOneHourOneUnit_base * t + OffsetSR_base; 
                           qval[2 * t + 1] = C_SR_GENbase_k_slave[i * NT + t];										
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
  		    	error = GRBsetdblattrelement(model, GRB_DBL_ATTR_OBJ, GenVarOneHourOneUnit_base * t + OffsetSR_base, 				
						     SRCostandMaxSustainedRateData_Set.SR_MSR_CostCoefficient[i] + Lambda_SR_GENbase_k_slave[i * NT + t] 
                                                     - C_SR_GENbase_k_slave[i * NT + t] * (SR_GENbase_k_slave[i * NT + t] + SR_Systembase_k_slave[i * NT + t]));
  		    	if (error) goto QUIT_01;
        	    }


		    /* *build constraints for 1 generator in NT hours* */
		    int Quantity_Nonzero_Coeff;
		    //generator capacity limit
		    if (RelaxGENbase_capacity == 0)
		    { 
		        Quantity_Nonzero_Coeff = 2;
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
		    }
		    //generator startup/shutdown cost limit
		    if (RelaxGENbase_startshut == 0)
		    { 								
		       Quantity_Nonzero_Coeff = 3;
		       for (int t = 0; t < NT; ++t)
		       {
                    	   //startup cost limit
			   if (t == 0)
			   { 
                              ind[0] = GenVarOneHourOneUnit_base * t + OffsetI_base;
                              val[0] = GenData_Set.Gen_StartUp[i];
                              ind[1] = GenVarOneHourOneUnit_base * t + OffsetSUD_base;
                              val[1] = -1;
  		    	      error = GRBaddconstr(model, Quantity_Nonzero_Coeff-1, ind, val, GRB_LESS_EQUAL, GenData_Set.Gen_StartUp[i] * I_GENbase_Initial[i], NULL);
  		    	      if (error) goto QUIT_01;
			   }
			   if (t >= 1)
			   { 
                              ind[0] = GenVarOneHourOneUnit_base * t + OffsetI_base;
                              val[0] = GenData_Set.Gen_StartUp[i];
                              ind[1] = GenVarOneHourOneUnit_base * (t - 1) + OffsetI_base;
                              val[1] = (-1) * GenData_Set.Gen_StartUp[i];
                              ind[2] = GenVarOneHourOneUnit_base * t + OffsetSUD_base;
                              val[2] = -1;
  		    	      error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, 0, NULL);
  		    	      if (error) goto QUIT_01;
			   }
			   //shutdown cost limit
			   if (t == 0)
			   { 
                              ind[0] = GenVarOneHourOneUnit_base * t + OffsetI_base;
                              val[0] = (-1) * GenData_Set.Gen_ShutDown[i];
                              ind[1] = GenVarOneHourOneUnit_base * t + OffsetSUD_base;
                              val[1] = -1;
  		    	      error = GRBaddconstr(model, Quantity_Nonzero_Coeff-1, ind, val, GRB_LESS_EQUAL, (-1) * GenData_Set.Gen_ShutDown[i] * I_GENbase_Initial[i], NULL);
  		    	      if (error) goto QUIT_01;
			   }
			   if (t >= 1)
			   { 
                              ind[0] = GenVarOneHourOneUnit_base * t + OffsetI_base;
                              val[0] = (-1) * GenData_Set.Gen_ShutDown[i];
                              ind[1] = GenVarOneHourOneUnit_base * (t - 1) + OffsetI_base;
                              val[1] = GenData_Set.Gen_ShutDown[i];
                              ind[2] = GenVarOneHourOneUnit_base * t + OffsetSUD_base;
                              val[2] = -1;
  		    	      error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, 0, NULL);
  		    	      if (error) goto QUIT_01;
			   }
		       }
		    }
		    //generator ramping up/down limit
		    if (RelaxGENbase_ramping == 0)
		    { 												
		       Quantity_Nonzero_Coeff = 4;
		       for (int t = 0; t < NT; ++t)
		       {
                    	   //ramping up limit
			   if (t == 0)
			   { 			   
                              ind[0] = GenVarOneHourOneUnit_base * t + OffsetPG_base;
                              val[0] = 1;
                              ind[1] = GenVarOneHourOneUnit_base * t + OffsetI_base;
                              val[1] = GenData_Set.Gen_RampUp[i] + GenData_Set.Gen_Pmax[i] - GenData_Set.Gen_Pmin[i];
  		    	      error = GRBaddconstr(model, Quantity_Nonzero_Coeff-2, ind, val, GRB_LESS_EQUAL, GenData_Set.Gen_RampUp[i] + GenData_Set.Gen_Pmax[i] + PG_GENbase_Initial[i] 
                                      + (GenData_Set.Gen_RampUp[i] - GenData_Set.Gen_Pmin[i]) * I_GENbase_Initial[i], NULL);
  		    	      if (error) goto QUIT_01;
			   }
			   if (t >= 1)
			   { 
                              ind[0] = GenVarOneHourOneUnit_base * t + OffsetPG_base;
                              val[0] = 1;
                              ind[1] = GenVarOneHourOneUnit_base * (t - 1) + OffsetPG_base;
                              val[1] = -1;
                              ind[2] = GenVarOneHourOneUnit_base * t + OffsetI_base;
                              val[2] = GenData_Set.Gen_RampUp[i] + GenData_Set.Gen_Pmax[i] - GenData_Set.Gen_Pmin[i];
                              ind[3] = GenVarOneHourOneUnit_base * (t - 1) + OffsetI_base;
                              val[3] = GenData_Set.Gen_Pmin[i] - GenData_Set.Gen_RampUp[i];
  		    	      error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, GenData_Set.Gen_RampUp[i] + GenData_Set.Gen_Pmax[i], NULL);
  		    	      if (error) goto QUIT_01;
			   }
			   //ramping down limit
			   if (t == 0)
			   { 
                              ind[0] = GenVarOneHourOneUnit_base * t + OffsetPG_base;
                              val[0] = -1;
                              ind[1] = GenVarOneHourOneUnit_base * t + OffsetI_base;
                              val[1] = GenData_Set.Gen_Pmin[i] - GenData_Set.Gen_RampDown[i];
  		    	      error = GRBaddconstr(model, Quantity_Nonzero_Coeff-2, ind, val, GRB_LESS_EQUAL, GenData_Set.Gen_RampDown[i] + GenData_Set.Gen_Pmax[i] - PG_GENbase_Initial[i] 
                                      + (GenData_Set.Gen_Pmin[i] - GenData_Set.Gen_RampDown[i] - GenData_Set.Gen_Pmax[i]) * I_GENbase_Initial[i], NULL);
  		    	      if (error) goto QUIT_01;
			   }
			   if (t >= 1)
			   { 
                              ind[0] = GenVarOneHourOneUnit_base * t + OffsetPG_base;
                              val[0] = -1;
                              ind[1] = GenVarOneHourOneUnit_base * (t - 1) + OffsetPG_base;
                              val[1] = 1;
                              ind[2] = GenVarOneHourOneUnit_base * t + OffsetI_base;
                              val[2] = GenData_Set.Gen_Pmin[i] - GenData_Set.Gen_RampDown[i];
                              ind[3] = GenVarOneHourOneUnit_base * (t - 1) + OffsetI_base;
                              val[3] = GenData_Set.Gen_RampDown[i] + GenData_Set.Gen_Pmax[i] - GenData_Set.Gen_Pmin[i];
  		    	      error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, GenData_Set.Gen_RampDown[i] + GenData_Set.Gen_Pmax[i], NULL);
  		    	      if (error) goto QUIT_01;
			   }
		       }
		     }
		    //generator min ON/OFF limit
		    if (RelaxGENbase_minonoff == 0)
		    { 
		       int TimeStayon;
                       int TimeStayoff;
		       int Rightside_value;
		       int t3startpoint;
		       int temporaryvar_minon = GenData_Set.Gen_MinOn[i];
		       int temporaryvar_minoff = GenData_Set.Gen_MinOff[i];
        	       //Decdie whether generation unit can be startup/shutdown in 1st hour
		       if (GenData_Set.Gen_InitialOn[i] >= 1)
		       {
		          if (GenData_Set.Gen_MinOn[i] - GenData_Set.Gen_InitialOn[i] >= 1)
 		          {
		      	     TimeStayon = GenData_Set.Gen_MinOn[i] - GenData_Set.Gen_InitialOn[i];  //calculate how many hours unit need to stay On
		          }
		          else if (GenData_Set.Gen_MinOn[i] - GenData_Set.Gen_InitialOn[i] <= 0)
                          {
			      TimeStayon=0;
		          }
		          TimeStayoff=0;
		       }
		       else if (GenData_Set.Gen_InitialOn[i] <= 0)  //when:GenData_Set.Gen_InitialOn[i]<=0
		       {
		          if (GenData_Set.Gen_MinOff[i] - abs(GenData_Set.Gen_InitialOn[i]) >= 1)
                          {
		   	     TimeStayoff = GenData_Set.Gen_MinOff[i] - abs(GenData_Set.Gen_InitialOn[i]); //calculate how many hours unit need to stay Off
                          }
		          else if (GenData_Set.Gen_MinOff[i] - abs(GenData_Set.Gen_InitialOn[i]) <= 0)
                          {
			     TimeStayoff=0;
                          }
		          TimeStayon=0;
		       }
		       //1st Min ON Inequality Constraints (1st ON time period--1:TimeStayon)
		       if (TimeStayon >= 1)
		       {
		          if (TimeStayon >= NT)
		          {
		   	     TimeStayon=NT;
		          }
		          Quantity_Nonzero_Coeff = 1;
		          for (int t = 0; t < TimeStayon; ++t)
		          {
                              ind[0] = GenVarOneHourOneUnit_base * t + OffsetI_base;
                              val[0] = -1;
  		              error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, -1, NULL);
  		    	      if (error) goto QUIT_01;
		          }
		       }
		       //1st Min OFF Inequality Constraints (1st OFF time period--1:TimeStayoff)
		       if (TimeStayoff >= 1)
		       {
		          if (TimeStayoff >= NT)
		          {
		   	      TimeStayoff=NT;
		          }
		          Quantity_Nonzero_Coeff = 1;
		          for (int t = 0; t < TimeStayoff; ++t)
		          {
                              ind[0] = GenVarOneHourOneUnit_base * t + OffsetI_base;
                              val[0] = 1;
  		    	      error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, 0, NULL);
  		    	      if (error) goto QUIT_01;
		          }
		       }
		       //2nd Min ON Inequality Constraints (2nd ON time period--TimeStayon+1:NT-GenData_Set.Gen_MinOn+1,if it exists)        
		       if (NT - GenData_Set.Gen_MinOn[i] + 1 - TimeStayon >= 1)  //check whether 2nd time period exists
		       {
		          Quantity_Nonzero_Coeff = GenData_Set.Gen_MinOn[i] + 1;
		          for (int t = TimeStayon; t < NT - GenData_Set.Gen_MinOn[i] + 1; ++t)
		          {
			      for (int m = t; m < t + GenData_Set.Gen_MinOn[i]; ++m)
			      {
                                  ind[m - t] = GenVarOneHourOneUnit_base * m + OffsetI_base;
                                  val[m - t] = -1;
                              }
                              val[0] = GenData_Set.Gen_MinOn[i] - 1;
			      if (t == 0) //if 2nd time period starts from first hour
 			      {
			         Rightside_value = GenData_Set.Gen_MinOn[i] * I_GENbase_Initial[i];
  		    	         error = GRBaddconstr(model, Quantity_Nonzero_Coeff - 1, ind, val, GRB_LESS_EQUAL, Rightside_value, NULL);
  		    	         if (error) goto QUIT_01;
			      }
			      else if (t >= 1)
 			      {
                                 ind[temporaryvar_minon] = GenVarOneHourOneUnit_base * (t - 1) + OffsetI_base;
                                 val[temporaryvar_minon] = (-1) * GenData_Set.Gen_MinOn[i];
			         Rightside_value = 0;
  		    	         error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, Rightside_value, NULL);
  		    	         if (error) goto QUIT_01;
			      }
                          }
		       }
		       //2nd Min OFF Inequality Constraints (2nd OFF time period--TimeStayoff+1:NT-GenData_Set.Gen_MinOff+1,if it exists)
		       if (NT - GenData_Set.Gen_MinOff[i] + 1 - TimeStayoff >= 1)  //check whether 2nd time period exists
		       {
		          Quantity_Nonzero_Coeff = GenData_Set.Gen_MinOff[i] + 1;
		          for (int t = TimeStayoff; t < NT - GenData_Set.Gen_MinOff[i] + 1; ++t)
		          {
		   	      for (int m = t; m < t + GenData_Set.Gen_MinOff[i]; ++m)
			      {
                                  ind[m - t] = GenVarOneHourOneUnit_base * m + OffsetI_base;
                                  val[m - t] = 1;
                              }
                              val[0] = 1 - GenData_Set.Gen_MinOff[i];
			      if (t == 0) //if 2nd time period starts from first hour
 			      {
			         Rightside_value = GenData_Set.Gen_MinOff[i] - GenData_Set.Gen_MinOff[i] * I_GENbase_Initial[i];
  		       	         error = GRBaddconstr(model, Quantity_Nonzero_Coeff - 1, ind, val, GRB_LESS_EQUAL, Rightside_value, NULL);
  		    	         if (error) goto QUIT_01;
			      }
			      else if (t >= 1)
 			      {
                                 ind[temporaryvar_minoff] = GenVarOneHourOneUnit_base * (t - 1) + OffsetI_base;
                                 val[temporaryvar_minoff] = GenData_Set.Gen_MinOff[i];
			         Rightside_value = GenData_Set.Gen_MinOff[i];
  		    	         error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, Rightside_value, NULL);
  		    	         if (error) goto QUIT_01;
			      }
                          }
		       }
		       //3rd Min ON Inequality Constraints (3rd ON time period--NT-GenData_Set.Gen_MinOn+2:NT,if 2nd time period exists/--TimeStayon+1,if no)
		       if (NT - GenData_Set.Gen_MinOn[i] - TimeStayon + 1 <= 0)  //check whether 3rd time period exists
		       {
		          t3startpoint = TimeStayon;
		       }
		       else if (NT - GenData_Set.Gen_MinOn[i] - TimeStayon + 1 >= 1)  //check whether 3rd time period exists
		       {
		          t3startpoint = NT - GenData_Set.Gen_MinOn[i] + 1;
		       }
		       if (TimeStayon != NT && NT >= t3startpoint + 1)
		       {
		           for (int t = t3startpoint; t < NT; ++t)
			   {
		               Quantity_Nonzero_Coeff = NT - t + 1;
			       for (int m = t; m < NT; ++m)
			       {
                                   ind[m - t] = GenVarOneHourOneUnit_base * m + OffsetI_base;
                                   val[m - t] = -1;
		   	       }
                               val[0] = NT - t - 1;
			       if (t == 0) //if 3rd time period starts from first hour
 			       {
			         Rightside_value = (NT - t) * I_GENbase_Initial[i];
  		    	         error = GRBaddconstr(model, Quantity_Nonzero_Coeff - 1, ind, val, GRB_LESS_EQUAL, Rightside_value, NULL);
  		    	         if (error) goto QUIT_01;
			       }
			       else if (t >= 1)
 			       {
                                 ind[NT - t] = GenVarOneHourOneUnit_base * (t - 1) + OffsetI_base;
                                 val[NT - t] = (-1) * (NT - t);
			         Rightside_value = 0;
  		    	         error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, Rightside_value, NULL);
  		    	         if (error) goto QUIT_01;
			       }
		   	   }
		       }
		       //3rd Min Off Inequality Constraints (3rd OFF time period--NT-GenData_Set.Gen_MinOff+2:NT,if 2nd time period exists/--TimeStayoff+1,if no)
		       if (NT - GenData_Set.Gen_MinOff[i] - TimeStayoff + 1 <= 0)  //check whether 3rd time period exists
		       {
		          t3startpoint = TimeStayoff;
		       }
		       else if (NT - GenData_Set.Gen_MinOff[i] - TimeStayoff + 1 >= 1)  //check whether 3rd time period exists
		       {
		          t3startpoint = NT - GenData_Set.Gen_MinOff[i] + 1;
		       }
		       if (TimeStayoff != NT && NT >= t3startpoint + 1)
		       {
		           for (int t = t3startpoint; t < NT; ++t)
		   	   {
		               Quantity_Nonzero_Coeff = NT - t + 1;
			       for (int m = t; m < NT; ++m)
			       {
                                   ind[m - t] = GenVarOneHourOneUnit_base * m + OffsetI_base;
                                   val[m - t] = 1;
		   	       }
                               val[0] = (-1) * (NT - t) + 1;
			       if (t == 0) //if 3rd time period starts from first hour
 			       {
			          Rightside_value = (NT - t) * (1 - I_GENbase_Initial[i]);
  		    	          error = GRBaddconstr(model, Quantity_Nonzero_Coeff - 1, ind, val, GRB_LESS_EQUAL, Rightside_value, NULL);
  		    	          if (error) goto QUIT_01;
			       }
			       else if (t >= 1)
 			       {
                                 ind[NT - t] = GenVarOneHourOneUnit_base * (t - 1) + OffsetI_base;
                                 val[NT - t] = NT - t;
			         Rightside_value = NT - t;
  		    	         error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, Rightside_value, NULL);
  		    	         if (error) goto QUIT_01;
			       }
			   }
		       }
		    }


		    //SR constraint--max power output limit: pg+sr<=pgmax										
		    if (RelaxSRbase_maxoutput == 0)
		    { 	
		       Quantity_Nonzero_Coeff = 2;
		       for (int t = 0; t < NT; ++t)
		       {
                           ind[0] = GenVarOneHourOneUnit_base * t + OffsetPG_base;
                           val[0] = 1;
                           ind[1] = GenVarOneHourOneUnit_base * t + OffsetSR_base;
                           val[1] = 1;
  		    	   error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, GenData_Set.Gen_Pmax[i], NULL);
  		    	   if (error) goto QUIT_01;			
		       }
		    }
		    //SR constraint--max sustained rate limit:0<=sr<=10*msr*I									
		    if (RelaxSRbase_maxsustained == 0)
		    { 	
		       //lower bound
		       Quantity_Nonzero_Coeff = 1;
		       for (int t = 0; t < NT; ++t)
		       {
                           ind[0] = GenVarOneHourOneUnit_base * t + OffsetSR_base;
                           val[0] = -1;
  		    	   error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, 0, NULL);
  		    	   if (error) goto QUIT_01;			
		       }
		       //uppder bound
		       Quantity_Nonzero_Coeff = 2;
		       for (int t = 0; t < NT; ++t)
		       {
                           ind[0] = GenVarOneHourOneUnit_base * t + OffsetSR_base;
                           val[0] = 1;
                           ind[1] = GenVarOneHourOneUnit_base * t + OffsetI_base;
                           val[1] = (-1) * 10* SRCostandMaxSustainedRateData_Set.SR_MSR_MaxSustainedRate[i];
  		    	   error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, 0, NULL);
  		    	   if (error) goto QUIT_01;			
		       }
		    }


  		    /* *define variable types* */
    		    for (int t = 0; t < NT; ++t)
		    {
            		vtype[GenVarOneHourOneUnit_base * t + OffsetPG_base] = GRB_CONTINUOUS;
            		vtype[GenVarOneHourOneUnit_base * t + OffsetI_base] = GRB_BINARY; 
            		vtype[GenVarOneHourOneUnit_base * t + OffsetSUD_base] = GRB_CONTINUOUS;  
            		vtype[GenVarOneHourOneUnit_base * t + OffsetSR_base] = GRB_CONTINUOUS;  						      
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
            		SR_GENbase_k_slave[i * NT + t] = sol[GenVarOneHourOneUnit_base * t + OffsetSR_base];   						     
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
                SlaveNode_sendto_MasterNode(SR_GENbase_k_slave, id_mpi_gen_master, tag_mpi_gen, NG, NT);					


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
	//variable structure: PG1,PG2,...,PGng,/SR1,SR2,...,SRng,/PT1,PLswitchable1,...,PTnl_switchable,PLswitchablenl
	const int SystemVarOneHour_GENbase = NG;
	const int SystemVarOneHour_SRbase = NG;														
	const int SystemVarOneHourOneLine_TSbase = 2 * (1 - RelaxTSbase_Module);									
	const int SystemVarOneHour_TSbase = SystemVarOneHourOneLine_TSbase * NL_switchable;        							
	const int SystemVarOneHour_Totalbase = SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHour_TSbase;				


	/* ***define (constant) paremeter of total quantity of quadratic objective terms in a module*** */
	const int SystemQuadraticTermOneHour_GENbase = NG;												
	const int SystemQuadraticTermOneHour_SRbase = NG;												
	const int SystemQuadraticTermOneHour_TSbase = NL_switchable * 2 * (1 - RelaxTSbase_Module);							
        const int SystemQuadraticTermOneHour_Totalbase = SystemQuadraticTermOneHour_GENbase + SystemQuadraticTermOneHour_SRbase + SystemQuadraticTermOneHour_TSbase;	


	/* ***define (constant) offset of each variable in its smallest module*** */
    	const int OffsetPT_Systembase = 0;               												
    	const int OffsetPLswitchable_Systembase = 1;													


    	/* ***Define and initialize variables resulted from this slave load*** */
    	double PG_Systembase_k_slave[NG*NT];
    	double SR_Systembase_k_slave[NG*NT];														
    	for (int i = 0; i < NG * NT; ++i)
        {
            PG_Systembase_k_slave[i]=0;
            SR_Systembase_k_slave[i]=0; 														
        }
    	double PT_Systembase_k_slave[NL_switchable*NT];												
    	double PLswitchable_Systembase_k_slave[NL_switchable*NT];											
	for (int l = 0; l < NL_switchable * NT; ++l)												
        {
            PT_Systembase_k_slave[l]=0;
            PLswitchable_Systembase_k_slave[l]=0; 
        }


        /* ***define arrays to receive information from the master node*** */
        int stop_decision_i;
        double Lambda_PG_GENbase_k_slave[NG*NT]; 
        double C_PG_GENbase_k_slave[NG*NT]; 
        double PG_GENbase_k_slave[NG*NT];  
        double Lambda_SR_GENbase_k_slave[NG*NT]; 												
        double C_SR_GENbase_k_slave[NG*NT]; 														
        double SR_GENbase_k_slave[NG*NT]; 													
        double Lambda_PT_TSbase_k_slave[NL_switchable*NT]; 												
        double C_PT_TSbase_k_slave[NL_switchable*NT]; 													
        double PT_TSbase_k_slave[NL_switchable*NT]; 													
        double Lambda_PLswitchable_TSbase_k_slave[NL_switchable*NT]; 											
        double C_PLswitchable_TSbase_k_slave[NL_switchable*NT]; 											
        double PLswitchable_TSbase_k_slave[NL_switchable*NT]; 												


	/* ***set GUROBI environment and model*** */
	/* *define GUROBI environment parameter* */
  	static GRBenv   *env   = NULL;
  	static GRBmodel *model = NULL;
	/* *define GUROBI parameter* */
	//variable structure: PG1,PG2,...,PGng,/SR1,SR2,...,SRng,/PT1,PLswitchable1,...,PT_switchablenl,PLswitchablenl
  	int       error = 0;                			//store error inf
  	double    sol[SystemVarOneHour_Totalbase];   		//store optimization solutions
  	int       ind[SystemVarOneHour_Totalbase];		//store the indexes of variables in each constraint
  	double    val[SystemVarOneHour_Totalbase];		//store the coefficients of variables in each constraint
  	char      vtype[SystemVarOneHour_Totalbase];		//store the types of all the variables
  	int       qrow[SystemQuadraticTermOneHour_Totalbase];	//store the indexes of variables in the "Row" of all the quadratic terms of the objective function
  	int       qcol[SystemQuadraticTermOneHour_Totalbase];	//store the indexes of variables in the "Column" of all the quadratic terms of the objective function
  	double    qval[SystemQuadraticTermOneHour_Totalbase];	//store the coefficients of all the quadratic terms of the objective function
  	double    objval;					//store the final optimal result of the objective function
  	int       optimstatus;					//store the optimization information
  	double    lb[SystemVarOneHour_Totalbase];   		//store lower bounds of vars								
  	double    ub[SystemVarOneHour_Totalbase];   		//store upper bounds of vars								


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
            //printf("stop decision received at slave node %d is %d \n", tag_mpi_system, stop_decision_i);
            
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
                SlaveNode_receivefrom_MasterNode(Lambda_SR_GENbase_k_slave, id_mpi_system_master, tag_mpi_system, NG, NT);				
                SlaveNode_receivefrom_MasterNode(C_SR_GENbase_k_slave, id_mpi_system_master, tag_mpi_system, NG, NT);					
                SlaveNode_receivefrom_MasterNode(SR_GENbase_k_slave, id_mpi_system_master, tag_mpi_system, NG, NT);					
                SlaveNode_receivefrom_MasterNode(Lambda_PT_TSbase_k_slave, id_mpi_system_master, tag_mpi_system, NL_switchable, NT);			
                SlaveNode_receivefrom_MasterNode(C_PT_TSbase_k_slave, id_mpi_system_master, tag_mpi_system, NL_switchable, NT);			
                SlaveNode_receivefrom_MasterNode(PT_TSbase_k_slave, id_mpi_system_master, tag_mpi_system, NL_switchable, NT);				
                SlaveNode_receivefrom_MasterNode(Lambda_PLswitchable_TSbase_k_slave, id_mpi_system_master, tag_mpi_system, NL_switchable, NT);		
                SlaveNode_receivefrom_MasterNode(C_PLswitchable_TSbase_k_slave, id_mpi_system_master, tag_mpi_system, NL_switchable, NT);		
                SlaveNode_receivefrom_MasterNode(PLswitchable_TSbase_k_slave, id_mpi_system_master, tag_mpi_system, NL_switchable, NT);			


              	/* **calculate optiaml results by calling GUROBI** */
        	//Module_Systembase(); //call function to calculate
		for (int t = 0; t < NT; ++t) //repeatedly calculate optimal result of each hour
		{

  		    /* *Create an empty GUROBI model* */
  		    error = GRBnewmodel(env, &model, "System_Module", 0, NULL, NULL, NULL, NULL, NULL);
  		    if (error) goto QUIT_02;


  		    /* *Add GUROBI variables* */
  		    error = GRBaddvars(model, SystemVarOneHour_Totalbase, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);			
  		    if (error) goto QUIT_02;


		    /* *build objective function for 1 hour involving multiple units* */
		    //add quadratic objective terms
		    for (int i = 0; i < NG; ++i)													
		    {
                        qrow[i] = i;
                        qcol[i] = i;
                        qval[i] = C_PG_GENbase_k_slave[i * NT + t];
                        qrow[i +  SystemVarOneHour_GENbase] = i + SystemVarOneHour_GENbase;
                        qcol[i +  SystemVarOneHour_GENbase] = i + SystemVarOneHour_GENbase;
                        qval[i +  SystemVarOneHour_GENbase] = C_SR_GENbase_k_slave[i * NT + t];
        	    }
		    if (RelaxTSbase_Module == 0) //if TS base module is not relaxed									
		    {
		    	for (int l = 0; l < NL_switchable; ++l)												
		    	{
                            qrow[SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * l + OffsetPT_Systembase] 
                                = SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * l + OffsetPT_Systembase;
                            qcol[SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * l + OffsetPT_Systembase]
                                = SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * l + OffsetPT_Systembase;
                            qval[SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * l + OffsetPT_Systembase] 
                                = C_PT_TSbase_k_slave[l * NT + t];
                            qrow[SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * l + OffsetPLswitchable_Systembase]
                                = SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * l + OffsetPLswitchable_Systembase;
                            qcol[SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * l + OffsetPLswitchable_Systembase]
                                = SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * l + OffsetPLswitchable_Systembase;
                            qval[SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * l + OffsetPLswitchable_Systembase] 
                                = C_PLswitchable_TSbase_k_slave[l * NT + t];
		    	}			
        	    }
  		    error = GRBaddqpterms(model, SystemQuadraticTermOneHour_Totalbase, qrow, qcol, qval);
  		    if (error) goto QUIT_02;
  		    //add linear objective terms													
		    for (int i = 0; i < NG; ++i)
		    {
  		    	error = GRBsetdblattrelement(model, GRB_DBL_ATTR_OBJ, i, (-1) * (Lambda_PG_GENbase_k_slave[i * NT + t] 
                                                     + C_PG_GENbase_k_slave[i * NT + t] * (PG_GENbase_k_slave[i * NT + t] + PG_Systembase_k_slave[i * NT + t])));
  		    	if (error) goto QUIT_02;
  		    	error = GRBsetdblattrelement(model, GRB_DBL_ATTR_OBJ, i + SystemVarOneHour_GENbase, (-1) * (Lambda_SR_GENbase_k_slave[i * NT + t] 
                                                     + C_SR_GENbase_k_slave[i * NT + t] * (SR_GENbase_k_slave[i * NT + t] + SR_Systembase_k_slave[i * NT + t])));
  		    	if (error) goto QUIT_02;
        	    }
		    if (RelaxTSbase_Module == 0) //if TS base module is not relaxed									
		    {
		    	for (int l = 0; l < NL_switchable; ++l)												
		    	{
  		    	    error = GRBsetdblattrelement(model, GRB_DBL_ATTR_OBJ, SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * l + OffsetPT_Systembase, 
					(-1) * (Lambda_PT_TSbase_k_slave[l * NT + t] + C_PT_TSbase_k_slave[l * NT + t] * (PT_TSbase_k_slave[l * NT + t] + PT_Systembase_k_slave[l * NT + t])));
  		    	    if (error) goto QUIT_02;
  		    	    error = GRBsetdblattrelement(model, GRB_DBL_ATTR_OBJ, SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * l + OffsetPLswitchable_Systembase, 
					(-1) * (Lambda_PLswitchable_TSbase_k_slave[l * NT + t] + C_PLswitchable_TSbase_k_slave[l * NT + t] 
                                                * (PLswitchable_TSbase_k_slave[l * NT + t] + PLswitchable_Systembase_k_slave[l * NT + t])));
  		    	    if (error) goto QUIT_02;
		    	}			
        	    }


		    /* *build constraints for 1 hour involving multiple units* */
		    int Quantity_Nonzero_Coeff;
		    double Rightside_value;
		    int temporaryvar;
		    int temporaryvar_01;
		    int temporaryvar_02;
		    //hourly system SR requirement limit											
		    if (RelaxSystemSR_base == 0) 													
		    {
		       Quantity_Nonzero_Coeff = NG;
		       for (int i = 0; i < NG; ++i)
		       {
                           ind[i] = SystemVarOneHour_GENbase + i;
                           val[i] = -1;
		       }
  		       error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, (-1) * SRSystemRequirementData_Set.SRT_SpiningResreve[t], NULL); 
  		       if (error) goto QUIT_02;
		    }							
		    //power balance limit
		    if (RelaxPowerBalance_base == 0) 													
		    {
		       Quantity_Nonzero_Coeff = NG;
		       for (int i = 0; i < NG; ++i)
		       {
                           ind[i] = i;
                           val[i] = 1;
		       }
  		       error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_EQUAL, LoadForecastData_Set.LoadForecast_CorrectionLoad[t], "power_balance_limit"); 
  		       if (error) goto QUIT_02;
		    }
		    //switchable Line power flow limit:-PLmax<=PL<=PLmax									
		    if (RelaxTSbase_Module == 0 && RelaxPLswitchablelimit_base == 0) //if TS base module & this constraint is not relaxed		
		    {
		       Quantity_Nonzero_Coeff = 1;								
		       for (int l = 0; l < NL_switchable; ++l)											
		       {
                    	   //upper bound limit
                           ind[0] = SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * l + OffsetPLswitchable_Systembase;
                           val[0] = 1;
  		    	   error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, PLlimit_switchable[l], NULL);
  		    	   if (error) goto QUIT_02;
			   //lower bound limit
                           ind[0] = SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * l + OffsetPLswitchable_Systembase;
                           val[0] = -1;
  		           error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, PLlimit_switchable[l], NULL);
  		           if (error) goto QUIT_02;
		       }
		    } 
		    //switchable Line power flow equation:PL=SF*(PG+PT_positive+PT_negative-DL)								
		    if (RelaxTSbase_Module == 0 && RelaxPLswitchableEquation_base == 0) //if TS base module & this constraint is not relaxed
		    {
		       Quantity_Nonzero_Coeff = NG + NL_switchable + 1;				
		       for (int l = 0; l < NL_switchable; ++l)											
		       {
			   for (int i = 0; i < NG; ++i)
			   {
			       temporaryvar = GenData_Set.Gen_BusNum[i];
                               ind[i] = i;
                               val[i] = *(SF_switchable_PointerSet[l] + (temporaryvar - 1));								
			   }
		    	   for (int j = 0; j < NL_switchable; ++j)
			   {
			       temporaryvar_01 = Bus_withPT_positive[j];
			       temporaryvar_02 = Bus_withPT_negative[j];
                               ind[NG + j] = SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * j + OffsetPT_Systembase;
                               val[NG + j] = (*(SF_switchable_PointerSet[l] + (temporaryvar_01 - 1))) 
                                                 - (*(SF_switchable_PointerSet[l] + (temporaryvar_02 - 1)));					
			   }
                           ind[NG + NL_switchable] = SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * l + OffsetPLswitchable_Systembase;
                           val[NG + NL_switchable] = -1;
			   Rightside_value = 0; 
		    	   for (int i = 0; i < NB; ++i)
			   {
                               Rightside_value = Rightside_value + (*(SF_switchable_PointerSet[l] + i)) * LoadForecastData_Set.LoadForecast_CorrectionLoad[t] * LoadFactor_allbus[i];
			   }
  		           error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_EQUAL, Rightside_value, NULL);
  		           if (error) goto QUIT_02; 
		       }
		    } 
		    //unswitchable Line power flow limit											
		    if (NL_unswitchable >= 1) //if quantity of unswitchable lines is not 0								
		    {
		       Quantity_Nonzero_Coeff = NG + NL_switchable * (1 - RelaxTSbase_Module);								
		       for (int l = 0; l < NL_unswitchable; ++l) 										
		       {
                    	   //up limit
		    	   for (int i = 0; i < NG; ++i)
			   {
			       temporaryvar = GenData_Set.Gen_BusNum[i];
                               ind[i] = i;
                               val[i] = *(SF_unswitchable_PointerSet[l] + (temporaryvar - 1));							
			   }
		    	   if (RelaxTSbase_Module == 0) //if TS base module is not relaxed							
			   {
		    	       for (int j = 0; j < NL_switchable; ++j)
			       {
			           temporaryvar_01 = Bus_withPT_positive[j];
			           temporaryvar_02 = Bus_withPT_negative[j];
                                   ind[NG + j] = SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * j + OffsetPT_Systembase;
                                   val[NG + j] = (*(SF_unswitchable_PointerSet[l] + (temporaryvar_01 - 1))) 
                                                 - (*(SF_unswitchable_PointerSet[l] + (temporaryvar_02 - 1)));				
			       }
			   }
			   Rightside_value = 0; 
		    	   for (int i = 0; i < NB; ++i)
			   {
                               Rightside_value = Rightside_value + (*(SF_unswitchable_PointerSet[l] + i)) * LoadForecastData_Set.LoadForecast_CorrectionLoad[t] * LoadFactor_allbus[i];
			   }
			   Rightside_value = Rightside_value + PLlimit_unswitchable[l]; 
  		           error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, Rightside_value, NULL);
  		           if (error) goto QUIT_02;
                    	   //low limit
		    	   for (int i = 0; i < NG; ++i)
			   {
			       temporaryvar = GenData_Set.Gen_BusNum[i];
                               ind[i] = i;
                               val[i] = (*(SF_unswitchable_PointerSet[l] + (temporaryvar - 1))) * (-1);			    
			   }
			   if (RelaxTSbase_Module == 0) //if TS base module is not relaxed							
			   {
		    	       for (int j = 0; j < NL_switchable; ++j)
			       {
			           temporaryvar_01 = Bus_withPT_positive[j];
			           temporaryvar_02 = Bus_withPT_negative[j];
                                   ind[NG + j] = SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * j + OffsetPT_Systembase;
                                   val[NG + j] = (-1) * (*(SF_unswitchable_PointerSet[l] + (temporaryvar_01 - 1))) 
                                                 + *(SF_unswitchable_PointerSet[l] + (temporaryvar_02 - 1));				
			       }
			   }
			   Rightside_value = 0; 
		    	   for (int i = 0; i < NB; ++i)
			   {
                               Rightside_value = Rightside_value + (*(SF_unswitchable_PointerSet[l] + i)) * LoadForecastData_Set.LoadForecast_CorrectionLoad[t] * LoadFactor_allbus[i];
			   }
			   Rightside_value = (-1) * Rightside_value + PLlimit_unswitchable[l];
  		           error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, Rightside_value, NULL); 
  		           if (error) goto QUIT_02; 
		       } 
		    } 


  		    /* *define variable lower & upper bounds* */										
    		    for (int i = 0; i < NG + NG; ++i)									
		    {
            		lb[i] = 0;
            		ub[i] = GRB_INFINITY;         
        	    }
    		    for (int l = 0; l < NL_switchable; ++l)									
		    {
            		lb[SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * l + OffsetPT_Systembase] = 0;
            		ub[SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * l + OffsetPT_Systembase] = GRB_INFINITY;
            		lb[SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * l + OffsetPLswitchable_Systembase] = -GRB_INFINITY;
            		ub[SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * l + OffsetPLswitchable_Systembase] = GRB_INFINITY;         
        	    }
  		    error = GRBsetdblattrarray(model, "LB", 0, SystemVarOneHour_Totalbase, lb);				
  		    if (error) goto QUIT_02;
  		    error = GRBsetdblattrarray(model, "UB", 0, SystemVarOneHour_Totalbase, ub);				
  		    if (error) goto QUIT_02;


  		    /* *define variable types* */
    		    for (int i = 0; i < SystemVarOneHour_Totalbase; ++i)								
		    {
            		vtype[i] = GRB_CONTINUOUS;         
        	    }
  		    error = GRBsetcharattrarray(model, GRB_CHAR_ATTR_VTYPE, 0, SystemVarOneHour_Totalbase, vtype);			
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
  		    error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, SystemVarOneHour_Totalbase, sol);				
  		    if (error) goto QUIT_02;

  		    if (optimstatus == GRB_OPTIMAL) 
		    {
  		    	printf("Optimization is completed & feasible\n");
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
            		SR_Systembase_k_slave[i * NT + t] = sol[SystemVarOneHour_GENbase + i];						
        	    }
		    if (RelaxTSbase_Module == 0) //if TS base module is not relaxed							
		    {
		    	for (int i = 0; i < NL_switchable; ++i)												
		    	{
            		    PT_Systembase_k_slave[i * NT + t] = sol[SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * i + OffsetPT_Systembase];
            		    PLswitchable_Systembase_k_slave[i * NT + t] = sol[SystemVarOneHour_GENbase + SystemVarOneHour_SRbase + SystemVarOneHourOneLine_TSbase * i + OffsetPLswitchable_Systembase];
		    	}			
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
                SlaveNode_sendto_MasterNode(SR_Systembase_k_slave, id_mpi_system_master, tag_mpi_system, NG, NT);				
                SlaveNode_sendto_MasterNode(PT_Systembase_k_slave, id_mpi_system_master, tag_mpi_system, NL_switchable, NT);		
                SlaveNode_sendto_MasterNode(PLswitchable_Systembase_k_slave, id_mpi_system_master, tag_mpi_system, NL_switchable, NT);		

                
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




    /*************************************************************************Slave-node-03's work in parallel manner START*************************************************************************/
    /*************************************************************************Slave-node-03's work in parallel manner START*************************************************************************/
    /*************************************************************************Slave-node-03's work in parallel manner START*************************************************************************/
    if (id_mpi == 3)
    { 

        //the following codes are only run once in a whole simulation process***********************************************************************************************
        /* ***time the simulation time of the slave node*** */
        wtime_mpi = MPI_Wtime( );


	/* ***define (constant) paremeter of variable quantity in certain module*** */
	//variable structure: [PT, PLswitchable, Z] *NT
	const int TSVarOneHourOneLine_base = 3;											
	const int TSVarOneLine_base = TSVarOneHourOneLine_base * NT;


	/* ***define (constant) paremeter of total quantity of quadratic objective terms in a module*** */
	const int TSQuadraticTermOneLine_base = 2* NT;											


	/* ***define (constant) offset of each variable in its smallest module*** */
	const int OffsetPT_base = 0; //should start from 0
	const int OffsetPLswitchable_base = 1;
	const int OffsetZ_base = 2;


    	/* ***Define and initialize variables resulted from this slave load*** */
    	double PT_TSbase_k_slave[NL_switchable*NT];
    	double PLswitchable_TSbase_k_slave[NL_switchable*NT];
    	double Z_TSbase_k_slave[NL_switchable*NT];
    	for (int i = 0; i < NL_switchable*NT; ++i)
	{
            PT_TSbase_k_slave[i]=0;
            PLswitchable_TSbase_k_slave[i]=0; 
            Z_TSbase_k_slave[i]=0; 
        }


        /* ***define arrays to receive information from the master node*** */
        int    stop_decision_i;
        double Lambda_PT_TSbase_k_slave[NL_switchable * NT];
        double C_PT_TSbase_k_slave[NL_switchable * NT];      
        double PT_Systembase_k_slave[NL_switchable * NT];
        double Lambda_PLswitchable_TSbase_k_slave[NL_switchable * NT];											
        double C_PLswitchable_TSbase_k_slave[NL_switchable * NT];      											
        double PLswitchable_Systembase_k_slave[NL_switchable * NT];    											


	/* ***set GUROBI environment and model*** */
	/* *define GUROBI environment parameter* */
  	static GRBenv   *env   = NULL;
  	static GRBmodel *model = NULL;
	/* *define GUROBI parameter* */
	//variable structure: [PG, I, SUD] *NT
  	int       error = 0;                		//store error inf
  	double    sol[TSVarOneLine_base];   		//store optimization solutions
  	int       ind[TSVarOneLine_base];		//store the indexes of variables in each constraint
  	double    val[TSVarOneLine_base];		//store the coefficients of variables in each constraint
  	char      vtype[TSVarOneLine_base];		//store the types of all the variables
  	int       qrow[TSQuadraticTermOneLine_base];	//store the indexes of variables in the "Row" of all the quadratic terms of the objective function
  	int       qcol[TSQuadraticTermOneLine_base];	//store the indexes of variables in the "Column" of all the quadratic terms of the objective function
  	double    qval[TSQuadraticTermOneLine_base];	//store the coefficients of all the quadratic terms of the objective function
  	double    objval;				//store the final optimal result of the objective function
  	int       optimstatus;				//store the optimization information
  	double    lb[TSVarOneLine_base];   		//store lower bounds of vars								
  	double    ub[TSVarOneLine_base];   		//store upper bounds of vars							


	/* *create GUROBI environment* */
  	error = GRBloadenv(&env, "TS_Module.log");
  	if (error) goto QUIT_03;		    


        /* ***define variable to count iterative number*** */
        int k_slave_03=1; //count the iterative number of slave node
        //the above codes are only run once in a whole simulation process**********************************************************************************************
        

        /* ***loop (run multiple times) to reveive information, calculate optimal result, send information*** */ 
        while (1)
        {

            SlaveNode_receive_stopdecision(&stop_decision_i, id_mpi_ts_master, tag_mpi_ts); //receive convergence information to judge whether to continue
            printf("stop decision received at slave node %d is %d \n", tag_mpi_ts, stop_decision_i);
          
            if (stop_decision_i == 0) //=0, don't stop & go on (because not converge)
            {
    		printf ("*************************************************************************************************************Slave-03 (k=%d) START\n",k_slave_03);


                /* **time the simulation time of current iteration** */
                //wtime_mpi = MPI_Wtime ( ); //start to time
                //printf("the loop is no error at this moment at process %d\n", id_mpi_gen_master);
                

                /* **receive information from the master node** */
                SlaveNode_receivefrom_MasterNode(Lambda_PT_TSbase_k_slave, id_mpi_ts_master, tag_mpi_ts, NL_switchable, NT);
                SlaveNode_receivefrom_MasterNode(C_PT_TSbase_k_slave, id_mpi_ts_master, tag_mpi_ts, NL_switchable, NT);
                SlaveNode_receivefrom_MasterNode(PT_Systembase_k_slave, id_mpi_ts_master, tag_mpi_ts, NL_switchable, NT);
                SlaveNode_receivefrom_MasterNode(Lambda_PLswitchable_TSbase_k_slave, id_mpi_ts_master, tag_mpi_ts, NL_switchable, NT);				
                SlaveNode_receivefrom_MasterNode(C_PLswitchable_TSbase_k_slave, id_mpi_ts_master, tag_mpi_ts, NL_switchable, NT);					
                SlaveNode_receivefrom_MasterNode(PLswitchable_Systembase_k_slave, id_mpi_ts_master, tag_mpi_ts, NL_switchable, NT); 				
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/                    
                			//for (int i = 0; i < NL_switchable*NT; ++i){
            	    			//     printf("slave 03 get from master, PLswitchable_Systembase_k_slave[%d] = %f", i, PLswitchable_Systembase_k_slave[i]); 
            	        		//     printf("\n"); 
    	        			//}
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/  


                /* **calculate optiaml results by calling GUROBI** */
        	//Module_GENbase(); //call function to calculate

		for (int l = 0; l < NL_switchable; ++l) //repeatedly calculate optimal result of each unit
		{

  		    /* *Create an empty GUROBI model* */
  		    error = GRBnewmodel(env, &model, "TS_Module", 0, NULL, NULL, NULL, NULL, NULL);
  		    if (error) goto QUIT_03;


  		    /* *Add GUROBI variables* */
  		    error = GRBaddvars(model, TSVarOneLine_base, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  		    if (error) goto QUIT_03;


		    /* *build objective function for 1 switchable line in NT hours* */
		    //add quadratic objective terms
		    for (int t = 0; t < NT; ++t)									
		    {
                           qrow[2 * t] = TSVarOneHourOneLine_base * t + OffsetPT_base;
                           qcol[2 * t] = TSVarOneHourOneLine_base * t + OffsetPT_base; 
                           qval[2 * t] = C_PT_TSbase_k_slave[l * NT + t];							
                           qrow[2 * t + 1] = TSVarOneHourOneLine_base * t + OffsetPLswitchable_base;
                           qcol[2 * t + 1] = TSVarOneHourOneLine_base * t + OffsetPLswitchable_base; 
                           qval[2 * t + 1] = C_PLswitchable_TSbase_k_slave[l * NT + t];										
        	    }
  		    error = GRBaddqpterms(model, TSQuadraticTermOneLine_base, qrow, qcol, qval);
  		    if (error) goto QUIT_03;
  		    //add linear objective terms
		    for (int t = 0; t < NT; ++t)
		    {
  		    	error = GRBsetdblattrelement(model, GRB_DBL_ATTR_OBJ, TSVarOneHourOneLine_base * t + OffsetPT_base, 
                                                     Lambda_PT_TSbase_k_slave[l * NT + t] 
                                                     - C_PT_TSbase_k_slave[l * NT + t] * (PT_TSbase_k_slave[l * NT + t] + PT_Systembase_k_slave[l * NT + t]));
  		    	if (error) goto QUIT_03;
  		    	error = GRBsetdblattrelement(model, GRB_DBL_ATTR_OBJ, TSVarOneHourOneLine_base * t + OffsetPLswitchable_base, 				
						     Lambda_PLswitchable_TSbase_k_slave[l * NT + t] 
                                                     - C_PLswitchable_TSbase_k_slave[l * NT + t] * (PLswitchable_TSbase_k_slave[l * NT + t] + PLswitchable_Systembase_k_slave[l * NT + t]));
  		    	if (error) goto QUIT_03;
        	    }


		    /* *build constraints for 1 switchable line in NT hours* */
		    int Quantity_Nonzero_Coeff;
		    //Virtual Generator Capacity Limit
		    if (RelaxTSbase_PTcapacity == 0)
		    {
		       Quantity_Nonzero_Coeff = 2;
		       for (int t = 0; t < NT; ++t)
		       {
                    	   //upper bound limit
                           ind[0] = TSVarOneHourOneLine_base * t + OffsetPT_base;
                           val[0] = 1;
                           ind[1] = TSVarOneHourOneLine_base * t + OffsetZ_base;
                           val[1] = M_limit;
  		    	   error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, M_limit, NULL);
  		    	   if (error) goto QUIT_03;
			   //lower bound limit
                           ind[0] = TSVarOneHourOneLine_base * t + OffsetPT_base;
                           val[0] = -1;
                           ind[1] = TSVarOneHourOneLine_base * t + OffsetZ_base;
                           val[1] = M_limit;
  		    	   error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, M_limit, NULL);
  		           if (error) goto QUIT_03;
		       }
		    }
		    //Power flow Limit
		    if (RelaxTSbase_PLlimit == 0)
		    {
		       Quantity_Nonzero_Coeff = 2;
		       for (int t = 0; t < NT; ++t)
		       {
                    	   //upper bound limit
                           ind[0] = TSVarOneHourOneLine_base * t + OffsetPLswitchable_base;
                           val[0] = 1;
                           ind[1] = TSVarOneHourOneLine_base * t + OffsetZ_base;
                           val[1] = M_limit - PLlimit_switchable[l];
  		    	   error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, M_limit, NULL);
  		    	   if (error) goto QUIT_03;
			   //lower bound limit
                           ind[0] = TSVarOneHourOneLine_base * t + OffsetPLswitchable_base;
                           val[0] = -1;
                           ind[1] = TSVarOneHourOneLine_base * t + OffsetZ_base;
                           val[1] = M_limit - PLlimit_switchable[l];
  		    	   error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, M_limit, NULL);
  		           if (error) goto QUIT_03;
		       }
		    }
		    //elationship between Virtual Generator Capacity and Power Flow
		    if (RelaxTSbase_PT2PLrelation == 0)
		    {
		       Quantity_Nonzero_Coeff = 3;
		       for (int t = 0; t < NT; ++t)
		       {
                    	   //upper bound limit
                           ind[0] = TSVarOneHourOneLine_base * t + OffsetPT_base;
                           val[0] = 1;
                           ind[1] = TSVarOneHourOneLine_base * t + OffsetPLswitchable_base;
                           val[1] = -1;
                           ind[2] = TSVarOneHourOneLine_base * t + OffsetZ_base;
                           val[2] = (-1) * M_limit;
  		    	   error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, 0, NULL);
  		    	   if (error) goto QUIT_03;
			   //lower bound limit
                           ind[0] = TSVarOneHourOneLine_base * t + OffsetPT_base;
                           val[0] = -1;
                           ind[1] = TSVarOneHourOneLine_base * t + OffsetPLswitchable_base;
                           val[1] = 1;
                           ind[2] = TSVarOneHourOneLine_base * t + OffsetZ_base;
                           val[2] = (-1) * M_limit;
  		    	   error = GRBaddconstr(model, Quantity_Nonzero_Coeff, ind, val, GRB_LESS_EQUAL, 0, NULL);
  		           if (error) goto QUIT_03;
		       } 
		    } 


  		    /* *define variable lower & upper bounds* */										
    		    for (int t = 0; t < NT; ++t)
		    {
            		lb[TSVarOneHourOneLine_base * t + OffsetPT_base] = 0;
            		lb[TSVarOneHourOneLine_base * t + OffsetPLswitchable_base] = -GRB_INFINITY;
            		lb[TSVarOneHourOneLine_base * t + OffsetZ_base] = 0;   
            		ub[TSVarOneHourOneLine_base * t + OffsetPT_base] = GRB_INFINITY;
            		ub[TSVarOneHourOneLine_base * t + OffsetPLswitchable_base] = GRB_INFINITY;
            		ub[TSVarOneHourOneLine_base * t + OffsetZ_base] = 1;
        	    }
  		    error = GRBsetdblattrarray(model, "LB", 0, TSVarOneLine_base, lb);				
  		    if (error) goto QUIT_03;
  		    error = GRBsetdblattrarray(model, "UB", 0, TSVarOneLine_base, ub);				
  		    if (error) goto QUIT_03;


  		    /* *define variable types* */
    		    for (int t = 0; t < NT; ++t)
		    {
            		vtype[TSVarOneHourOneLine_base * t + OffsetPT_base] = GRB_CONTINUOUS;
            		vtype[TSVarOneHourOneLine_base * t + OffsetPLswitchable_base] = GRB_CONTINUOUS;
            		vtype[TSVarOneHourOneLine_base * t + OffsetZ_base] = GRB_BINARY;   
        	    }
  		    error = GRBsetcharattrarray(model, GRB_CHAR_ATTR_VTYPE, 0, TSVarOneLine_base, vtype);
  		    if (error) goto QUIT_03;


		    /* *update model* */
		    error = GRBupdatemodel(model);
  		    if (error) goto QUIT_03;


		    /* *call GUROBI solver to conduct optimization calculation* */
  		    error = GRBoptimize(model);
  		    if (error) goto QUIT_03;


		    /* *write model to 'GEN_Module.lp/.sol'* */
  		    error = GRBwrite(model, "TS_Module.lp");
  		    error = GRBwrite(model, "TS_Module.sol");
  		    if (error) goto QUIT_03;


		    /* capture solution information */
  		    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
  		    if (error) goto QUIT_03;
  		    error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
  		    if (error) goto QUIT_03;
  		    error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, TSVarOneLine_base, sol);
  		    if (error) goto QUIT_03;

  		    if (optimstatus == GRB_OPTIMAL) 
		    {
  		    	printf("Optimization is completed & feasible\n");
  		    } 
		    else if (optimstatus == GRB_INF_OR_UNBD) 
		    {
    		        printf("Model is infeasible or unbounded\n");
  		    } 
		    else 
  		    {
    		        printf("Optimization was stopped early\n");
  		    }


		    /* *sotre optimization solution of each switchable line* */
    		    for (int t = 0; t < NT; ++t)
		    {
            		PT_TSbase_k_slave[l * NT + t] = sol[TSVarOneHourOneLine_base * t + OffsetPT_base];
            		PLswitchable_TSbase_k_slave[l * NT + t] = sol[TSVarOneHourOneLine_base * t + OffsetPLswitchable_base];
            		Z_TSbase_k_slave[l * NT + t] = sol[TSVarOneHourOneLine_base * t + OffsetZ_base]; 
        	    }


  		    /* *Free GUROBI model* */
  		    GRBfreemodel(model);

	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/                    
            				printf("slave 03 in %d iteration for switchable line %d*******************************************************************************",k_slave_03, l); 
            				printf("\n"); 
	    				/*FOR TESTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

                 }//END:for(int l = 0; l < NL_switchable; ++l)


                /* **send information to the master node** */
                SlaveNode_sendto_MasterNode(PT_TSbase_k_slave, id_mpi_ts_master, tag_mpi_ts, NL_switchable, NT);
                SlaveNode_sendto_MasterNode(PLswitchable_TSbase_k_slave, id_mpi_ts_master, tag_mpi_ts, NL_switchable, NT);						


                /* **time the simulation time of current iteration** */
                //wtime_mpi = MPI_Wtime ( ) - wtime_mpi;
                //printf ( "Slave node %d's simulation time in the iteration %d = %g\n",id_mpi, wtime_mpi, k_slave_03);


                /* **update the counter** */
                k_slave_03++;//count the iterative number of slave 01


    		printf ("*************************************************************************************************************Slave-03 (k=%d) END\n",k_slave_03-1); 
            }//END:(stop_decision_i == 0)
            
            else //if converged,just terminate
            {
                //printf("MPI communication will terminate, because the convergence is down.\n");
                ierr_mpi = MPI_Finalize ( );//All processes must call this routine before exiting. All processes will still exist but may not make any further MPI calls.
                break; //if converged, jumpe out of the "while: loop and terminate
            }//END:else

        }//END:while(1)


  	/* ***if GUROBI error, just stop and report error*** */
	QUIT_03:
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
    /*************************************************************************Slave-node-03's work in parallel manner END*************************************************************************/
    /*************************************************************************Slave-node-03's work in parallel manner END*************************************************************************/
    /*************************************************************************Slave-node-03's work in parallel manner END*************************************************************************/




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





