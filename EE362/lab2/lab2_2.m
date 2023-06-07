L   = 1.146e-3;
R_L = 2.23;

R_1 = 67.42;
R_2 = 1.20;
C   = 9.59e-6;

R_S = 50;

A = [-1/(C*(R_S+R_1)) -1/C;
      1/L             -((R_2 + R_L)/L) ];
  
B = [ 1/(C*(R_S+R_1));
     0               ];
C = [1 0;
     0 1];
D = [0;
     0];

sys     = ss(A,B,C,D);
[Y,T,X] = step(sys);

subplot(2, 2, 1);
plot(T, 10*X(:, 1));
xlabel('time [s]');
ylabel('x_1(t) [V]');
grid on;

subplot(2, 2, 3);
plot(T, 10*X(:, 2));
xlabel('time [s]');
ylabel('x_2(t) [A]');
grid on;

subplot(2, 2, [2, 4]);
plot(10*X(:,1), 10*X(:,2));
xlabel('x_1(t) [V]');
ylabel('x_2(t) [A]');
grid on;
