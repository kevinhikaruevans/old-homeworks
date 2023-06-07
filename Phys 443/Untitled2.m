N = 10000;

dt = 1e-17;
t = (-N:N) * dt;
tau = 10e-15;

w_0 = 2 * pi * 3.0e8 / 800e-9;

f = exp(- (t.^2) / (4 .* tau .^ 2)) .* cos(w_0 .* t);

%plot(t, f)
F = abs(fft(f));

w = (-N:N) / dt;
plot(w, F)
%plot(t, f);

%%dw = 2*pi/dt;
%F2 = abs(F);
%F2 = F2 / max(F2);
%plot(F2)

%w = (-N:N) / dt;
%plot(w, F)