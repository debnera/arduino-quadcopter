function [w] = Real_PD_Control(angles, angularVelo, z, veloZ, targetAngles, targetZ )
%REAL_PD_CONTROL Summary of this function goes here
%   Angles deg/rad
%   Angular velocity estimate
%   Z estimate
%   Z velocity estimate
%   Engine speed in 0%-100%
global Kdz Kdroll Kdpitch Kdyaw Kpz Kproll Kppitch Kpyaw Kiroll Kipitch Kiyaw Kiz;
global dt;
errors = targetAngles - angles;
persistent integrals intZ;
if isempty(integrals); integrals = zeros(3,1); intZ = 0; end;
integrals = integrals + errors*dt;
intZ = intZ + (targetZ - z)*dt;


%     thrust = (g + Kdz*(0 - veloZ) + Kpz*(targetZ - Z)) * m/(cos(roll)*cos(pitch));
thrust = 30 + Kdz*(0-veloZ) + Kpz*(targetZ - z) + Kiz*intZ;
torqueRoll = Kdroll*(0 - angularVelo(1)) + Kproll*errors(1) + Kiroll*integrals(1);
torquePitch = Kdpitch*(0 - angularVelo(2)) + Kppitch*errors(2) + Kipitch*integrals(2);
torqueYaw = Kdyaw*(0 - angularVelo(3)) + Kpyaw*errors(3) + Kiyaw*integrals(3);

w1 = thrust - torquePitch - torqueYaw;
w2 = thrust - torqueRoll + torqueYaw;
w3 = thrust + torquePitch - torqueYaw;
w4 = thrust + torqueRoll + torqueYaw;

w = [w1,w2,w3,w4];

motorMax = max(w);
motorMin = min(w);
if(motorMax > 100)
    w = w + (100 - motorMax);
end
if(motorMin < 0)
    w = w + (0 - motorMin);
end

end

