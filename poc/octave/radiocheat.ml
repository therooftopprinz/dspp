QPSK
-----------------------
samp_rate    = 44100; % sample rate
sps          = 8; % sample per symbol
cps          = 1; % cyles per symbol
sym_rate     = samp_rate/sps; % symbol rate
sym_tab_qpsk = [1+j,-1+j,-1-j,1-j].*0.707106781;
sym_tab_qam  = 0.35355339059*[
                -2+2j,-1+2j,1+2j,2+2j,
                -2+1j,-1+1j,1+1j,2+1j,
                -2-1j,-1-1j,1-1j,2-1j,
                -2-2j,-1-2j,1-2j,2-2j];
bps          = log(length(sym_tab))/log(2); % bit per symbol
w            = 2*pi*cps/sps;

assert(cps*sym_rate+sym_rate/2 <=samp_rate/2,Max Bandwith Past Nyquist!!')
assert(cps*sym_rate-sym_rate/2 >=0 ,'Max Bandwith Below Zero')

printf('Carrier frequency %d Hz\n', cps*sym_rate);
printf('Upper Side Band is %d Hz\n',cps*sym_rate+sym_rate/2);
printf('Lower Side Band is %d Hz\n',cps*sym_rate-sym_rate/2);
printf('Bandwidth is %d Hz\n',sym_rate);5
printf('Max Bit Rate %d \n', bps*sym_rate);

sym_tab = sym_tab_qam
% baseband symbols
a = repmat(sym_tab(1),1,sps);
b = repmat(sym_tab(2),1,sps);
c = repmat(sym_tab(3),1,sps);
d = repmat(sym_tab(4),1,sps);
ee = repmat(sym_tab(5),1,sps);
f = repmat(sym_tab(6),1,sps);
g = repmat(sym_tab(7),1,sps);
h = repmat(sym_tab(8),1,sps);
ii = repmat(sym_tab(9),1,sps);
jj = repmat(sym_tab(10),1,sps);
k = repmat(sym_tab(11),1,sps);
l = repmat(sym_tab(12),1,sps);
m = repmat(sym_tab(13),1,sps);
n = repmat(sym_tab(14),1,sps);
o = repmat(sym_tab(15),1,sps);
p = repmat(sym_tab(16),1,sps);

% databsb = 0.707106781.*[f g jj k];
databsb = [a d m p f g jj k b c ee ii n o l h];
bsb_fr = [1 0 0 0 0 0 0];
fk = genFilter(bsb_fr);
databsb_o = databsb;
databsb = conv(databsb,fk);

printf('Data length %d samples\n', length(databsb));

tmp = length(databsb)-1;
carrier = e.^(j*w*[0:1:tmp]);
datamod = complexMixer(carrier,databsb));
datamodx = real(datamod).+ imag(datamod);

plot(
     %real(datamod),'-b',
     %imag(datamod),'-r',
     datamodx,'-k',
     real(databsb),'-c',
     imag(databsb),'-m'); % print modulated data
wavwrite(datamodx,samp_rate,'datamodx.wav');

fk = genFilter(bsb_fr);
pstart = 0;
pend = pstart + length(datamodx)-1;
dcarrier = e.^(j*w*[pstart:1:pend]);
datadmod = datamodx.*dcarrier;
datadmod = conv(fk,datadmod);

subplot(2,1,1)
plotComplex(databsb);
subplot(2,1,2)
plotComplex(datadmod);

plot(
     real(carrier),'-b',
     imag(carrier),'-r',
     real(dcarrier),'-c',
     imag(dcarrier),'-m'); % print carrier

FILTER
-----------------------
f1 = 10000;
f2 = 15000;
delta_f = f2-f1;
Fs = 192000;
dB  = 40;
N = dB*Fs/(22*delta_f);
f =  [f1]/(Fs/2);
hc = fir1(round(N)-1, f,'low');
plotFft (fftshift(fft(hc,4096)),Fs);

OTA EXPERIMENT
-----------------------
samp_rate    = 44100; % sample rate
sym_tab_qam  = 0.35355339059*[
                -2+2j,-1+2j,1+2j,2+2j,
                -2+1j,-1+1j,1+1j,2+1j,
                -2-1j,-1-1j,1-1j,2-1j,
                -2-2j,-1-2j,1-2j,2-2j];

SF  = sym_tab_qam(6);
SG  = sym_tab_qam(7);
SJ  = sym_tab_qam(10);

SS = 0;
S0 = SJ;
S1 = SG;
PP = SF;

