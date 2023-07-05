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
        std::chrono::time_point<std::chrono::steady_clock> clockStart;
        std::chrono::time_point<std::chrono::steady_clock> clockNow;


        // Booleans that capture if a state was visited or not
        bool restWasEntered;
        bool transientWasEntered;
        bool riseWasEntered;
        bool overshootWasEntered;
        bool boundedWasEntered;

    public:
        Observer();

        double reference;
        double error;
        double sysOut;

        // Thresholds
        double epsilon;
        double overshootLevel;
        double riseLevel;
        double riseTime;
        double settlingTime;


        /*
        * Saves the current time to a time point
        */
        void setCurrentTime();

        /*
        * Return the current time
        */
        std::chrono::time_point<std::chrono::steady_clock> returnCurrentTime();

        /*
        * Return the start time point
        */
        std::chrono::time_point<std::chrono::steady_clock> returnStartTime();

        inline ObserverState* getCurrentState() const {return currentState; }

        /*
        * Function to cast new states
        */
        void transition();

        /*
        *
        * setExternalInput gets the data from the system and save the values in the Observer.
        * These are the reference values to which the guards are compared.
        * Order: Reference, Error, System Output
        */
        void setExternalInput(double reference, double error, double sysOut); 

        /*
        * 
        * initalThreshold set the values for the transition guards.
        * The inputs are compared to this values.
        * Order: steady state error (%), Overhsoot (%), riseLevel (%), rise time (sec), settling time (sec) 
        */
        void initialThreshold(double epsilon, double overshoot_level, double riseLevel, double riseTime, double settlingTime); // sets Threshold to overshoot
    

        /*
        * Set new State
        */
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
        void setRistVisited();

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