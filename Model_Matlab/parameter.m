%% Speed Controller

clear; clc;

%% Model and Parameter

Ts = 0.1;   % sample time
T = 30;     % simulation time

v_set = 10; % reference velocity

v_init = 0; % initial velocity

A_acc_lead = 5;%0.6; % Amplitude of sinusoidal acceleration (m/s^2)

amin_ego = -inf;%-1;  % min acceleration 
amax_ego = inf;%1;    % max acceleration

G_ego = tf(1,[0.5,1,0]);

G_discret = c2d(G_ego,Ts,'tustin'); % discretization
G_discret.Variable = 'z^-1'; % change appereance from z to 1/z

% Controller

%num = [5, 6.5];
%dem = [1, -1];
%C = tf(num,dem,Ts);
C = pid(5, 1.5, 'Ts', Ts);

c_loop = feedback(G_discret*C,1);
%% launch simulation

out = sim("Velocity_Controlled_Vehicle.slx",T);

%% Properties

ST = 0.05; % settling time threshold
RT = [0.0 0.9]; % rise time threshold

S = stepinfo(out.resp_v_c.signals.values, out.tout, 'SettlingTimeThreshold', ST, 'RiseTimeLimits', RT)

%% Plot

