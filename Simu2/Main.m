%% Based on research made by:
% http://sal.aalto.fi/publications/pdf-files/eluu11_public.pdf

clear all;
% Initial variables
position = [0; 0; 10]; % Position
angles = [-180; -180; -180]; % Angles

targetZ = 10; % Target height
targetAngles = [0; 0; 0]; % Target orientation

% Advanced initial variables
velocity = [0; 0; 0]; % Velocity
acceleration = [0; 0; 0]; % Acceleration
Dangles = [0; 0; 0]; % Angular velocities
DDangles = [0; 0; 0]; % Angular acceleration

%% PID
% P controls how intense the controlling is.
% D smooths out the oscillation caused by P.
global Kdz Kdroll Kdpitch Kdyaw Kpz Kproll Kppitch Kpyaw Kiroll Kipitch Kiyaw Kiz;
Kdz = 3;
Kdroll = 0.05;
Kdpitch = Kdroll;
Kdyaw = 0;

Kpz = 3;
Kproll = 0.1;
Kppitch = Kproll;
Kpyaw = 0;

Kiroll = 0;
Kipitch = 0;
Kiyaw = 0;
Kiz = 1;

%% Static variables
global Ixx Iyy Izz;
Ixx = 5; % Grams instead of kg
Iyy = 5;
Izz = 1;
I = [Ixx 0 0; 0 Iyy 0; 0 0 Izz]; % Inertial matrix

global m g k L b;
m = 1000; % mass in grams
g = -9.81; % gravity
k = 0.5109; % lift/rpm - grams
L = 0.2; % Distance from motor to center
b = 1 * 10^-2; % Drag coefficient

% Simulation times, in seconds.
start_time = 0;
end_time = 10;
global dt;
dt = 0.001; % 0.005
times = start_time:dt:end_time;
N = numel(times);

% Preallocate variables for analysing purposes.
POS = zeros(3,N);
ANGLE = zeros(3,N);
DANGLE = zeros(3,N);
ANGACC = zeros(3,N);
W = zeros(4,N);
iStats = 1;

prevAngles = angles;
prevZ = position(3);

pass = 0;
maxPass = numel(times);
passDivider = round(maxPass/10);
for t = times
    pass = pass+1;
    if(rem(pass, passDivider) == 0)
        fprintf('Pass: %d/%d\n', pass, maxPass);
    end
    % Select controller:
    % [w1, w2, w3, w4] = [0,0,0,0];
    
    %     [w1, w2, w3, w4] = Z_Control( angles, maxThrust );
%     
%     [w1, w2, w3, w4] = PD_Control(position, angles, velocity, Dangles,...
%         targetZ, targetAngles);
    
    % Estimate angular speeds from angles.
    estAngVelo = angles - prevAngles;
    for i = 1:numel(estAngVelo)
        if(estAngVelo(i) < -180)
            estAngVelo(i) = estAngVelo(i) + 360;
        end
        if(estAngVelo(i) > +180)
            estAngVelo(i) = estAngVelo(i) - 360;
        end
    end
    estAngVelo = estAngVelo / dt;
    estZvelo = (position(3) - prevZ) /dt;
        
    prevAngles = angles;
    prevZ = position(3);
    [w] = Real_PD_Control(angles, estAngVelo, position(3), estZvelo, targetAngles, targetZ);
%     [w] = Real_PD_Control(angles, Dangles, position(3), velocity(3), targetAngles, targetZ);
    %     prevAngles = Dangles;
    %     prevZ = position(3);
    
    w = Throttle2Speed(w);
    w1 = w(1);
    w2 = w(2);
    w3 = w(3);
    w4 = w(4);
    W = CollectStats3(W,w,iStats);
    W(4,iStats) = w(4);
    
    
    % Compute linear and angular accelerations.
    acceleration = Acceleration(w1, w2, w3, w4, angles);
    velocity = velocity + acceleration*dt;
    position = position + velocity*dt;
    
    torques = Torque(w1, w2, w3, w4);
    DDangles = AngularAcceleration(torques);
    Dangles = Dangles + (DDangles*dt);
    angles = angles+(Dangles*dt);
    angles = wrapTo180(angles);
    
    % Save statistical variables
    %     DrawCopter(pos,angles);
    POS = CollectStats3(POS, position, iStats);
    ANGLE = CollectStats3(ANGLE, angles, iStats);
%     DANGLE = CollectStats3(DANGLE, Dangles, iStats);
%     ANGACC = CollectStats3(ANGACC, DDangles, iStats);
    
    iStats = iStats+1;
end
% DrawCopter_RT(POS, ANGLE, dt);
DrawStats3(times, POS, 'Positions', 'X', 'Y', 'Z - altitude');
DrawStats3(times, ANGLE, 'Orientation', 'Roll', 'Pitch', 'Yaw');
% DrawStats3(times, DANGLE, 'Angular velocity', 'DRoll', 'DPitch', 'DYaw');
% DrawStats3(times, ANGACC, 'Angular acceleration', 'DDRoll', 'DDPitch', 'DDYaw');
figure;
hold on;
plot(times,W(1,:), 'r');
plot(times,W(2,:), 'g');
plot(times,W(3,:), 'b');
plot(times,W(4,:), 'y');
title('Motors');
legend('m1', 'm2', 'm3', 'm4')
hold off;


