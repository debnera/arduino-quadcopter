function acceleration = Acceleration(w1, w2, w3, w4, anglesDeg )
%ACCELERATION Calculates the linear acceleration, including gravity.
%    f = ma  -->  a = f/m
global m g k L b;
    thrust = [0; 0; k * (w1+w2+w3+w4)];
    R = RotationMatrix(anglesDeg);
    acceleration = R*thrust/m;
    acceleration(3) = acceleration(3) + g;
    


end

