motor = [1000, 500, 3000, 0];
target = 0.5;
mid = 0;
for i = 1:4
    mid = mid+motor(i);
end;
mid/4;

% Refresh motors
for i = 1:4
    motor(i) = motor(i)/motorFix;
end
motor(1) = map(motor(i), 0, mid, 0, target)