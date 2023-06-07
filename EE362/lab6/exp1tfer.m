L = 1e-3;    % henry
C = 63.3e-9; % farad
R = 50.26;   % ohm

N = [1, 0,     0      ]; % numerator coefficients
D = [1, (R/L), 1/(L*C)]; % denomenator coefficients

Hsys1 = tf(N, D);
step(Hsys1);
grid;
figure;
bode(Hsys1);
grid;