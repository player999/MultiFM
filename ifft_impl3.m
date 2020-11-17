function output = ifft_impl3(input)
    N = length(input);
    output = zeros(size(input));
    for mm = 1:N
        for nn = 1:N
            output(mm) = output(mm) + input(nn)*exp(complex(0,1)*2*pi*(nn-1)*(mm-1)/N);
        end
        output(mm) = output(mm) / N;
    end
end
