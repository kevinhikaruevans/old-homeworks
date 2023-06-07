% reusing the transfer function from Part II.
dt = 5e-6;
t  = (0:dt:20e-3);
h  = 1307.4886 .* exp(-6096.13 .* t) ...
    - 2 .* 480.725 .* exp(-285.2701.*t) .* cos(2725.7171.*t + deg2rad(60.45));

% input functions:
u  = ones(1, 2 * length(t));       % unit step, filled with 1
d  = [1/dt zeros(1, length(t)-1)]; % impulse, a spike then filled with 0

% numerically find the convolutions for both
s  = conv(h, u) * dt; % unit step
s2 = conv(h, d) * dt; % impulse

figure;
plot(t, s(1:length(t)));
title('Step response with conv()');
xlabel('Time (s)');
ylabel('Amplitude (V)');
grid on;

figure;
plot(t, s2(1:length(t)));
title('Impulse response with conv()');
xlabel('Time (s)');
ylabel('Amplitude (V)');
grid on;