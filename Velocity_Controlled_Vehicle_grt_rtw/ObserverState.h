/*
*
* This file includes the abstract class of the Observer States.
* 
*/

#pragma once
#include "Observer.h"

// Forward declaration to resolve circular dependencies
class Observer;

class ObserverState{
    public:
        virtual void transition(Observer* observer) = 0;

        // Destructor
        virtual ~ObserverState() {}
    
};