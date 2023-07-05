/*
*
* This file includes all states of the Observer and additionally methods for transitions and actions.
* Contains the 5 States: Rest, Transient, Rise, Overshoot, Bounded
*
*/

#pragma once
#include "ObserverState.h"
#include "Observer.h"
#include <chrono>

// State Rest
class Rest : public ObserverState {
    public:
        void transition(Observer* observer);
        static ObserverState& getInstance();

    private:
        Rest() {}
        Rest(const Rest& other);
        Rest& operator=(const Rest& other);
};

// State Transient
class Transient : public ObserverState {
    public:
        void transition(Observer* observer);
        static ObserverState& getInstance();

    private:

        std::chrono::duration<double> timeTaken;

        Transient() {}
        Transient(const Transient& other);
        Transient& operator=(const Transient& other);
};

// State Rise
class Rise : public ObserverState {
    public:
        void transition(Observer* observer);
        static ObserverState& getInstance();

    private:
        Rise() {}
        Rise(const Rise& other);
        Rise& operator=(const Rise& other);
};

// State Overshoot
class Overshoot : public ObserverState {
    public:
        void transition(Observer* observer);
        static ObserverState& getInstance();

    private:
        Overshoot() {}
        Overshoot(const Overshoot& other);
        Overshoot& operator=(const Overshoot& other);
};

// State Bounded
class Bounded : public ObserverState {
    public:
        void transition(Observer* observer);
        static ObserverState& getInstance();

    private:
        
        // "real time" counter
        std::chrono::time_point<std::chrono::steady_clock> counterStart = std::chrono::steady_clock::now();
        std::chrono::time_point<std::chrono::steady_clock> counterCurrent;
        std::chrono::duration<double> counterDuration;
        std::chrono::duration<double> timeTaken;

        // This boolen track if the counter should be reseted or just go on
        bool counterReset = false;

        /* 
        * counterLimit is the value in seconds that the FSM must remain in Bounded
        * before it is considered stable and transits back to Rest
        */
        double counterLimit = 2; // in seconds

        Bounded() {}
        Bounded(const Bounded& other);
        Bounded& operator=(const Bounded& other);
};