#include "MyRobotSafetyProperties.hpp"

MyRobotSafetyProperties::MyRobotSafetyProperties(ControlSystem &cs, double dt)
    : cs(cs),
    
      slSystemOff("System is offline"),
      slSwitchingOff("System is switching off"),
      slSwitchingOn("System is switching on"),
      slEmergency("Emergency state"),
      slSystemOn("System is online"),
      slMotorPowerOn("Motor power is on"),
      slBraking("Braking"),
      slDriving("Driving"),

      doSystemOn("Startup the system"),
      doSystemOff("Shutdown the system"),
      systemOn("System is on"),
      systemOff("System is off"),
      doMotorPowerOn("Turn on motor power"),
      doMotorPowerOff("Turn off motor power"),
      emergencyOn("Emergency triggered"),
      emergencyOver("Emergency over"),
      startDriving("Start driving"),
      startBraking("Start braking")


{
    eeros::hal::HAL &hal = eeros::hal::HAL::instance();

    // Declare and add critical outputs
    greenLED = hal.getLogicOutput("onBoardLEDgreen");
    redLED = hal.getLogicOutput("onBoardLEDred");

    criticalOutputs = { greenLED, redLED };

    // Declare and add critical inputs
    buttonPause = eeros::hal::HAL::instance().getLogicInput("onBoardButtonPause");
    buttonMode = eeros::hal::HAL::instance().getLogicInput("onBoardButtonMode");

    criticalInputs = { buttonPause, buttonMode };

    // Add all safety levels to the safety system
    addLevel(slSystemOff);
    addLevel(slSwitchingOff);
    addLevel(slSwitchingOn);
    addLevel(slEmergency);
    addLevel(slSystemOn);
    addLevel(slMotorPowerOn);
    addLevel(slBraking);
    addLevel(slDriving);


    // Add events to individual safety levels
    slSystemOff.addEvent(doSystemOn, slSwitchingOn, kPublicEvent);
    slSwitchingOff.addEvent(systemOff, slSystemOff, kPublicEvent);
    slSwitchingOn.addEvent(systemOn, slSystemOn, kPublicEvent);
    slSystemOn.addEvent(doMotorPowerOn, slMotorPowerOn, kPublicEvent);
    slMotorPowerOn.addEvent(doMotorPowerOff, slSystemOn, kPublicEvent);
    slMotorPowerOn.addEvent(startDriving, slDriving, kPublicEvent);
    slDriving.addEvent(startBraking, slBraking, kPublicEvent);
    slBraking.addEvent(startDriving, slDriving, kPublicEvent);


    // Add events to multiple safety levels
    addEventToAllLevelsBetween(slEmergency, slMotorPowerOn, doSystemOff, slSwitchingOff, kPublicEvent);
    addEventToAllLevelsBetween(slSystemOn, slMotorPowerOn, emergencyOn, slEmergency, kPublicEvent);


    // Define input actions for all levels
    slSystemOff.setInputActions({           ignore(buttonPause),                    ignore(buttonMode) });
    slSwitchingOff.setInputActions({        ignore(buttonPause),                    ignore(buttonMode) });
    slBraking.setInputActions({             ignore(buttonPause),                    ignore(buttonMode) });
    slSwitchingOn.setInputActions({         ignore(buttonPause),                    ignore(buttonMode) });
    slEmergency.setInputActions({           ignore(buttonPause),                    check(buttonMode, false, emergencyOver) });
    slSystemOn.setInputActions({            check(buttonPause, false, emergencyOn),    ignore(buttonMode) });
    slMotorPowerOn.setInputActions({        check(buttonPause, false, emergencyOn),    ignore(buttonMode) });
    slBraking.setInputActions({             ignore(buttonPause),                    ignore(buttonMode) });
    slDriving.setInputActions({             check(buttonPause, false, startBraking), ignore(buttonMode) });


    // Define output actions for all levels
    slSystemOff.setOutputActions({           set(greenLED, false),   set(redLED, false) });
    slSwitchingOff.setOutputActions({        set(greenLED, false),   set(redLED, true) });
    slSwitchingOn.setOutputActions({         set(greenLED, false),   set(redLED, false) });
    slEmergency.setOutputActions({           set(greenLED, true),    set(redLED, true) });
    slSystemOn.setOutputActions({            set(greenLED, true),    set(redLED, false) });
    slMotorPowerOn.setOutputActions({        set(greenLED, true),    set(redLED, false) });
    slDriving.setOutputActions({             set(greenLED, true),    set(redLED, false) });
    slBraking.setOutputActions({             set(greenLED, true),    set(redLED, false) });

    // Define and add level actions
    slSystemOff.setLevelAction([&](SafetyContext *privateContext) {
        cs.timedomain.stop();
        eeros::Executor::stop();
    });

    slSwitchingOff.setLevelAction([&](SafetyContext *privateContext) {
        cs.timedomain.stop();
        privateContext->triggerEvent(systemOff);
    });

    slSwitchingOn.setLevelAction([&](SafetyContext *privateContext) {
        cs.timedomain.start();
        privateContext->triggerEvent(systemOn);
    });

    slSystemOn.setLevelAction([&](SafetyContext *privateContext) {
        if (slSystemOn.getNofActivations()*dt >= 1)
        {
            privateContext->triggerEvent(doMotorPowerOn);
        }
    });

    slEmergency.setLevelAction([&](SafetyContext *privateContext) {
        
    });

    slMotorPowerOn.setLevelAction([&](SafetyContext *privateContext) {
        if (slMotorPowerOn.getNofActivations()*dt >= 5)
        {
            privateContext->triggerEvent(startDriving);
        }
    });

    slDriving.setLevelAction([&](SafetyContext *privateContext) {
        if (slDriving.getNofActivations()*dt >= 5)
        {
            privateContext->triggerEvent(startBraking);
        }
    });



    // Define entry level
    setEntryLevel(slSystemOff);

    // Define exit function
    exitFunction = ([&](SafetyContext *privateContext) {
        privateContext->triggerEvent(doSystemOff);
    });
}
