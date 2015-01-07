function R = rotation(angles)
%ROTATION Calculates the rotation matrix R
%   Detailed explanation goes here
    roll = angles(1);
    pitch = angles(2);
    yaw = angles(3);

    x1 = cos(roll)*cos(yaw)-cos(pitch)*sin(roll)*sin(yaw);
    x2 = cos(pitch)*cos(yaw)*sin(roll)+cos(roll)*sin(yaw);
    x3 = sin(roll)*sin(pitch);
    
    y1 = -cos(yaw)*sin(roll)-cos(roll)*cos(pitch)*sin(yaw);
    y2 = cos(roll)*cos(pitch)*cos(yaw)-sin(roll)*sin(yaw);
    y3 = cos(roll)*sin(pitch);
    
    z1 = sin(pitch)*sin(yaw);
    z2 = -cos(yaw)*sin(pitch);
    z3 = cos(pitch);
    
    R = [x1 y1 z1; x2 y2 z2; x3 y3 z3];

end

