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

function output = fft_impl2(input, nn, isign)
    data = zeros(1,length(input)*2);
    data(1:2:end) = real(input);
    data(2:2:end) = imag(input);
    n = 2 * nn;
    j = 1;
    for i = 1:2:n
       if j > i
          tempr = data(j); 
          tempi = data(j + 1);
          data(j) = data(i);
          data(j + 1) = data(i + 1);
          data(i) = tempr;
          data(i + 1) = tempi;
       end
       m = nn;
       while (m >= 2) && (j > m)
          j = j - m;
          m = m / 2;
       end
       j  = j + 2;
    end
    
    mmax = 2;
    
    while n > mmax
       istep = 2 * mmax;
       theta = 2 * pi / (isign * mmax);
       wpr = -2*sin(0.5*theta)*sin(0.5*theta);
       wpi = sin(theta);
       wr = 1;
       wi = 0;
       
       for m = 1:2:mmax
           for i=m:istep:n
               j = i + mmax;
               tempr = wr*data(j) - wi*data(j+1);
               tempi = wr*data(j+1) + wi*data(j);
               data(j) = data(i)-tempr;
               data(j+1) = data(i+1) - tempi;
               data(i) = data(i) + tempr;
               data(i+1) = data(i+1) + tempi;
           end
           wtemp=wr;
           wr=wr*wpr-wi*wpi+wr;
           wi=wi*wpr+wtemp*wpi+wi;
       end
       mmax=istep;
    end
    output = complex(data(1:2:end), data(2:2:end));
end



