function output = ifft_impl4(input) 
    output = conj(fft_impl4(conj(input))/length(input));
end
