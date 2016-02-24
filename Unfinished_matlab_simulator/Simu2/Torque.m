function torque = Torque( m1,m2,m3,m4 )
%TORQUE Calculates total torque from the motors affecting the copter.
%   Detailed explanation goes here
%   L = Distance from motor to center
%   k = Thrust/rpm
%   b = Drag constant
global k L b;
    Troll = L*k*(-m2+m4); 
    Tpitch = L*k*(-m1+m3);
    Tyaw = b*(-m1+m2-m3+m4);
    
    torque = [Troll, Tpitch, Tyaw];
end

