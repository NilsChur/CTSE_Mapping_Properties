/* 
 * Copyright 2013-2018 The MathWorks, Inc.
 *
 * File: rt_cppclass_main.cpp
 *
 * Abstract:
 *
 *   A real-time main that runs generated Simulink Coder code under most 
 *   operating systems.  Based on the definition of MULTITASKING, a single-task 
 *   or multitask step function is employed.
 *
 *   This file is a useful starting point for creating a custom main when 
 *   targeting a custom floating point processor or integer micro-controller.
 *
 *   Alternatively for ERT targets, you can generate a sample ert_main.cpp file 
 *   with the generated code by selecting the "Generate an example main program"
 *   option.  In this case, ert_main.cpp is precisely customized to the
 *   model requirements.  
 *
 * Required Defines:
 *
 *   MODEL - Model name
 *   NUMST - Number of sample times
 *
 */

/*==================*
 * Required defines *
 *==================*/

#ifndef MODEL
# error Must specify a model name.  Define MODEL=name.
#else
/* create generic macros that work with any model */
# define EXPAND_CONCAT(name1,name2) name1 ## name2
# define CONCAT(name1,name2) EXPAND_CONCAT(name1,name2)
# define MODEL_INSTANCE   CONCAT(MODEL,_Obj)
#endif

#ifndef NUMST
# error Must specify the number of sample times.  Define NUMST=number.
#endif

#if CLASSIC_INTERFACE == 1
# error "Classic call interface is not supported by rt_cppclass_main.cpp."
#endif

#if ONESTEPFCN==0
#error Separate output and update functions are not supported by rt_cppclass_main.cpp. \
You must update rt_cppclass_main.cpp to suit your application needs, or select \
the 'Single output/update function' option.
#endif

#if TERMFCN==0
#error The terminate function is required by rt_cppclass_main.cpp. \
You must update rt_cppclass_main.cpp to suit your application needs, or select \
the 'Terminate function required' option.
#endif

#define QUOTE1(name) #name
#define QUOTE(name) QUOTE1(name)    /* need to expand name    */

#ifndef SAVEFILE
# define MATFILE2(file) #file ".mat"
# define MATFILE1(file) MATFILE2(file)
# define MATFILE MATFILE1(MODEL)
#else
# define MATFILE QUOTE(SAVEFILE)
#endif

/*==========*
 * Includes *
 *==========*/
#include "Velocity_Controlled_Vehicle.h"
#include <iostream>

// For DIVINE
#include <dios.h>
#include <sys/divm.h>
#include <stdbool.h>
#include <map>

#include "rtwtypes.h"
#if !defined(INTEGER_CODE) || INTEGER_CODE == 0
# include <stdio.h>    /* optional for printf */
#else
extern "C" {
  extern int printf(const char *, ...); 
  extern int fflush(void *);
}
#endif
#include <string.h>  /* optional for strcmp */
#include "rtmodel.h"  /* includes model.h */

#include "rt_logging.h"
#ifdef UseMMIDataLogging
#include "rt_logging_mmi.h"
#endif

#include "ext_work.h"

#ifdef MODEL_STEP_FCN_CONTROL_USED
#error rt_cppclass_main.cpp does not support model step function prototype control.
#endif

/*========================* 
 * Setup for multitasking * 
 *========================*/

/* 
 * Let MT be synonym for MULTITASKING (to shorten command line for DOS) 
 */
#if defined(MT)
# if MT == 0
# undef MT
# else
# define MULTITASKING 1
# endif
#endif

#if defined(TID01EQ) && TID01EQ == 1
#define FIRST_TID 1
#else 
#define FIRST_TID 0
#endif

/*====================*
 * External functions *
 *====================*/

#if !defined(MULTITASKING)
 /* single-rate step function */
 #define MODEL_STEP(obj) obj.MODEL_STEPNAME()
#else
 /* multirate step function */
 #define MODEL_STEP MODEL_STEPNAME
#endif


/*==================================*
 * Global data local to this module *
 *==================================*/
#ifndef MULTITASKING
static boolean_T OverrunFlags[1];    /* ISR overrun flags */
static boolean_T eventFlags[1];      /* necessary for overlapping preemption */
#else
static boolean_T OverrunFlags[NUMST];
static boolean_T eventFlags[NUMST]; 
#endif

