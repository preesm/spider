clearvars


%revert order

a=[0:4095];
%for i=1:65535
 %   a(i)=i-1;
  %  y(i) = bitreverse(i-1,log2(16));
%end

y = bitrevorder(a);

for proc=0:p-1
    for i=0:N/p-1
        b(proc+1,i+1) = data_in(bitreverse(proc, log2(p))+i*p+1);
    end
end



%create a 2 dim array
matlab_br = [a;y]';

%import output of python, C implementations
py_br = csvread('/home/x0231061/git/compa_spider/sim/poutput.csv');
c_br = csvread('/home/x0231061/git/compa_spider/sim/coutput.csv');

%Check similarity between output of algorithms (c, python, matlab)
isequal(matlab_br, py_br)
isequal(matlab_br, c_br)
isequal(py_br, c_br)

%sort the array
%w = sortrows(v, 2);