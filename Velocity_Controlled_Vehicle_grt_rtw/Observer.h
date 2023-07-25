/*
*
* This file includes the observer template.
*
*/

#pragma once
#include "ObserverState.h"
#include <chrono>

// Forward declarations to resolve circular dependencies
class ObserverState;

class Observer {
    protected:

        // Test Time with Chrono
        //std::chrono::time_point<std::chrono::steady_clock> clockStart;
        //std::chrono::time_point<std::chrono::steady_clock> clockNow;

        /*
        * Struct that captures if a state was entered (true) or not (false)
        * These are protected because here it is very important that they are not accidentally changed during runtime!
        */
        bool restWasEntered;
        bool transientWasEntered;
        bool riseWasEntered;
        bool overshootWasEntered;
        bool boundedWasEntered;

    public:
        Observer();
        // Attributes
        // Input generated from the system 
        double reference;
        double error;
        double sysOut;
        double time;

        // Thresholds design decisions
        double epsilon;
        double overshootLevel;
        double riseLevel;
        double riseTime;
        double settlingTime;

        // Methods to set and get time values
        /*
        * Saves the current time to a time point
        */
        //void setCurrentTime();

        /*
        * Return the current time
        */
        //std::chrono::time_point<std::chrono::steady_clock> returnCurrentTime();

        /*
        * Return the start time point
        */
        //std::chrono::time_point<std::chrono::steady_clock> returnStartTime();
        // void stop_timer(); maybe needed later but not now

        // Methods
        inline ObserverState* getCurrentState() const {return currentState; }
        void transition();

        /*
        *
        * setExternalInput gets the data from the system and save the values in the Observer.
        * These are the reference values to which the guards are compared.
        * Input order is: first, the reference/input of the system
        * Second, the error, difference between reference and system output
        * Third, the system output
        * Forth, time information
        */
        void setExternalInput(double reference, double error, double sysOut, double time); 

        /*
        * 
        * initalThreshold set the values for the transition guards.
        * The inputs are compared to this values.
        * The input order is: first, the accepted steady state error epsilon
        * Second, the maximum allowed overshoot in %
        * Third, the rise level in %
        * Fourth, the time allowed to reach the rise level in sec
        * And finally, the settling times
        * 
        */
        void initialThreshold(double epsilon, double overshoot_level, double riseLevel, double riseTime, double settlingTime); // sets Threshold to overshoot
    

        // Set new State
        void setState(ObserverState& newstate);

        /*
        * Returns true if the state Rest was visited or false if Rest was not visited
        */
        bool wasRestVisited();

        /*
        * Returns true if the state Transient was visited or false if Transient was not visited
        */
        bool wasTransientVisited();

        /*
        * Returns true if the state Rise was visited or false if Rise was not visited
        */
        bool wasRiseVisited();

        /*
        * Returns true if the state Overshoot was visited or false if Overshoot was not visited
        */
        bool wasOvershootVisited();

        /*
        * Returns true if the state Bounded was visited or false if Bounded was not visited
        */
        bool wasBoundedVisited();


        
        /*
        * Sets the boolean wasRestVisited to true
        */
        void setRestVisited();

        /*
        * Sets the boolean wasTransientVisited to true
        */
        void setTransientVisited();

        /*
        * Sets the boolean wasRiseVisited to true
        */
        void setRiseVisited();

        /*
        * Sets the boolean wasOvershootVisited to true
        */
        void setOvershootVisited();

        /*
        * Sets the boolean wasBoundedVisited to true
        */
        void setBoundedVisited();
        

    private:
        ObserverState* currentState;
};