/* Create model instance */
static MODEL_CLASSNAME MODEL_INSTANCE;

/*===================*
 * Visible functions *
 *===================*/

#if !defined(MULTITASKING) /* single task */

/* Function: rtOneStep ========================================================
 *
 * Abstract:
 *   Perform one step of the model.  This function is modeled such that
 *   it could be called from an interrupt service routine (ISR) with minor
 *   modifications.
 */
static void rt_OneStep(MODEL_CLASSNAME & mdl)
{
    /* Disable interrupts here */

    /***********************************************
     * Check and see if base step time is too fast *
     ***********************************************/
    if (OverrunFlags[0]++) {
        rtmSetErrorStatus(mdl.getRTM(), "Overrun");
    }

    /*************************************************
     * Check and see if an error status has been set *
     * by an overrun or by the generated code.       *
     *************************************************/
    if (rtmGetErrorStatus(mdl.getRTM()) != NULL) {
        return;
    }

    /* Save FPU context here (if necessary) */
    /* Re-enable interrupts here */
    /* Set model inputs here */

    /**************
     * Step model *
     **************/
    MODEL_STEP(mdl);

    /* Get model outputs here */

    /**************************
     * Decrement overrun flag *
     **************************/
    OverrunFlags[0]--;

    rtExtModeCheckEndTrigger();

    /* Disable interrupts here */
    /* Restore FPU context here (if necessary) */
    /* Re-enable interrupts here */

} /* end rtOneStep */

#else /* multitask */

/* Function: rtOneStep ========================================================
 *
 * Abstract:
 *   Perform one step of the model.  This function is modeled such that
 *   it could be called from an interrupt service routine (ISR) with minor
 *   modifications.
 *
 *   Note that error checking is only necessary when this routine is
 *   attached to an interrupt.
 *
 *   Also, you may wish to unroll any or all of for and while loops to
 *   improve the real-time performance of this function.
 */
static void rt_OneStep(MODEL_CLASSNAME & mdl)
{
    int_T i;

    /* Disable interrupts here */

    /***********************************************
     * Check and see if base step time is too fast *
     ***********************************************/
    if (OverrunFlags[0]++) {
        rtmSetErrorStatus(mdl.getRTM(), "Overrun");
    }

    /*************************************************
     * Check and see if an error status has been set *
     * by an overrun or by the generated code.       *
     *************************************************/
    if (rtmGetErrorStatus(mdl.getRTM()) != NULL) {
        return;
    }

    /* Save FPU context here (if necessary) */
    /* Re-enable interrupts here */
    
    /*************************************************
     * Update EventFlags and check subrate overrun   *
     *************************************************/
    for (i = FIRST_TID+1; i < NUMST; i++) {
        if (rtmStepTask(mdl.getRTM(),i) && eventFlags[i]++) {
            OverrunFlags[0]--;
            OverrunFlags[i]++;
            /* Sampling too fast */
            rtmSetErrorStatus(mdl.getRTM(), "Overrun");
            return;
        }
        if (++rtmTaskCounter(mdl.getRTM(),i) == rtmCounterLimit(mdl.getRTM(),i))
            rtmTaskCounter(mdl.getRTM(), i) = 0;
    }

    /* Set model inputs associated with base rate here */

    /*******************************************
     * Step the model for the base sample time *
     *******************************************/
    MODEL_STEP(mdl,0);

    /* Get model outputs associated with base rate here */

    /************************************************************************
     * Model step complete for base sample time, now it is okay to          *
     * re-interrupt this ISR.                                               *
     ************************************************************************/
    OverrunFlags[0]--;

    /*********************************************************
     * Step the model for any other sample times (subrates) *
     *********************************************************/
    for (i = FIRST_TID+1; i < NUMST; i++) {
        /*************************************************************
         * If task "i" is running, don't run any lower priority task *
         *************************************************************/
        if (OverrunFlags[i]) return; 

        if (eventFlags[i]) {
            OverrunFlags[i]++;

            /* Set model inputs associated with subrate here */

            /******************************************
             * Step the model for sample time "i" *
             ******************************************/
            MODEL_STEP(mdl,i);

            /* Get model outputs associated with subrate here */
            
            /**********************************************
             * Indicate task complete for sample time "i" *
             **********************************************/
            OverrunFlags[i]--;
            eventFlags[i]--;
        }
    }

    rtExtModeCheckEndTrigger();
    /* Disable interrupts here */
    /* Restore FPU context here (if necessary) */
    /* Enable interrupts here */

} /* end rtOneStep */

