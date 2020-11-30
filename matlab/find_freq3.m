%{
Copyright 2020 Taras Zakharchenko taras.zakharchenko@gmail.com

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1.  Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
3.  Neither the name of the copyright holder nor the names of its contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
%}

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