tx_symbols = [...
% % 57 58 59 60 61 62 63 64  1  2  3  4  5  6  7  8
% % -8 -7 -6 -5 -4 -3 -2 -1  0  1  2  3  4  5  6  7
% % XX       XX       XX   |   XX       XX       XX
%   PP SS SS PP SS SS PP SS SS PP SS SS PP SS SS PP; % TEST PILOT ONLY
%   SS SS SS SS SS SS SS SS SS SS SS SS SS SS SS SS; % SILENT
% % FRAME 0 SUBFRAME 0
% %             XX XX XX XX XX XX XX XX
%   S0 S1 S0 S1 PP PP PP PP PP PP PP PP S0 S1 S0 S1; % SYMBOL 0
%   S1 S0 S1 S0 PP PP PP PP PP PP PP PP S1 S0 S1 S0; % SYMBOL 1
%   S1 S0 S1 S0 S1 S1 S0 S1 S0 S0 S1 S0 S1 S0 S1 S0; % SYMBOL 2
%   S1 S1 S0 S1 S1 S0 S1 S0 S1 S0 S1 S0 S0 S1 S0 S1; % SYMBOL 3
%   S1 S0 S1 S1 S0 S1 S0 S1 S0 S0 S1 S1 S0 S1 S0 S1; % SYMBOL 4
%   S1 S0 S1 S0 S0 S1 S0 S0 S1 S0 S1 S1 S0 S1 S0 S1; % SYMBOL 5
%   PP S0 S1 PP S1 S0 PP S1 S0 PP S1 S0 PP S1 S1 PP; % SYMBOL 6
% % XX       XX       XX       XX       XX       XX
% % FRAME 0 SUBFRAME 1
%   S1 S0 S1 S1 S0 S1 S0 S1 S0 S0 S1 S1 S0 S1 S0 S1; % SYMBOL 0
%   S1 S0 S1 S0 S0 S1 S0 S0 S1 S0 S1 S1 S0 S1 S0 S1; % SYMBOL 1
%   S1 S0 S1 S0 S1 S1 S0 S1 S0 S0 S1 S0 S1 S0 S1 S0; % SYMBOL 2
%   S1 S1 S0 S1 S1 S0 S1 S0 S1 S0 S1 S0 S0 S1 S0 S1; % SYMBOL 3
%   S1 S0 S1 S1 S0 S1 S0 S1 S0 S0 S1 S1 S0 S1 S0 S1; % SYMBOL 4
%   S1 S0 S1 S0 S0 S1 S0 S0 S1 S0 S1 S1 S0 S1 S0 S1; % SYMBOL 5
%   PP S0 S1 PP S1 S0 PP S1 S0 PP S1 S0 PP S1 S1 PP; % SYMBOL 6
% % XX       XX       XX       XX       XX       XX
% % FRAME 0 SUBFRAME 1
%   S1 S0 S1 S1 S0 S1 S0 S1 S0 S0 S1 S1 S0 S1 S0 S1; % SYMBOL 0
%   S1 S0 S1 S0 S0 S1 S0 S0 S1 S0 S1 S1 S0 S1 S0 S1; % SYMBOL 1
%   S1 S0 S1 S0 S1 S1 S0 S1 S0 S0 S1 S0 S1 S0 S1 S0; % SYMBOL 2
%   S1 S1 S0 S1 S1 S0 S1 S0 S1 S0 S1 S0 S0 S1 S0 S1; % SYMBOL 3
%   S1 S0 S1 S1 S0 S1 S0 S1 S0 S0 S1 S1 S0 S1 S0 S1; % SYMBOL 4
%   S1 S0 S1 S0 S0 S1 S0 S0 S1 S0 S1 S1 S0 S1 S0 S1; % SYMBOL 5
%   PP S0 S1 PP S1 S0 PP S1 S0 PP S1 S0 PP S1 S1 PP; % SYMBOL 6
% % XX       XX       XX       XX       XX       XX

% -8 -7 -6 -5 -4 -3 -2 -1  0  1  2  3  4  5  6  7
%  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
  PP SS SS PP SS SS PP SS SS PP SS SS PP SS SS PP;
  PP SS SS PP SS SS PP SS SS PP SS SS PP SS SS PP;
  SS SS SS SS SS SS SS SS SS SS SS SS SS SS SS SS;
  SS SS SS SS SS SS SS SS SS SS SS SS SS SS SS SS;
  S1 S0 S1 S1 S0 S1 S0 S1 S0 S0 S1 S1 S0 S1 S0 S1;
  S1 S0 S1 S0 S0 S1 S0 S0 S1 S0 S1 S1 S0 S1 S0 S1;
  S1 S0 S1 S0 S1 S1 S0 S1 S0 S0 S1 S0 S1 S0 S1 S0;
  S1 S1 S0 S1 S1 S0 S1 S0 S1 S0 S1 S0 S0 S1 S0 S1;
  S1 S0 S1 S1 S0 S1 S0 S1 S0 S0 S1 S1 S0 S1 S0 S1;
  S1 S0 S1 S1 S0 S1 S0 S1 S0 S0 S1 S1 S0 S1 S0 S1;
  S1 S0 S1 S0 S0 S1 S0 S0 S1 S0 S1 S1 S0 S1 S0 S1;
  S1 S0 S1 S1 S0 S1 S0 S1 S0 S0 S1 S1 S0 S1 S0 S1;
  S1 S0 S1 S0 S0 S1 S0 S0 S1 S0 S1 S1 S0 S1 S0 S1;
  S1 S0 S1 S0 S1 S1 S0 S1 S0 S0 S1 S0 S1 S0 S1 S0;
  S1 S0 S1 S1 S0 S1 S0 S1 S0 S0 S1 S1 S0 S1 S0 S1;
  S1 S1 S0 S1 S1 S0 S1 S0 S1 S0 S1 S0 S0 S1 S0 S1;
  S1 S0 S1 S1 S0 S1 S0 S1 S0 S0 S1 S1 S0 S1 S0 S1;
  S1 S0 S1 S1 S0 S1 S0 S1 S0 S0 S1 S1 S0 S1 S0 S1;
  S1 S0 S1 S0 S0 S1 S0 S0 S1 S0 S1 S1 S0 S1 S0 S1;
  S1 S0 S1 S0 S1 S1 S0 S1 S0 S0 S1 S0 S1 S0 S1 S0;
  S1 S0 S1 S1 S0 S1 S0 S1 S0 S0 S1 S1 S0 S1 S0 S1;
  S1 S0 S1 S0 S0 S1 S0 S0 S1 S0 S1 S1 S0 S1 S0 S1;
  S1 S0 S1 S1 S0 S1 S0 S1 S0 S0 S1 S1 S0 S1 S0 S1;
];

