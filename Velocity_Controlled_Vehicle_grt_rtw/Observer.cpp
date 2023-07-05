/*
*
* This file includes the definitions of the methods for the Observer.
*
*/

#include "Observer.h"
#include "ConcreteObserverStates.h"

// Initialization
Observer::Observer() {
    currentState = &Rest::getInstance();

    reference = 0;
    error = 0;
    sysOut = 0;

    /*
    * Clock initialization and set reference value.
    * As soon as the Observer is generated the clocks starts ticking.
    */
    clockStart = std::chrono::steady_clock::now();


    // Init all states with false -> not entered
    restWasEntered = false;
    transientWasEntered = false;
    riseWasEntered = false;
    overshootWasEntered = false;
    boundedWasEntered = false;


    // Thresholds
    epsilon = 0;
    overshootLevel = 0;
    riseLevel = 0;
    riseTime = 0;
    settlingTime = 0;
}

void Observer::setState(ObserverState& newState) {
    currentState = &newState;
}

void Observer::transition() {
    // Next state is determined by the current state
    currentState->transition(this);
}

void Observer::setExternalInput(double Input1, double Input2, double Input3) {
    reference = Input1;
    error = fabs(Input2/Input1)*100;
    sysOut = Input3;
}

void Observer::initialThreshold(double thr1, double thr2, double thr3, double thr4, double thr5) {
    epsilon = thr1;
    overshootLevel = 1.0 + (thr2/100);
    riseLevel = thr3/100;
    riseTime = thr4;
    settlingTime = thr5;
}

bool Observer::wasRestVisited(){
    return restWasEntered;
}

bool Observer::wasTransientVisited(){
    return transientWasEntered;
}

bool Observer::wasRiseVisited(){
    return riseWasEntered;
}

bool Observer::wasOvershootVisited(){
    return overshootWasEntered;
}

bool Observer::wasBoundedVisited(){
    return boundedWasEntered;
}

void Observer::setRestVisited(){
    restWasEntered = true;
};

void Observer::setTransientVisited(){
    transientWasEntered = true;
};

void Observer::setRistVisited(){
    riseWasEntered = true;
};

void Observer::setOvershootVisited(){
    overshootWasEntered = true;
};

void Observer::setBoundedVisited(){
    boundedWasEntered = true;
};

void Observer::setCurrentTime() {
    clockNow = std::chrono::steady_clock::now();
}

std::chrono::time_point<std::chrono::steady_clock> Observer::returnCurrentTime() {
    return clockNow;
}

std::chrono::time_point<std::chrono::steady_clock> Observer::returnStartTime(){
    return clockStart;
}