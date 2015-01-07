function [w1, w2, w3, w4] = PD_Control(position, angles, velocity, Dangles, targetZ, targetAngles)
%PD_CONTROL Summary of this function goes here
%   Detailed explanation goes here
global m g k L b;
global Ixx Iyy Izz;
global Kdz Kdroll Kdpitch Kdyaw Kpz Kproll Kppitch Kpyaw;
    angles = deg2rad(angles);
    targetAngles = deg2rad(targetAngles);
    targetVeloZ = 0;
    targetVeloRoll = 0;
    targetVeloPitch = 0;
    targetVeloYaw = 0;
    veloZ = velocity(3);
    veloRoll = Dangles(1);
    veloPitch = Dangles(2);
    veloYaw = Dangles(3);
    Z = position(3);
    roll = angles(1);
    pitch = angles(2);
    yaw = angles(3);
    targetRoll = targetAngles(1);
    targetPitch = targetAngles(2);
    targetYaw = targetAngles(3);

    thrust = (g + Kdz*(targetVeloZ - veloZ) + Kpz*(targetZ - Z)) * m/(cos(roll)*cos(pitch));
    torqueRoll = Ixx * (Kdroll*(targetVeloRoll - veloRoll) + Kproll*(targetRoll - roll));
    torquePitch = Iyy * (Kdpitch*(targetVeloPitch - veloPitch) + Kppitch*(targetPitch - pitch));
    torqueYaw = Izz * (Kdyaw*(targetVeloYaw - veloYaw) + Kpyaw*(targetYaw - yaw));
    % Restrict values
    maxThrust = -750*g*4;
    if (thrust < 0); thrust = 0.1* maxThrust; end;
    if (thrust > maxThrust); thrust = maxThrust; end;
    
    % Simplify terms before using them.
    simpThrust = thrust/(4*k);
    simpTorquePitch = torquePitch / (2*k*L);
    simpTorqueRoll = torqueRoll / (2*k*L);
    simpTorqueYaw = torqueYaw / (4*b);
    
    % Calculate correct rpm for each motor.
    w1 = simpThrust - simpTorquePitch - simpTorqueYaw;
    w2 = simpThrust - simpTorqueRoll + simpTorqueYaw;
    w3 = simpThrust + simpTorquePitch - simpTorqueYaw;
    w4 = simpThrust + simpTorqueRoll + simpTorqueYaw;
    if(w1 > maxThrust/(4*k)); w1 = maxThrust/4*k; end;
    
    if(w2 > maxThrust/(4*k)); w2 = maxThrust/4*k; end;
    if(w3 > maxThrust/(4*k)); w3 = maxThrust/4*k; end;
    if(w4 > maxThrust/(4*k)); w4 = maxThrust/4*k; end;
    if(w1 < 0); w1 = 0; end;
    if(w2 < 0); w2 = 0; end;
    if(w3 < 0); w3 = 0; end;
    if(w4 < 0); w4 = 0; end;
end

