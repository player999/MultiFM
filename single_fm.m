clear;
f = fopen('data100mhz.bin', 'r');
Fs = 20000000;
AFs = 44100;
station = 5e6;
decimator = 50;

raw_values = fread(f,'int8');
fclose(f);
mat = double(raw_values) + 1;

Q = mat(2:2:end);
I = mat(1:2:end);

[Im,Qm] = mixer_ml(Fs, station, I, Q);
[If,Qf] = filter_ml('lowpass', 256, Fs, 100e3, decimator, Im, Qm);
omega = atan_ml(If,Qf);
y = diff_ml(omega);
[z1,~] = filter_ml('lowpass', 256, Fs/decimator, AFs/2, (Fs/decimator)/AFs, y, y);
z1 = z1 .* 6000;
audio_encoder_ml('shit.mp3', z1);
