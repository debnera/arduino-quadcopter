function DDangles = AngularAcceleration( torques )
%ANGULARACCELERATION Summary of this function goes here
%   Detailed explanation goes here
global Ixx Iyy Izz;
    % Body frame accelerations:
    DDroll = torques(1)/Ixx;
    DDpitch = torques(2)/Iyy;
    DDyaw = torques(3)/Izz;
    DDangles = [DDroll; DDpitch; DDyaw];
    DDangles = rad2deg(DDangles);
    
    % Inertial frame accelerations:
%     mat1 = [0, Droll*cos(roll)*tan(pitch)+Dpitch*sin(roll)/(cos(pitch))^2,...
%         Droll*sin(roll)*cos(pitch)+Dpitch*cos(roll)/(cos(pitch))^2];
%     
%     mat2 = [0, -Droll*sin(roll), -Droll*cos(roll)];
%     
%     mat3 = [0, Droll*cos(roll)/cos(pitch)+Droll*sin(roll)*tan(pitch)/cos(pitch),...
%             -Droll*sin(roll)/cos(pitch)+Dpitch*cos(roll)*tan(pitch)/cos(pitch)];
%     
%     Matrix = [mat1; mat2; mat3]; 
end

