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
