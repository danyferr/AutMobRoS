#ifndef MyRobotSAFETYPROPERTIES_HPP_
#define MyRobotSAFETYPROPERTIES_HPP_

#include <eeros/safety/SafetyProperties.hpp>
#include <eeros/hal/HAL.hpp>
#include "ControlSystem.hpp"

class MyRobotSafetyProperties : public eeros::safety::SafetyProperties
{
public:
    MyRobotSafetyProperties(ControlSystem &cs, double dt);

    // Define all possible events
    eeros::safety::SafetyEvent doSystemOff;
    eeros::safety::SafetyEvent doSystemOn;
    eeros::safety::SafetyEvent systemOn;
    eeros::safety::SafetyEvent systemOff;
    eeros::safety::SafetyEvent doMotorPowerOn;
    eeros::safety::SafetyEvent doMotorPowerOff;
    eeros::safety::SafetyEvent emergencyOn;
    eeros::safety::SafetyEvent emergencyOver;
    eeros::safety::SafetyEvent startDriving;
    eeros::safety::SafetyEvent startBraking;
    // eeros::safety::SafetyEvent stopParking;
    // eeros::safety::SafetyEvent startParking;





    // Defina all possible levels
    eeros::safety::SafetyLevel slSystemOff;
    eeros::safety::SafetyLevel slSystemOn;
    eeros::safety::SafetyLevel slSwitchingOn;
    eeros::safety::SafetyLevel slSwitchingOff;
    eeros::safety::SafetyLevel slMotorPowerOn;
    eeros::safety::SafetyLevel slEmergency;
    eeros::safety::SafetyLevel slDriving;
    eeros::safety::SafetyLevel slBraking;
    // eeros::safety::SafetyLevel slParking;


private:
    // Define all critical outputs
    eeros::hal::Output<bool>* greenLED;
    eeros::hal::Output<bool>* redLED;

    // Define all critical inputs
    eeros::hal::Input<bool>* buttonPause;
    eeros::hal::Input<bool>* buttonMode;

    ControlSystem &cs;
};

#endif // MyRobotSAFETYPROPERTIES_HPP_
