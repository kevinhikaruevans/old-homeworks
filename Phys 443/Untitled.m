Fs = 1000; % sampling freq
t = 0:1/Fs:1;
f = 5;
y = sin(2 * pi * t * f);
nfft = 2048;

Y = abs(fft(y, nfft));
Y = Y(1:nfft/2);
x = (0:nfft/2-1)*Fs / nfft;

plot(x, Y);
