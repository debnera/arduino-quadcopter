function [w1, w2, w3, w4 ] = Z_Control( anglesDeg, maxThrust )
%Z_CONTROL Summary of this function goes here
% Compute required thrust to keep the same altitude.
global g m k;
    T_vector = RotationMatrix(anglesDeg) * [0; 0; 1]; 
    T = -g*m/T_vector(3);
    if (T < 0); T = 0.1* maxThrust; end;
    if (T > maxThrust); T = maxThrust; end;
        
    w1 = T/(4*k);
    w2 = T/(4*k);
    w3 = T/(4*k);
    w4 = T/(4*k);


end

