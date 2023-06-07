% coefficients of the transfer function:
% order:
%     s^   3,       2,        1,          0
N     = [0.5, 4166.67,  5494500, 4.57875e10]; % numerator
D     = [  1, 6666.67, 10989000, 4.57875e10]; % denominator
Nstep = [0 N];
Dstep = [D 0]; % inc the order of D(s)

% generate transfer function, H(s)
H            = tf(Nstep, Dstep);
[z, p,    k] = zpkdata(H, 'v');       % zeros, poles, gain
[r, ptmp, k] = residue(Nstep, Dstep); % residues r -> are the K_i values

% print out the residues over the poles
for i = 1 : 4
   fprintf("\n%d -> %s\n\t----------------------\n\t%s\n", ...
    i, num2str(r(i)), num2str(p(i)) );
end


dt = 5e-6;
t  = (0:dt:20e-3);
y = 1.0 + -0.2145 .* exp(-6.0961e3 .* t) ...
    + 2 .* 0.175429 .* exp(-0.2853e3 .* t) .* cos(2.7257e3 .* t + deg2rad(144.489));
figure;
plot(t, y);
title('Step response');
xlabel('Time (s)');
ylabel('Amplitude (V)');
grid on;

% step response
% Y = (1 / s) H(s)