#endif /* MULTITASKING */

/* Function: rt_InitModel ====================================================
 * 
 * Abstract: 
 *   Initialized the model and the overrun flags
 *
 */
static void rt_InitModel(MODEL_CLASSNAME & mdl)
{
#if defined(MULTITASKING)
    int i;
    for(i=0; i < NUMST; i++) {
        OverrunFlags[i] = 0;
        eventFlags[i] = 0;
    }
#else
    OverrunFlags[0] = 0;
    eventFlags[0] = 0;
#endif

    /************************
     * Initialize the model *
     ************************/
    mdl.initialize();
}

/* Function: rt_TermModel ====================================================
 * 
 * Abstract:
 *   Terminates the model and prints the error status
 *
 */
static int_T rt_TermModel(MODEL_CLASSNAME & mdl)
{
    mdl.terminate();
    
    {
        const char_T *errStatus = (const char_T *) (rtmGetErrorStatus(mdl.getRTM()));
        
        if (errStatus != NULL && strcmp(errStatus, "Simulation finished")) {
            (void)printf("%s\n", errStatus);
#if defined(MULTITASKING)
            int_T i;
            for (i = 0; i < NUMST; i++) {
                if (OverrunFlags[i]) {
                    (void)printf("ISR overrun - sampling rate too"
                                 "fast for sample time index %d.\n", i);
                }
            }
#else
            if (OverrunFlags[0]) {
                    (void)printf("ISR overrun - base sampling rate too fast.\n");
                }
#endif
            
            return(1);
        }
    }
    
    return(0);
}

/*
 * 
 * In the following section the büchi automata will be implemented with are constructed via the Spot online tool
 * To get the boolen if a state was visited the Observer itself provied a method for each state, here a short listing:
 * - (...).ObserverFSM.wasRestVisited(); return a boolen if the state Rest was visited
 * - (...).ObserverFSM.wasTransientVisited(); return a boolen if the state Transient was visited
 * - (...).ObserverFSM.wasRiseVisited(); return a boolen if the state Rise was visited
 * - (...).ObserverFSM.wasOvershootVisited(); return a boolen if the state Overshoot was visited
 * - (...).ObserverFSM.wasBoundedVisited(); return a boolen if the state Bounded was visited
 * 
 * But, is one function next that covers all properties necessary or multiple function that each cover one automaton?
 * First try with multiple functions that each cover one büchi automaton
 * all functions follow the schema int next_(...)(int state, Observer observer)
 * - were next_(...) donates the property for which the büchi automaton stands
 * - and int state and Observer observer the model instance 
*/

// Checks if the system reaches the rise state
int nextRise(int state, std::map<APs, bool> AP){
    /*
    never { Frise 
    T0_init:
        if
        :: (rise) -> goto accept_all
        :: (!(rise)) -> goto T0_init
        fi;
    accept_all:
        skip
    }
    */
    __dios_trace_f("Rise: %x", AP[rise]);
    switch(state) {
        case -1:
            return 1;

        case 0:
            __vm_ctl_flag(0, _VM_CF_Error);
            if(AP[rise]) {return 0;}

        case 1:
            
            if(AP[rise]) {return 0;}
            if(!AP[rise]) {return 1;}

        default:
            return state;
    }
};

// Checks if the system reaches the overshoot state
int nextOvershoot(int state, std::map<APs, bool> AP){
    /* never { G!overshoot 
    accept_init:
        if
        :: (!(overshoot)) -> goto accept_init
        :: (overshoot) -> goto T0_S1
        fi;
    T0_S1:
        if
        :: (true) -> goto T0_S1
        fi;
    }
    */
    __dios_trace_f("Overshoot; %x", AP[overshoot]);
    switch (state) {
        case -1: // initial state
            return 0; // state is now initilized but not visited

        case 0:
            // __vm_ctl_flag(0, _VM_CF_Error);
            if(!AP[overshoot]) {return 0;}
            if(AP[overshoot]) {return 1;}

        case 1:
            __vm_ctl_flag(0, _VM_CF_Error);
            if (AP[overshoot]) {return 1;}

        default:
            return state;

   }
};

