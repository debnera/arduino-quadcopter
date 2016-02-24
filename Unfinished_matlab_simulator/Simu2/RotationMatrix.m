function R = RotationMatrix(anglesDeg)
%ROTATIONMATRIX Summary of this function goes here
%   Detailed explanation goes here
    angles = deg2rad(anglesDeg);
    roll = angles(1);
    pitch = angles(2);
    yaw = angles(3);
    R = [cos(yaw)*cos(pitch), cos(yaw)*sin(pitch)*sin(roll) - sin(yaw)*cos(roll), cos(yaw)*sin(pitch)*cos(roll) + sin(yaw)*sin(roll);
        sin(yaw)*cos(pitch), sin(yaw)*sin(pitch)*sin(roll) + cos(yaw)*cos(roll), sin(yaw)*sin(pitch)*cos(roll) - cos(yaw)*sin(roll);
        -sin(pitch), cos(pitch)*sin(roll), cos(pitch)*cos(roll)];

end

