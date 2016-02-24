function [w] = Old_Real_PD_control( anglesDegree, Z, targetAngles, targetZ, prevAngles, prevZ)
%REAL_PD_CONTROL Summary of this function goes here
%   Detailed explanation goes here
global m g k L b dt;
global Ixx Iyy Izz;
global Kdz Kdroll Kdpitch Kdyaw Kpz Kproll Kppitch Kpyaw;
    prevAngles = deg2rad(prevAngles);
    anglesRad = deg2rad(anglesDegree);
    targetAngles = deg2rad(targetAngles);
    
    
    
    
    roll = anglesRad(1);
    pitch = anglesRad(2);
    yaw = anglesRad(3);
    veloRoll = (roll - prevAngles(1))/dt;
    veloPitch = (pitch - prevAngles(2))/dt;
    veloYaw = (yaw - prevAngles(3))/dt;
    veloZ = (Z - prevZ)/dt;
    
    targetRoll = targetAngles(1);
    targetPitch = targetAngles(2);
    targetYaw = targetAngles(3);
    
    thrust = (g + Kdz*(0 - veloZ) + Kpz*(targetZ - Z)) * m/(cos(roll)*cos(pitch));
    torqueRoll = Ixx * (Kdroll*(0 - veloRoll) + Kproll*(targetRoll - roll));
    torquePitch = Iyy * (Kdpitch*(0 - veloPitch) + Kppitch*(targetPitch - pitch));
    torqueYaw = Izz * (Kdyaw*(0 - veloYaw) + Kpyaw*(targetYaw - yaw));
    
    simpThrust = thrust/(4*k);
    simpTorquePitch = torquePitch / (2*k*L);
    simpTorqueRoll = torqueRoll / (2*k*L);
    simpTorqueYaw = torqueYaw / (4*b);
    
    maxThrust = -750*g;
    maxRPM = maxThrust/k;
    
    w1 = (simpThrust - simpTorquePitch - simpTorqueYaw)/maxRPM;
    w2 = (simpThrust - simpTorqueRoll + simpTorqueYaw)/maxRPM;
    w3 = (simpThrust + simpTorquePitch - simpTorqueYaw)/maxRPM;
    w4 = (simpThrust + simpTorqueRoll + simpTorqueYaw)/maxRPM;
    
    w = [w1 w2 w3 w4];
    for i = 1:4
        w(i) = min(w(i), 1);
        w(i) = max(w(i), 0.1);
%         printf('%0.01f\n',w(i));
    end
    w
    
    % SIMULATION ONLY
    w = w * maxRPM;
    
end

