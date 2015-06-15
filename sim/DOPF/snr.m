function v = snr(sig,ref)
    v = 20*log10(norm(sig(:))/norm(sig(:)-ref(:)));
