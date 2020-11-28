clear;
f = fopen('../station_search_i.bin', 'r');
stations_i = fread(f,'double');
fclose(f);
f = fopen('../station_search_q.bin', 'r');
stations_q = fread(f,'double');
fclose(f);
complex_data = complex(stations_i, stations_q);

Cf = 100e6;
%Fs = 1008000;
Fs = 10e6;
N = pow2(20);

possible_stations = uint32([-Fs/2:100000:Fs/2]*(N/Fs) + (N/2));
possible_stations = possible_stations(2:end);

spectrum = abs(fftshift(fft(complex_data)));
spectrum_mean = movmean(spectrum,1001);
station_amplitudes = spectrum_mean(possible_stations);
stations = [];
for ii = 1:length(station_amplitudes)-2
   lower = (station_amplitudes(ii) + station_amplitudes(ii + 2)) / 2;
   upper = station_amplitudes(ii + 1);
   if (upper / lower) > 5
       stations = cat(1, stations, possible_stations(ii + 1));
   end
end
stations = double(stations);
stations = Cf + (stations - (N/2)) * (Fs/N);

