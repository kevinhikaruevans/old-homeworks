Fs = 1e20; % sampling freq
t = 0:1/Fs:1e-13;
%f = 5;

tau = 10e-15;

w_0 = 2 * pi * 3.0e8 / 800e-9;
y = exp(-(t.^2) / (4 .* tau .^ 2)) .* cos(w_0 .* t);
plot(t, y)

%y = sin(2 * pi * t * f);
nfft = 2^16;

Y = abs(fft(y, nfft));
plot(Y)

%Y = Y(1:nfft/2);
x = (0:nfft-1)*Fs / nfft;

plot(x, Y);
