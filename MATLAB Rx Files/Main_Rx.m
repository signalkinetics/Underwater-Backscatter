[data, Fs] = audioread('pwm_rx_signal.wav');

% Used to plot the carrier frequency
% z = fft(data);
% f = (0:length(z)-1)*Fs/length(z);
% plot(f,abs(z))

%Determining the time domain and processing the data
datanew = data(:,1);
time = 1/Fs:1/Fs:length(datanew)/Fs;

%%Extrating frquency
bin_size = Fs/length(data);
[~,i] = max(abs(fft(datanew)));
freq = (i-1)*bin_size;


%Demodulating the signal
dem = demodulate(datanew,Fs,freq,time);


%Period of an fm0 signal
fm0_period = 0.121;
fm0_samps = floor(fm0_period*Fs) + mod(floor(fm0_period*Fs), 2);

%Produces a sample FM0 signal and finds the start of the preamble
signal = transpose(generate_fm0_sig([1 0 1 1 0 1 1 1 0 1 0 0 1 0 0 0 1 0 1 0],fm0_samps));
start = find_back_start(dem,signal);

%Decodes the incoming FM0 signal
processed_data = fm0_decode(dem(start*100+(round(Fs*fm0_period*20)):end),fm0_samps)



%EXTRA TEST CODE
%s01 = transpose(generate_fm0_sig([0],fm0_samps));
%s00 = transpose(generate_fm0_sig([1],fm0_samps));
%s10 = transpose(generate_fm0_sig([0],fm0_samps));
%s10 = s10*-1;
%s11 = transpose(generate_fm0_sig([1],fm0_samps));
%s11 = s11+1;
%dem = dem(start*100+(round(Fs*fm0_period*20)):end);
%new_signal = dem(0*fm0_samps+1:(1)*fm0_samps);
%corr01 = xcorr(new_signal,s01,'biased',0);
%corr10 = xcorr(new_signal,s10,'biased',0);
%corr00 = xcorr(new_signal,s00,'biased',0);
%corr11 = xcorr(new_signal,s11,'biased',0);



