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

FFT_SIZE = 8;
%input_data = complex(I(1:FFT_SIZE), Q(1:FFT_SIZE));
original_input_data = complex([0.3535, 0.6464, 0.3535, -1.3535, 0.3535, 1.0607, -1.0607, -0.3535]);
processed_input_data = complex([0.3535, 0.3535, 0.6464, -1.3535, 0.3535, -1.0607, 1.0607, -0.3535]);
input_data = processed_input_data;

fft_depth = log2(FFT_SIZE);
W = complex(zeros(FFT_SIZE, fft_depth));
add_indices = zeros(FFT_SIZE, fft_depth);
mult_indices = zeros(FFT_SIZE, fft_depth);

for j = 1:fft_depth
    num_coef = 2 ^ (j - 1);
    for i = 1:FFT_SIZE
        k = mod(i-1, num_coef);
        N = num_coef * 2;
        w = exp(complex(0,-1)*2*pi*k / N);
        %display(sprintf('%d %d', k, N));
        if bitand(i - 1, bitsll(1,j - 1)) > 0
            w = -w;
        end
        W(i, j) = w;
        period = 2^(j - 1);
        a = floor((i - 1) / (period * 2)) * (period * 2);
        add_indices(i, j) = mod(i - 1, period) + a;
        mult_indices(i,j) = add_indices(i, j) + period;
    end
end

old_data = input_data;
new_data = complex(zeros(size(old_data)));
for j = 1:fft_depth
    for i = 1:FFT_SIZE
        new_data(i) = old_data(mult_indices(i,j) + 1) * W(i,j) + old_data(add_indices(i,j) + 1);
    end
    old_data = new_data;
end
output_data = old_data;
