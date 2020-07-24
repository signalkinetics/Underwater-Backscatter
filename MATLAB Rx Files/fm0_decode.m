function bits = fm0_decode(signal,fm0_samps)
    s01 = transpose(generate_fm0_sig([0],fm0_samps));
    s00 = transpose(generate_fm0_sig([1],fm0_samps));
    s10 = transpose(generate_fm0_sig([0],fm0_samps));
    s10 = s10*-1;
    s11 = transpose(generate_fm0_sig([1],fm0_samps));
    s11 = s11+1;
    
    bits = [];
    count = 0;
    signal = signal-mean(signal);
    while(count<8)
        new_signal = signal(count*fm0_samps+1:(count+1)*fm0_samps);
        %corr01 = moving_corr(new_signal,s01);
        %corr01 = xcorr(new_signal,s01,0);
        corr01 = (1/length(new_signal))*sum((new_signal).*(s01));
        %corr10 = moving_corr(new_signal,s10);
        %corr10 = xcorr(new_signal,s10,0);
        corr10 = (1/length(new_signal))*sum((new_signal).*(s10));
        %corr11 = moving_corr(new_signal,s11);
        %corr11 = xcorr(new_signal,s11,0);
        corr11 = (1/length(new_signal))*sum((new_signal).*(s11));
        %corr00 = moving_corr(new_signal,s00);
        %corr00 = xcorr(new_signal,s00,0);
        corr00 = (1/length(new_signal))*sum((new_signal).*(s00));
        four_corr = [abs(corr01), abs(corr10), abs(corr00), abs(corr11)];
        index = find(four_corr==max(four_corr));
        if(index==[1 2])
            bits = [bits, 0];
        end
        if(index==[3 4])
            bits = [bits, 1];
        end
        count = count + 1;
    end   
end