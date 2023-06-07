% flatten the T, R, B matrices
T = [60 80 50 30 95 120 65 75 110 98 77 88];
R = [35 62 22 25 75 89 45 55 53 35 32 38];
B = [165 220 175 150 250 200 220 210 180 190 200 180] * 1000;

% total variables = 36

% the decision vars will be formatted as a 1D array like:
% [ t_{11}, ...,
%   r_{11}, ...,
%   b_{11}, ... ]
% where t=truck costs, r=rail costs, b=rail fee

% production to meet
Pi = [10500 18800 13200];

% demands to meet
Ci = [7700 9900 12200 11100];

% a giant value
M = 1e12;

% cost coefficients
c = [T R B];

% constraints:
% production:
%   - add up the individual qty per factory
Ae1 = [
        1 1 1 1 0 0 0 0 0 0 0 0, 1 1 1 1 0 0 0 0 0 0 0 0, zeros(1, 12);
        0 0 0 0 1 1 1 1 0 0 0 0, 0 0 0 0 1 1 1 1 0 0 0 0, zeros(1, 12);
        0 0 0 0 0 0 0 0 1 1 1 1, 0 0 0 0 0 0 0 0 1 1 1 1, zeros(1, 12);
      ];

% demand:
%   - add up the qty per center
Ae2 = [1 0 0 0 1 0 0 0 1 0 0 0, 1 0 0 0 1 0 0 0 1 0 0 0, zeros(1, 12);
       0 1 0 0 0 1 0 0 0 1 0 0, 0 1 0 0 0 1 0 0 0 1 0 0, zeros(1, 12);
       0 0 1 0 0 0 1 0 0 0 1 0, 0 0 1 0 0 0 1 0 0 0 1 0, zeros(1, 12);
       0 0 0 1 0 0 0 1 0 0 0 1, 0 0 0 1 0 0 0 1 0 0 0 1, zeros(1, 12)];

% concat the equality constraints
Ae = [Ae1; Ae2];
be = [Pi'; Ci'];

% r_{ij} <= M*b_{ij}
% => r_{ij} - M*b_{ij} <= 0
A = [zeros(12, 12) eye(12) -M*eye(12)];
b = [zeros(12, 1)];

% set bounds
% [0, inf) for the truck and rail
% [0, 1] for the binary var
L = zeros(1, 36);
U = [inf*ones(1, 24)  ones(1, 12)];

% lp relaxation (it fails):
% [x, z] = linprog(c', A, b, Ae, be, L, U);

intcon = 1:36; % just make everything an integer
[x, z] = intlinprog(c', intcon, A, b, Ae, be, L, U);
format long;
disp(x)