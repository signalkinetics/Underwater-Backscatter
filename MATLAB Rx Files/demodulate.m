function bb_s_b = demodulate(s, Fs, fc, t)
    % carrier frequency multiplication
    clc
    %mult_s = [s'.*exp(1j*2*pi*fc.*t) zeros(1,70*length(s'.*exp(1j*2*pi*fc.*t)))]; % cos(2*pi*fc.*t)';
    mult_s = [s'.*exp(1j*2*pi*fc.*t) ];
    %mult_s2 =[s.*cos(2*pi*fc.*t)' ;zeros(70*length(s.*cos(2*pi*fc.*t)'),1)];
    mult_s2 =[s.*cos(2*pi*fc.*t)'];
    
    bin_size = Fs/length(mult_s);
    %f = 0:bin_size:(length(mult_s)-1)*bin_size;
    f = -(length(mult_s)-1)*bin_size/2:bin_size:(length(mult_s)-1)*bin_size/2;
     hold on; figure; plot(f, fftshift(abs(fft(mult_s2)))); title('2'); hold off;
     hold on; figure; plot(f, fftshift(abs(fft(mult_s)))); title('1'); hold off;
    %% Bandpass to get rid of low frequency water interferenace as well
    % LPF
    T = 1/Fs;
    f_cut_high = 1000;
    tau = 1/f_cut_high;
    a = T/tau;
    [b2, a2] = butter(6, f_cut_high/Fs/2);
    bb_s_b = abs(filter(b2, a2, mult_s)).';
%     bb_s_b = s;
%     bb_s_b2 = filter(b2, a2, mult_s2);
%     bb_s = filter(a, [1 a-1], mult_s);
%     bb_s2 = filter(a, [1 a-1], mult_s2);
    % HPF
%     f_cut_low = 5;
%     b = T*f_cut_low;
%     bb_s_b = abs(filter([1-b b-1],[1 b-1], bb_s_b)).';
%     bb_s_b2 = filter([1-b b-1],[1 b-1], bb_s_b2);
%     figure; plot(real(bb_s_b));
%     figure; plot(bb_s_b2);
%     hold on; figure; plot(f, fftshift(abs(fft(bb_s_b2)))); title('23'); hold off;
%     hold on; figure; plot(f, fftshift(abs(fft(bb_s_b)))); title('13'); hold off;%     figure; plot(bb_s);
    fprintf("LPF to %d\n", f_cut_high);

end

%% Legacy using fft and ifft
%     bin_size = Fs/length(mult_l);
%     f = 0:bin_size:bin_size*(length(mult_l)-1);
%     figure; plot(f, abs(fft(mult_l)));
%     % Bandpass filter setup
%     BPF_high_cutoff = 20000; %Hz
%     BPF_high_cutoff_i = int32(BPF_high_cutoff/bin_size);
%     % % We need to trim the water surface vibration
%     BPF_low_cutoff = 8 ;
%     BPF_low_cutoff_i = int32(BPF_low_cutoff/bin_size);
% 
% 
%     fft_l = fft(mult_l);
%     fft_l(BPF_high_cutoff_i+1:end-BPF_high_cutoff_i) = 0;
%     fft_l(1:BPF_low_cutoff_i) = 0;
%     fft_l(end-BPF_low_cutoff_i+1:end) = 0;
%     figure; plot(f, abs(fft_l));
%     demod_l = real(ifft(fft_l));