// Checks if the system reaches a stable state within the settling time
int nextSettlingTime(int state, std::map<APs, bool> AP){
    /* never {  G(!ST | GSS) 
    accept_init:
        if
        :: ((SS) && (ST)) -> goto accept_S0
        :: (!(ST)) -> goto accept_init
        :: ((!(SS)) && (ST)) -> goto T0_S2
        fi;
    accept_S0:
        if
        :: (SS) -> goto accept_S0
        :: (!(SS)) -> goto T0_S2
        fi;
    T0_S2:
        if
        :: (true) -> goto T0_S2
    fi;
    }*/

    __dios_trace_f("AP[settling time]: %x", AP[settling]); 
    __dios_trace_f("AP[safe]: %x", AP[safe]);
    switch(state) {
        case -1:
            return 1;
        case 0:
            __vm_ctl_flag(0, _VM_CF_Accepting);
            if (AP[safe]) {return 0;}
            if (!AP[safe]) {return 2;}

        case 1:
            // __vm_ctl_flag(0, _VM_CF_Accepting);
            if (!AP[settling]) {return 1;}
            if (AP[settling] && AP[safe]) {return 0;}
            if (AP[settling] && !AP[safe]) {return 2;}
            
        case 2:
            // __vm_ctl_flag(0, _VM_CF_Error);
            return 2;

        default:
            return state;
    }

};

// int nextSettlingTime(int state, std::map<APs, bool> AP){
//     /*
//     never { Fsettling_time
//     T0_init:
//         if
//         :: (settling_time) -> goto accept_all
//         :: (!(settling_time)) -> goto T0_init
//         fi;
//     accept_all:
//         skip
//     }
//     */
//    __dios_trace_f("Settling Time: %x", AP[settling]); 
//    switch(state) {
//     case -1:
//         return 1;

//     case 0:
//         __vm_ctl_flag(0, _VM_CF_Accepting);
//         if(AP[settling]) {return 0;}

//     case 1:
//         if(AP[settling]) {return 0;}
//         if(!AP[settling]) {return 1;}

//     default:
//         return state;
//    }
// };




/* Function: next ====================================================
 * 
 * Abstract:
 *   Sync the büchi automata (property) with the system
 *   Property: <>([]safe)
 *   Type: Liveness (execute with --liveness)  
 */

int nextStable( int state, std::map<APs, bool> AP ) {

    __dios_trace_f("AP[safe]: %x", AP[safe]); 
    switch ( state ) {
        case -1:
            return 0;
        /*
        T0_init:
        if
        :: (true) -> goto T0_init
        :: (safe) -> goto accept_S1
        fi;
         */
        case 0:
            if (!AP[safe]) {return 0;}
            if (AP[safe]) {return __vm_choose( 2 ) ? 0 : 1;}
        /*
        accept_S1:
        if
        :: (safe) -> goto accept_S1
        :: (!(safe)) -> goto T0_S2
        fi;
        */
        case 1:
            __vm_ctl_flag( 0, _VM_CF_Accepting);
            if (AP[safe]) {return 1;}
            if (!AP[safe]) {return 2;}
        /*
        T0_S2:
        if
        :: (true) -> goto T0_S2
        fi;
        */
        case 2:
            return 2;
        default:
            return state;
    }
}


/* Function: main =============================================================
 *
 * Abstract:
 *   Execute model on a generic target such as a workstation.
 */
