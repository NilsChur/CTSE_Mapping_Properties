/*
*
* This file includes the definitions of the methods for the Observer.
*
* Version 0.2               02.06.2023 
*/

#include "Observer.h"
#include "ConcreteObserverStates.h"
#include <cstdio>
#include <cmath>
#include <dios.h>
#include <sys/divm.h>
#include <iostream>

// Initialization
Observer::Observer() {
    currentState = &Rest::getInstance();

    // System "attributes"
    reference = 0;
    error = 0;
    sysOut = 0;
    time = 0;

    /*
    * Clock initialization and set reference value.
    * As soon as the Observer is generated the clocks starts ticking.
    */
    //std::chrono::steady_clock::time_point clockStart = std::chrono::steady_clock::now();
    //std::chrono::steady_clock::time_point clockNow;

    //clockStart = std::chrono::steady_clock::now();

    // Init all states with false -> not entered

    restWasEntered = false;
    transientWasEntered = false;
    riseWasEntered = false;
    overshootWasEntered = false;
    boundedWasEntered = false;




    // Thresholds
    epsilon = 0.0;
    overshootLevel = 0.0;
    riseLevel = 0.0;
    riseTime = 0.0;
    settlingTime = 0.0;
}

void Observer::setState(ObserverState& newState) {
    currentState = &newState;
}

void Observer::transition() {
    // Next state is determined by the current state
    currentState->transition(this);
}

/*
* Inputs are Signals/Data from the System.
* Order: reference, error, sysOut, time
*/
void Observer::setExternalInput(double Input1, double Input2, double Input3, double Input4) {
    reference = Input1;
    error = fabs(Input2/Input1)*100;
    sysOut = Input3;
    time = Input4;
    //__dios_trace_f("Ref: %d, Error: %d, Out: %d", reference, error, sysOut);
    printf("Error: %d\n", (int)(10*Input2));
    //printf("Time: %d\n", (int)(10*Input4));
}

/*
* Sets values for the property thresholds.
* Order: steady state error (%), Overhsoot (%), riseLevel (%), rise time (sec), settling time (sec)
*/
void Observer::initialThreshold(double thr1, double thr2, double thr3, double thr4, double thr5) {
    epsilon = thr1;
    overshootLevel = 1.0 + (thr2/100);
    riseLevel = 100-thr3;
    riseTime = thr4;
    settlingTime = thr5;
    printf("Threshold initialised: %d, %d, %d, %d, %d\n", (int)epsilon, (int)(10*overshootLevel), (int)riseLevel, (int)riseTime, (int)settlingTime);
}

// Returns true or false of Rest was entered
bool Observer::wasRestVisited(){
    return restWasEntered;
}

// Returns true or false of Transient was entered
bool Observer::wasTransientVisited(){
    return transientWasEntered;
}

// Returns true or false of Rise was entered
bool Observer::wasRiseVisited(){
    return riseWasEntered;
}

// Returns true or false of Overshoot was entered
bool Observer::wasOvershootVisited(){
    return overshootWasEntered;
}

// Returns true or false of Bounded was entered
bool Observer::wasBoundedVisited(){
    return boundedWasEntered;
}

void Observer::setRestVisited(){
    restWasEntered = true;
};

void Observer::setTransientVisited(){
    transientWasEntered = true;
};

void Observer::setRiseVisited(){
    riseWasEntered = true;
};

void Observer::setOvershootVisited(){
    overshootWasEntered = true;
};

void Observer::setBoundedVisited(){
    boundedWasEntered = true;
};


/*
* Starts the timer for checking time constraints
*/
/*void Observer::setCurrentTime() {
    clockNow = std::chrono::steady_clock::now();
}

std::chrono::time_point<std::chrono::steady_clock> Observer::returnCurrentTime() {
    return clockNow;
}

std::chrono::time_point<std::chrono::steady_clock> Observer::returnStartTime(){
    return clockStart;
}
*/
/* 
* Maybe need later
void Observer::stop_timer() {
}
*/