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

