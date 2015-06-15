clear;

N = 2^16;
p = 2^4;

seed = 2000;
rand('seed',seed);

logname = sprintf('FFT with %d samples ( %dKsamples)\n It will broken into %d FFTs of size %d + combining', N, N/1024,p,N/p)


aa= -2^15;
bb = 2^15;

data_in =  randi([aa bb],N,1) + 1i*randi([aa bb],N,1);
data_fft = fft(data_in);

tic;
% Data Ordering
for proc=0:p-1
    for i=0:N/p-1
        b(proc+1,i+1) = data_in(bitreverse(proc, log2(p))+i*p+1);
    end
end

% Local FFT
for proc=0:p-1
    b(proc+1,:) = fft(b(proc+1,:));
end

% DOPF
Count = 0;
for e = 0:log2(p)-1
    for proc = 0:p-1
        if(bitand(proc, 2^e) == 0)
            recvProc = proc + 2^e;
            for k=0:N/p-1
                tmp = b(proc+1,k+1);
                
                q = 2^(e+1+log2(N/p));
                m = mod(proc*N/p+k,q);
                z = exp(-2*1i*pi*m/q);
                
                b(proc+1,k+1) = tmp + b(recvProc+1,k+1) * z;
                b(recvProc+1,k+1) = tmp - b(recvProc+1,k+1) * z;
            end
            sprintf('e=%d,proc=%d,recvProc=%d\t b(%d,1..%d) and b(%d,1..%d) \n', e,proc,recvProc,recvProc+1,N/(p),proc+1,N/p);
        Count = Count +1;
        end
    end
end

% Pack data (no cost)
for proc = 0:p-1
    data_out(proc*N/p+1:(proc+1)*N/p) = b(proc+1,:);
end

toc
sprintf('Count: %d \n', Count)
Resultname = sprintf('SNR: %f dB\n', snr(data_fft, data_out))
%v = 20*log10(norm(data_out(:))/norm(data_out(:)-data_fft(:)))


