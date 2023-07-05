# Mapping Control Theory and Software Engineering Properties

[Repo Content](#contents)  |  [Model Checking](#model-checking)  |  [Observer Intro](#observer)  |  [Contact Us](#contact)  

This repository is an online appendex for the ongooing research effort for mapping control theory and software engineering properties.

## Contents 
- ./Results: contains the reports and results for the tests
- ./MATLAB: contains all matlab files necessray to run the code
- ./Velocity_Controlled_Vehicle_grt_rtw_ contains the auto-generated cpp code from simulink and the observer
- ./my_mpcACCsystem: contains the ACC desing by [Ricardo](https://github.com/rdinizcal/mappingCTSEproperties)


## Model Checking

### Prerequisites
- [DIVINE 4](https://divine.fi.muni.cz/) (Version 4.4.2) even though version 4.4.4 exist but it had no manual during time of this work.
- Access to [Spot](https://spot.lrde.epita.fr/app/).
- MATLAB (2021a), you can also try other versions but some small details may have changed and do not work exactly as stated here.
- Simulink + Simulink Coder Toolbox



### Code Generation
To generate the code from the Simulink model follow the steps below and the [MATLAB tutorial](https://de.mathworks.com/help/dsp/c-code-generation.html):
- Go to Model Parameter in the Simulink model
- Select C++ as the language
- Set Solver: Typ = Fixed-Step
- Hardware Implementation: Change it to your Operating System (i recommend Linux, at least Linux VM)
- Code Generation: Language = C++
- Choose either:
    - Generate Code Only = True; This does not generate a makefile only the code artifacts
    - Generate Code Only = False; This build the whole program and generates a makefile
    - ! Probably while compiling the MATLAB files will not be found, copie them from your MATLAB_ROOT folder on your system, same goes for the main function template...
- Interface: Class; Warning set to NONE
- Then open the Simulink Coder app and click on generate code or build

### Instrumenting the auto-generated CPP code for Model Checking (with DIVINE)


## Observer 

In the following a short explanation of the Observer is given to understand the basic functionallity.

The observer consists of 5 files:
  - ObserverState.h
  - ConcreteObserverStates.h
  - ConcreteObserverState.cpp
  - Observer.h
  - Observer.cpp

The files ObserverState.h and Observer.h are abstract classes of the state of the observer and the FSM itself.
The file ConcreteObserverStates.h include the definitions for all states, the constructor and deconstructor.
While in the file ConcreteObserverStates.cpp the expliic definitions of each state and method is implemented.
The Observer.cpp file contains the explicit definitions for the FSM.

### Functionality

In general the observer should capture trancient behavior of dynamic system (e.g., controller, CPS).
Here the Observer consits of 5 states:
  - Rest
  - Transient
  - Rise
  - Overshoot
  - Bounded

Initially the observer is in the rest state, but once it was triggeret by an external input the observer changes to transient.
If the system reaches a specific value wihtin a specific time the observer tranists to the rise state. 
If the system overshoot a predefined value the observer transits to overshoot and if the system putput is bounded by a defined margin the observer tranits to bounded.
Furthermore, if the observer stays in the bounded state long enough, it is assumed that the system has settled down and is called stable. The the observer then returns to the rest state.

### Interface

The observer provied vasrious methods to get acces to data and write data to the observer.
This section breifly describes how the observer can be adressed, so have can you pass date to the observer and how to get informations about it state/data. As well as how you set the thresholds for each state.

#### Setting the input data
The observer is equipt with the method setExternalInput(double ...), this method has three input:
  - reference
  - error
  - sysOut
in exactly this order.
With this method you can pass a reference signal, the error (difference between reference and real value) and the actual output of the system to examinat.

#### Set Thresholds for Guards

Moreover, the observer provieds the method initialThreshold(double ...), this method has five input:
  - epsilon
  - overshootLevel
  - riseLevel
  - riseTime
  - settlingTime
in exactly this order.
Epsilon percentage of the maximum steady state error compared to the settling point.
overshootLevel describes the Maximum percentage over the reference that is reached.
riseLevel defines the time the system needs to rise from starting point to and ending point. Here the start point is zero and the ending point is 90% of the reference.
settlingTime is defined by the time the system needs to enter and remain in a specific error band.

#### Return Visited States

The observer can return if state was visited or not. The method was*enter specific state*Visited() return a boolen if the state was visited or not. 1 if it was visited and 0 if not. Keep in mind *enter specific state* is a placeholder for the real state. This method exsists therefore five time, for each state.



## Contact

Feel free to submit issues or improvements

Don't hesitate to contact us, for questions, discussion, or suggestions.

Nils Chur (nils.chur@rub.de)