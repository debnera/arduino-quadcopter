% Compute thrust given current inputs and thrust coefficient.
function T = thrust(inputs, k)
    % Inputs are values for ${\omega_i}^2$
    T = [0; 0; k * sum(inputs)];
end