constellation_colors = repmat([RED;GREEN;BLUE;YELLOW;CYAN;MAGENTA;DBLUE;DCYAN;RED;GREEN;BLUE;YELLOW;CYAN;MAGENTA;DBLUE;DCYAN],23,1);
tx_symbols = reshape(tx_symbols',1,columns(tx_symbols)*rows(tx_symbols));
plotConstellations(tx_symbols, constellation_colors);
tx_baseband = ofdmEncode(tx_symbols,16,128,72);

rx_symbols = ofdmDecode(tx_baseband,16,128,72,23);
plotConstellations(rx_symbols(1:368),constellation_colors);
startsymbol  = 1;
wsize        = 1;
endsymbol    = startsymbol + wsize*16 -1;
plotConstellations(rx_symbols(startsymbol:endsymbol),constellation_colors(startsymbol:endsymbol));
plotComplex(rx_symbols(startsymbol:endsymbol));

fc = 10000;
tmp = length(tx_baseband)-1;
carrier = e.^(j*2*pi*fc*[0:1:tmp]/samp_rate);
upconverted_baseband = real(carrier).*real(tx_baseband).+(j*imag(carrier).*imag(tx_baseband));
upconverted_baseband_real  = real(upconverted_baseband).+ imag(upconverted_baseband);

upconverted_baseband_real = [zeros(1,200) upconverted_baseband_real];

sound(rot90([zeros(1,100) upconverted_baseband_real]), 44100)
wavwrite([upconverted_baseband_real].*(10), 44100,'test_ofdm_bpsk.wav')
[upconverted_baseband_real_rx_o samp_rate nbits]  = wavread('test_ofdm_bpsk.wav');


upconverted_baseband_real_rx = rot90(upconverted_baseband_real_rx_o(1:length(upconverted_baseband_real_rx_o)));
upconverted_baseband_real_rx_ol = length(upconverted_baseband_real_rx_o);

subplot(2,1,1)
plotComplex(upconverted_baseband_real)
subplot(2,1,2)
plotComplex(upconverted_baseband_real_rx(1:length(upconverted_baseband_real)))

% Magic Lowpass
f1 = fc;
f2 = f1+500;
dB  = 40;
N = dB*samp_rate/(22*(f2-f1));
fq =  [f1]/(samp_rate/2);
rx_lowpass_kernel = fir1(round(N)-1, fq,'low');
printf('Filter lenght: %d \n',length(rx_lowpass_kernel))

s = 0;
tmp = length(upconverted_baseband_real_rx)-1;
rx_carrier    = e.^(j*2*pi*(fc)*[s:1:s+tmp]/samp_rate);
rx_baseband_unfiltered = rx_carrier.*upconverted_baseband_real_rx;
rx_baseband = conv(rx_lowpass_kernel,rx_baseband_unfiltered);
filtleno2 = length(rx_lowpass_kernel)/2;
rx_baseband_fp_removed = rx_baseband(length(rx_lowpass_kernel)/2:length(rx_baseband)-length(rx_lowpass_kernel)/2); % Remove filter samples
rx_baseband_synched = rx_baseband_fp_removed(214:length(rx_baseband_fp_removed)); % Magic Symbol Synchronization
rx_baseband_synched_ol = length(rx_baseband_synched);
rx_baseband_synched = interpft(rx_baseband_synched,rx_baseband_synched_ol*2);
rx_baseband_synched = circshift(rx_baseband_synched,-1,2); % Magic Sampling Timing Correction
rx_baseband_synched = interpft(rx_baseband_synched,rx_baseband_synched_ol);

symbol_rx = ofdmDecode(rx_baseband_synched,16,128,72,23);
% plotConstellations(symbol_rx,constellation_colors);
subplot(2,1,1);
plotComplex(tx_symbols(1:16*5));
subplot(2,1,2);
plotComplex(symbol_rx(1:16*5));

symbol_rx_l = length(symbol_rx);
sc_dc       = getSubcarrierSymbols(16,16, symbol_rx(16*4+1:symbol_rx_l));
sc_dc_tx    = getSubcarrierSymbols(16,16,tx_symbols(16*4+1:symbol_rx_l));
sc_dc_l     = length(sc_dc);

figure(1,'position', get(0,'screensize')*0.5);
subplot(2,2,1);
plotComplex(sc_dc_tx);
subplot(2,2,3);
plotComplex(sc_dc);
subplot(1,2,2)
plotConstellations([sc_dc_tx sc_dc], [repmat([BLACK],sc_dc_l,1); repmat([RED],sc_dc_l,1)])


% Magic Lowpass
f1 = 44100/200;
f2 = f1+1100;
dB  = 40;
N = dB*samp_rate/(22*(f2-f1));
fq =  [f1]/(samp_rate/2);
signal_detect_lowpass_kernel = fir1(round(N)-1, fq,'low');
printf('Filter lenght: %d \n',length(signal_detect_lowpass_kernel))

sig=rx_baseband_fp_removed;
% sig=[zeros(1,200) tx_baseband];
signal_detect = conv(autocorr(sig,200),signal_detect_lowpass_kernel)(length(signal_detect_lowpass_kernel)/2:length(sig));
signal_detect = signal_detect;
subplot(4,1,1);
plotComplex(sig);
subplot(4,1,2);
plotComplex(autocorr(sig,200));
subplot(4,1,3);
plotComplex(signal_detect);
subplot(4,1,4);
plotComplex(diff(real(signal_detect)));

SC-FDMA
-----------------------
samp_rate    = 44100; % sample rate
sym_tab_qam  = 0.35355339059*[
                -2+2j,-1+2j,1+2j,2+2j,
                -2+1j,-1+1j,1+1j,2+1j,
                -2-1j,-1-1j,1-1j,2-1j,
                -2-2j,-1-2j,1-2j,2-2j];

SF  = sym_tab_qam(6);
SG  = sym_tab_qam(7);
SJ  = sym_tab_qam(10);

SS = 0;
S0 = SJ;
S1 = SG;
PP = SF;

% baseband symbols
%  A B | C D
%  E F | G H
%  ----+-----
%  I J | K L
%  M N | O P


data_symbols     = [SF SG SJ SK];
DS = fft(data_symbols);

tx_symbols = [...
%  -8  -7  -6  -5  -4  -3  -2    -1     0     1     2    3    4    5    6    7
%   1   2   3   4   5   6   7     8     9    10    11   12   13   14   15   16
   DS(1) DS(2) DS(3) DS(4) DS(1) DS(2) DS(3) DS(4) DS(1) DS(2) DS(3) DS(4) DS(1) DS(2) DS(3) DS(4)
   SF SG SJ SK SF SG SJ SK SF SG SJ SK SF SG SJ SK;
];

tx_symbols = reshape(tx_symbols',1,columns(tx_symbols)*rows(tx_symbols));
tx_baseband = ofdmEncode(tx_symbols,16,128,72);

plotComplex(DS);
plotComplex(tx_baseband);


