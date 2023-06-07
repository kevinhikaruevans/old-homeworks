% flatten the T, R, B matrices

T = [60 80 50 30 95 120 65 75 110 98 77 88];
R = [35 62 22 25 75 89 45 55 53 35 32 38];
B = [165 220 175 150 250 200 220 210 180 190 200 180] * 1e3;
% total variables = 36

% production
Pi = [10500 18800 13200];

% demand
Ci = [7700 9900 12200 11100];

% a giant value
M = 1e12;

% cost coefficients
c = [T R B];

% constraints:
Ae1 = [ones(1, 8)  zeros(1, 16)             zeros(1, 12);
      zeros(1, 8)  ones(1, 8)   zeros(1, 8) zeros(1, 12);
      zeros(1, 16) ones(1, 8)               zeros(1, 12)];

Ae2 = [1 0 0 0 1 0 0 0, 1 0 0 0 1 0 0 0, 1 0 0 0 1 0 0 0 zeros(1, 12);
       0 1 0 0 0 1 0 0, 0 1 0 0 0 1 0 0, 0 1 0 0 0 1 0 0 zeros(1, 12);
       0 0 1 0 0 0 1 0, 0 0 1 0 0 0 1 0, 0 0 1 0 0 0 1 0 zeros(1, 12);
       0 0 0 1 0 0 0 1, 0 0 0 1 0 0 0 1, 0 0 0 1 0 0 0 1 zeros(1, 12)];

Ae = [Ae1; Ae2];
be = [Pi'; Ci'];

A = [zeros(12, 12) eye(12) -M*eye(12)];
b = [zeros(12, 1)];

% actually gonna make the equality constraint an inequality
A = [A; -Ae];
b = [b; -be];

% set bounds
L = [zeros(1, 36)];
U = [Inf*ones(1, 24) ones(1, 12)]; % ensure binary bounds 

intcon = 1:36; % just make everything an integer

[x, z] = intlinprog(c', intcon, A, b, [], [], L, U);
x = ceil(x) %uh