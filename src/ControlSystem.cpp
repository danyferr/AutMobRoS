#include "ControlSystem.hpp"

ControlSystem::ControlSystem(double dt)
    : q1("quat1"), g(2.0), motorVoltageSetpoint(0.0), motor("motor1"),
    signalChecker(-0.2, 0.2), timedomain("Main time domain", dt, true)
{
    // Name all blocks
    q1.setName("q1");
    g.setName("gain");
    signalChecker.setName("signalChecker");
    motor.setName("motor1");
    motorVoltageSetpoint.setName("motorVoltageSetpoint");

    // Name all signals
    q1.getOut().getSignal().setName("phi/2");
    g.getOut().getSignal().setName("phi");
    motorVoltageSetpoint.getOut().getSignal().setName("Motor voltage stepoint [V]");

    // Connect signals
    g.getIn().connect(q1.getOut());
    signalChecker.getIn().connect(g.getOut());
    motor.getIn().connect(motorVoltageSetpoint.getOut());

    // Add blocks to timedomain
    timedomain.addBlock(q1);
    timedomain.addBlock(g);
    timedomain.addBlock(signalChecker);
    timedomain.addBlock(motorVoltageSetpoint);
    timedomain.addBlock(motor);

    // Add timedomain to executor
    eeros::Executor::instance().add(timedomain);
}