int_T main(int_T argc, const char *argv[])
{
    int_T ret;

    /* External mode */
    rtParseArgsForExtMode(argc, argv);
 
    /*******************************************
     * warn if the model will run indefinitely *
     *******************************************/
#if MAT_FILE==0 && EXT_MODE==0
    printf("warning: the simulation will run with no stop time; "
           "to change this behavior select the 'MAT-file logging' option\n");
    fflush(NULL);
#endif

    /************************
     * Initialize the model *
     ************************/
    rt_InitModel(MODEL_INSTANCE);

    /* External mode */
    rtSetTFinalForExtMode(&rtmGetTFinal(MODEL_INSTANCE.getRTM()));
    rtExtModeCheckInit(NUMST);
    rtExtModeWaitForStartPkt(rtmGetRTWExtModeInfo(MODEL_INSTANCE.getRTM()),
                             NUMST,
                             (boolean_T *)&rtmGetStopRequested(MODEL_INSTANCE.getRTM()));

    (void)printf("\n** starting the model **\n");

    /***********************************************************************
     * Execute (step) the model.  You may also attach rtOneStep to an ISR, *
     * in which case you replace the call to rtOneStep with a call to a    *
     * background task.  Note that the generated code sets error status    *
     * to "Simulation finished" when MatFileLogging is specified in TLC.   *
     ***********************************************************************/

    /***********************************************************************
     *  Instructions to embed property
     *  while ( true )
     *   {
     *       __dios_reschedule();
     *       system_step();
     *       buchi_step();
     *   }
     * */


    /* 
     * States for model checking with Divine
     * Four different states are needed for each büchi automaton
    */
    int stateStable = -1, oldStateStable = 0;

    int stateRise = -1, oldStateRise = 0;
    int stateOvershoot = -1, oldStateOvershoot = 0;
    int stateSettlingTime = -1, oldStateSettlingTime = 0;


    //while (rtmGetErrorStatus(MODEL_INSTANCE.getRTM()) == NULL &&
           //!rtmGetStopRequested(MODEL_INSTANCE.getRTM())) {
    while(true){    
        __dios_reschedule();
        oldStateRise = stateRise; 
        oldStateOvershoot = stateOvershoot;
        oldStateSettlingTime = stateSettlingTime;

        oldStateStable = stateStable;


        rtExtModePauseIfNeeded(rtmGetRTWExtModeInfo(MODEL_INSTANCE.getRTM()),
                               NUMST,
                               (boolean_T *)&rtmGetStopRequested(MODEL_INSTANCE.getRTM()));

        if (rtmGetStopRequested(MODEL_INSTANCE.getRTM())) break;

        /* external mode */
        rtExtModeOneStep(rtmGetRTWExtModeInfo(MODEL_INSTANCE.getRTM()),
                         NUMST,
                         (boolean_T *)&rtmGetStopRequested(MODEL_INSTANCE.getRTM()));
        
        /*
        * 
        * No need for an extra call for the observer transitions, because the observer is part of the model.
        * Everytime the model does a step is "automatically" updates the values. 
        * Therefore, rt_OneStep(...) also updates the Observer.
        * 
        */
        rt_OneStep(MODEL_INSTANCE);
        
        // Each Büchi automaton is updated 
        // stateRise = nextRise(stateRise, MODEL_INSTANCE.AP);
        // stateOvershoot = nextOvershoot(stateOvershoot, MODEL_INSTANCE.AP);
        stateSettlingTime = nextSettlingTime(stateSettlingTime, MODEL_INSTANCE.AP);

        // stateStable = nextStable(stateStable, MODEL_INSTANCE.AP);


        // __dios_trace_f( "state rise: %d -> %d", oldStateRise, stateRise );
         __dios_trace_f( "state stable: %d -> %d", oldStateSettlingTime, stateSettlingTime );
        // __dios_trace_f( "state overshoot: %d -> %d", oldStateOvershoot, stateOvershoot );

        // __dios_trace_f( "state stability: %d -> %d", oldStateStable, stateStable);
    }

    /*******************************
     * Cleanup and exit (optional) *
     *******************************/

#ifdef UseMMIDataLogging
    rt_CleanUpForStateLogWithMMI(rtmGetRTWLogInfo(MODEL_INSTANCE.getRTM()));
#endif
    rt_StopDataLogging(MATFILE,rtmGetRTWLogInfo(MODEL_INSTANCE.getRTM()));

    ret = rt_TermModel(MODEL_INSTANCE);

    rtExtModeShutdown(NUMST);

    return ret;
}

/* [EOF] main.cpp */


