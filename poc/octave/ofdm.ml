samp_rate    = 44100; % sample rate
sym_tab_qam  = 0.35355339059*[
                -2+2j,-1+2j,1+2j,2+2j,
                -2+1j,-1+1j,1+1j,2+1j,
                -2-1j,-1-1j,1-1j,2-1j,
                -2-2j,-1-2j,1-2j,2-2j];

sym_tab = sym_tab_qam;
% baseband symbols
%  A B | C D
%  E F | G H
%  ----+-----
%  I J | K L
%  M N | O P

SA  = sym_tab(1);
SB  = sym_tab(2);
SC  = sym_tab(3);
SD  = sym_tab(4);
SE  = sym_tab(5);
SF  = sym_tab(6);
SG  = sym_tab(7);
SH  = sym_tab(8);
SI  = sym_tab(9);
SJ  = sym_tab(10);
SK  = sym_tab(11);
SL  = sym_tab(12);
SM  = sym_tab(13);
SN  = sym_tab(14);
SO  = sym_tab(15);
SP  = sym_tab(16);

S0 = SJ;
S1 = SG;
PP = SF;

symbol_tx = [SF SG SJ SK SF SG SJ SK SG SJ SK SF SG SJ SK SF SJ SK SF SG SJ SK SF SG SK SF SG SJ SK SF SG SJ];
used_sc   = 8; % Used Subcarriers
fft_size  = 64;
cp_len    = 16; % Cyclic Prefix Length
sym_rate  = samp_rate/(fft_size+cp_len); % Symbol Rate
sc_in_use = 8;
bw        = used_sc*sym_rate; % Bandwidth
bps       = 2; %  Bits Per Subcarrier Symbol
fc        = 7500; % Carrier Frequency

printf('Symbol Rate %d\n', sym_rate)
printf('Bandwith %d\n', bw)
printf('Total Bit Rate %d\n', bw*bps)
tx_baseband = ofdmEncode(symbol_tx,sc_in_use,fft_size,cp_len);

tmp = length(tx_baseband)-1;
carrier = e.^(j*2*pi*fc*[0:1:tmp]/samp_rate);
upconverted_baseband = real(carrier).*real(tx_baseband).+(j*imag(carrier).*imag(tx_baseband));
upconverted_baseband_real  = real(upconverted_baseband).+ imag(upconverted_baseband);
upconverted_baseband_real = upconverted_baseband_real.+0.*rot90(noise(length(upconverted_baseband_real),'white'));
% Channel Condition
% channel_response = [1 0.5 0.3 0.1 0.01 0];
% channel_response = [zeros(1,length(channel_response)-1) channel_response];
% upconverted_baseband_real = conv(channel_response, upconverted_baseband_real);
% upconverted_baseband_real = upconverted_baseband_real(length(channel_response)/2:length(upconverted_baseband_real)-length(channel_response)/2);

% Magic Lowpass
f1 = fc;
f2 = f1+500;
dB  = 40;
N = dB*samp_rate/(22*(f2-f1));
fq =  [f1]/(samp_rate/2);
rx_lowpass_kernel = fir1(round(N)-1, fq,'low');
printf('Filter length: %d \n',length(rx_lowpass_kernel))

carrier_period_sample_sz = samp_rate/fc;
s = (0/360)*(fc/carrier_period_sample_sz);
rx_carrier    = e.^(j*2*pi*(fc)*[s:1:s+tmp]/samp_rate);
rx_baseband_unfiltered = rx_carrier.*upconverted_baseband_real;
rx_baseband = conv(rx_lowpass_kernel,rx_baseband_unfiltered);

rx_baseband_synched = rx_baseband(length(rx_lowpass_kernel)/2:length(rx_baseband)-length(rx_lowpass_kernel)/2); % Magic Symbol Synchronization
rx_baseband_synched_ol = length(rx_baseband_synched);
rx_baseband_synched = interpft(rx_baseband_synched,rx_baseband_synched_ol*2);
rx_baseband_synched = circshift(rx_baseband_synched,-1,2); % Magic Sampling Timing Correction
rx_baseband_synched = interpft(rx_baseband_synched,rx_baseband_synched_ol);

symbol_rx = ofdmDecode(rx_baseband_synched,sc_in_use,fft_size,cp_len,4);

figure(1,'position',get(0,'screensize').*0.8);

subplot(2,4,3);
plotConstellations(symbol_tx(1:32),repmat([RED;GREEN;BLUE;YELLOW;CYAN;MAGENTA;DBLUE;DCYAN],4,1));
title('Transmitted Symbols Constellation');
axis('square');
xlabel('I');
ylabel('Q');
grid on;

subplot(2,4,4);
plotConstellations(symbol_rx(1:32),repmat([RED;GREEN;BLUE;YELLOW;CYAN;MAGENTA;DBLUE;DCYAN],4,1));
title('Received Symbols Constellation');
axis('square');
xlabel('I');
ylabel('Q');
grid on;

timesigax0 = subplot(4,2,1);
plotComplex(tx_baseband);
title('OFDM Baseband');
xlabel('Sample');
ylabel('Amplitude');

timesigax1 = subplot(4,2,3);
plot(real(interpft(upconverted_baseband_real,length(upconverted_baseband_real)*10)));
title(sprintf('Modulated Carrier (%dHz)', fc));
xlabel('Sample (Interpolated x10)');
ylabel('Amplitude');

timesigax2 = subplot(4,2,5);
plotComplex(interpft(rx_baseband_unfiltered,length(rx_baseband_unfiltered)*10));
title('Demodulated OFDM Baseband Unfiltered');
xlabel('Sample');
ylabel('Amplitude');

timesigax3 = subplot(4,2,7);
plotComplex(rx_baseband_synched);
title('Demodulated OFDM Baseband LP Filtered');
xlabel('Sample');
ylabel('Amplitude');

linkaxes([timesigax0, timesigax3])
linkaxes([timesigax1, timesigax2])

subplot(4,4,11)
plotComplex(symbol_tx);
title('Trasmitted Symbols');
subplot(4,4,15)
plotComplex(symbol_rx);
title('Received Symbols');

subplot(4,4,12);
[ax l1 l2] = plotFft(fftshift(fft([channel_response zeros(1,1000)])),44100);
title('Channel Frequency Resopnse');
xlabel('Frequency (Hz)');
ylabel(ax(1),'Gain (dB)');
ylabel(ax(2),'Phase (Radians)');

subplot(4,4,16);
plotComplex(interpft(channel_response,length(channel_response)*1000));
title('Channel Impulse Resopnse');
xlabel('Sample (Interpolated x1000)');
ylabel('Amplitude');
