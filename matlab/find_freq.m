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
