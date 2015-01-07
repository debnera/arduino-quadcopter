function w = Throttle2Speed( w )
%THROTTLE2SPEED Summary of this function goes here
%   Detailed explanation goes here
%   1200kv * 12v = 14400 Max RPM
global g k;
maxLift = -g*750; % In grams
maxSpeed = maxLift/k;
minSpeed = 0;

for i = 1:numel(w)
    w(i) = w(i)*maxSpeed/100; % /100
    if w(i) < minSpeed
        w(i) = minSpeed;
    end
    if w(i) > maxSpeed
        w(i) = maxSpeed;
    end
end
end

