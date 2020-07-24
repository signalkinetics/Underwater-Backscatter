fr = 17700; % resonant frequency
Fs = 44100; % sampling frequency
t = 0:1/Fs:300*pi;
%determine frequency
mcu_period = 0.1; %half_period
n = 44100*mcu_period; %half_period

x = sin(2*pi*t.*fr);

square = [1 1 1 1  0 1 1  0 1 1 1 1  0 1 1 1 1  0 1 1  0 1 1 1 1  0 1 1  0 1 1 0]; %two ones is a one and 1 one is a zero

y = repelem(square,round(n)); %square signal with desired frequency

%make the square signal as long as the sine
number_of_repetition_of_signal = round(length(x)/(length(square)*round(n)))-1;
y1 = repmat(y,1,number_of_repetition_of_signal);
additions = length(x)-length(y1);
y2 = repmat(0,1,additions); %adds a bunch of zeros at the end of the signal to match lenght

y3 = [y1 y2];

z = y3.*x;
zeros_m = zeros(1,100*n/4);
pwm = z(1:length(square)*n);
figure(1)
plot(pwm);
%signal_to_close_loop
%z = [zeros_m, z1, zeros_m, z1, zeros_m, z1, zeros_m, z1, zeros_m, z1, zeros_m, z1 ];
%figure(2)
z2 = [x(1:25*41100) ,zeros_m, pwm, zeros_m, x]; %PWM with sinusoidal signal
plot(z2)
%generate sound
%new signal
%new_s = [x(1:41100*13), zeros_m(1:4357), z(1:137903),x]; %zeros_m, x(1:41100*100)
%sound(z2,Fs);
sound(z, Fs);
%clear sound
figure(3);
plot(z);
ylim([-10 10]); 


