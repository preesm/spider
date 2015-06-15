clear;

N = 2^16;
p = 2^12;

seed = 2000;
rand('seed',seed);

logname = sprintf('FFT with %d samples ( %dKsamples)\n It will broken into %d FFTs of size %d + combining', N, N/1024,p,N/p)


aa= -2^15;
bb = 2^15;

data_in =  randi([aa bb],N,1) + 1i*randi([aa bb],N,1);
data_fft = fft(data_in);

tic;
% Data Ordering bis
 %instead of 16*4096, generate a 4096*16 matrix
for proc=0:p-1
    for i=0:N/p-1
        b(proc+1,i+1) = data_in(bitreverse(proc, log2(p))+i*p+1);
    end
end



% Local FFT
b1 = conj(b)';
p = 2^4;
for proc=0:p-1   %instead of passing lines to the FFT, pass columns
    b1(proc+1,:) = fft(b1(proc+1,:));
end

b1 = conj(b1)';
p = 2^12;

% DOPF
Count = 0;
for e = 0:log2(p)-1
    e1 = ceil((e+1)/(log2(p)/log2(N/p)))-1;
    
    for proc = 0:p-1
        if(bitand(proc, 2^e1) == 0)
            recvProc = proc + 2^e;
            for k=0:p-1
                tmp = b(proc+1,k+1+e1-1);
                
                q = 2^(e1+1+log2(p));
                m = mod(proc*p+k,q);
                z = exp(-2*1i*pi*m/q);
                
                b(proc+1,k+1+e1) = tmp + b(recvProc+1,k+1+e1) * z;
                b(recvProc+1,k+1+e1) = tmp - b(recvProc+1,k+1+e1) * z;
            end
            sprintf('e=%d,proc=%d,recvProc=%d\t b(%d,1..%d) and b(%d,1..%d) \n', e,proc,recvProc,recvProc+1,N/(p),proc+1,N/p);
        Count = Count +1;
        end
    end
end

% Pack data (no cost)
for proc = 0:p-1
    data_out(proc*N/p+1:(proc+1)*N/p) = b1(proc+1,:);
end

toc
sprintf('Count: %d \n', Count)
Resultname = sprintf('SNR: %f dB\n', snr(data_fft, data_out))
%v = 20*log10(norm(data_out(:))/norm(data_out(:)-data_fft(:)))


