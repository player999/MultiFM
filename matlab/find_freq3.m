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
T = 1/Fs;
N = pow2(20);
n = 3;
top_bw = 1000;
coef = 5;


station_frequencies = [-Fs/2:100e3:Fs/2];
station_frequencies = station_frequencies(2:end);

spec = zeros(1, length(station_frequencies));
timescale = (0:T:length(complex_data)/Fs)';
timescale = timescale(1:length(timescale) - 1);
amps = [];
for ii = 1:length(spec)
    reference_exp = zeros(size(timescale));
    for jj = -n:n
        freq = station_frequencies(ii) + (jj * (top_bw / (2 * n + 1)));
        reference_exp = reference_exp + exp(-complex(0,1)*2*pi*freq*timescale) / (2*N + 1); 
    end
    amplitude = abs(sum(complex_data .* reference_exp));
    amps = cat(1, amps, [amplitude]);
end

possible_stations = station_frequencies*(N/Fs) + (N/2);
stations = [];
for ii = 1:length(amps)-2
   lower = (amps(ii) + amps(ii + 2)) / 2;
   upper = amps(ii + 1);
   if (upper / lower) > coef
       stations = cat(1, stations, possible_stations(ii + 1));
   end
end
stations = double(stations);
stations = Cf + (stations - (N/2)) * (Fs/N);

