f = fopen('data100mhz.bin', 'r');
Fs = 20000000;
AFs = 44100;
decimator = 50;

raw_values = fread(f,'int8');
fclose(f);
mat = double(raw_values) + 1;

Q = mat(2:2:end);
I = mat(1:2:end);

[If,Qf] = filter_ml('lowpass', 256, 20e6, 100e3, decimator, I, Q);
omega = atan_ml(If,Qf);
y = diff_ml(omega);
z1 = downsample(y, round((Fs/decimator)/(AFs)));

sound(z1, AFs);
