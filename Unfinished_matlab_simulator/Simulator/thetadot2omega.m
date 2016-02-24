function omega = thetadot2omega(thetadot, theta)
    % omega = Angular velocity vector
    % Dtheta = Angular velocity
    % theta = Angles
    roll = theta(1);
    pitch = theta(2);
%     yaw = theta(3);
    matrix = [1 0 -sin(pitch); 
        0 cos(roll) cos(pitch)*sin(roll); 
        0 -sin(roll) cos(pitch)*cos(roll)];
    
    omega = matrix * thetadot;
    
end

