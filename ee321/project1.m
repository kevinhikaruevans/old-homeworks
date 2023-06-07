A = [ -4166.67,   4166.67,    0;
       2500.00,  -2500.00, -100;
          0.00, 109890.10,    0];  
B = [83.33;
     50.00;
      0.00];
C = [25, -25, 0];
D = [0.5];

% solve system and system->tf
system = ss(A, B, C, D);
H      = tf(system);

% plot a bode plot
bode(H);

% plot: impulse, step, and bode plots
%subplot(2, 1, 1);
figure;
impulse(H);
grid on;

%subplot(2, 1, 2);
figure;
step(H);
grid on;

%subplot(4, 1, [3, 4]);
figure;
bode(H);
grid on;

