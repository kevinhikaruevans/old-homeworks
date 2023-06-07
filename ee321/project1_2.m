% coefficients of the transfer function:
% order:
% s^   3,       2,        1,          0
N = [0.5, 4166.67,  5494500, 4.57875e10]; % numerator
D = [  1, 6666.67, 10989000, 4.57875e10]; % denominator

% generate transfer function, H(s)
H            = tf(N, D);
[z, p,    k] = zpkdata(H, 'v');  % zeros, poles, gain
[r, ptmp, k] = residue(N, D);    % residues r -> are the K_i values

% print out the residues over the poles
for i = 1 : 3
   fprintf("\n%d -> %s\n\t----------------------\n\t%s\n", ...
    i, num2str(r(i)), num2str(p(i)) );
end


dt = 5e-6;
t  = (0:dt:20e-3);
y  = 1307.4886 .* exp(-6096.13 .* t) ...
    - 2 .* 480.725 .* exp(-285.2701.*t) .* cos(2725.7171.*t + deg2rad(60.45));

figure;
plot(t, y);
title('Impulse response');
xlabel('Time (s)');
ylabel('Amplitude (V)');
grid on;

% step response
% Y = (1 / s) H(s)
