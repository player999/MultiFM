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

stations = zeros(length(risings), 1);
offset = 1;
for ii = 1:length(risings)
    f1 = risings(ii);
    for jj=ii:length(fallings)
       if (fallings(jj) - risings(ii)) > 200
          f2 = fallings(jj);
          for kk = ii:length(fallings)
              if risings(kk) > f2
                 ii = kk; 
              end
          end
          f_relative = ((f2 + f1) / 2) - (N/2);
          f_res = f_relative * (Fs / N) + Cf;
          stations(offset) = f_res;
          offset = offset + 1;
          break;
       end
    end
end