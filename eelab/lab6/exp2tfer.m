

%%%%%%%%%%%%%%%%%%%%%%%
% Exp. 2/3   %        %
C1 = 10e-9;  % farad  %
C2 =  5e-9;  % farad  %
R  = 281.35; % ohm    %
%%%%%%%%%%%%%%%%%%%%%%%


N = [0,            0, 1 / (R.^2 * C1 * C2)]; % numerator coefficients
D = [1, 2 / (R * C1), 1 / (R.^2 * C1 * C2)]; % denomenator coefficients

Hsys2 = tf(N, D);

step(Hsys2);
grid on;


Hbandpass = series(Hsys1, Hsys2);
bode(Hbandpass);
grid on;
figure;
step(Hbandpass);
grid on;