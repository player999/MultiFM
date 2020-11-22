%List of all radio frequencies
clear;
f = fopen('../data100mhz.bin', 'r');
N = pow2(20);
Fs = 20000000;
Cf = 100000000;
raw_values = fread(f,'int8');
fclose(f);

mat = double(raw_values) + 1;
mat = complex(mat(1:2:end), mat(2:2:end));
mat = mat(1:N);

clear raw_values;
clear f;

spectrum = abs(fftshift(fft(mat)));
spectrum_mean = movmean(spectrum,1001);
spectrum_sq = movmean(spectrum,1001) > 15000;
spectrum_diff = diff(spectrum_sq);

risings = find(spectrum_diff == 1);
fallings = find(spectrum_diff == -1);
widths = fallings - risings;
idxs = find(((widths > 500) & (widths < 10000)) == 1);
risings = risings(idxs);
fallings = fallings(idxs);
stations = Cf + (((risings + fallings) / 2) - (N/2)) * (Fs/N);
