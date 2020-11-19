function output = fft_impl(input)

fft_size = length(input);
fft_depth = log2(fft_size);
W = zeros(2^(fft_depth - 1),fft_depth);

for jj = 1:fft_depth
   for ii = 1:2^(jj - 1)
       W(ii,jj) = exp(complex(0,1)*2*pi*(ii-1)/(2 ^ jj));
   end
end

old_data = input;
for ii = 1:length(fft_size)
    old_data(ii) = input(bitrevorder(ii));
end
    
new_data = zeros(size(old_data));
for jj = 1:fft_depth
    offset = 1;
    offset2 = 2^(jj-1);
    offset3 = 2 ^ jj;
    step = 0;
    for ii = 1:(2^(fft_depth - jj))    
        for kk = 1:2^(jj - 1)
            new_data(offset) = old_data(step + kk) + W(kk,jj) * old_data(step + offset2 + kk);
            offset = offset + 1;
        end
        
        for kk = 1:2^(jj - 1)
            new_data(offset) = old_data(step + kk) - W(kk,jj) * old_data(step + offset2 + kk);
            offset = offset + 1;
        end
        step = step + offset3;
    end
    old_data = new_data;
end

output = old_data;
