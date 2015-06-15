
function r = bitreverse(v, N)
    for i=1:length(v)
        r(i) = 0;
        for n=1:N
            r(i) = floor(r(i)*2);
            r(i) = r(i) + bitand(v(i), 1);
            v(i) = floor(v(i)/2);
        end 
    end
end