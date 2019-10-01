samp_rate    = 44100; % sample rate
sym_tab_qam  = 0.35355339059*[...
                -3+3j,-1+3j,1+3j,3+3j;
                -3+1j,-1+1j,1+1j,3+1j;
                -3-1j,-1-1j,1-1j,3-1j;
                -3-3j,-1-3j,1-3j,3-3j;];

% baseband symbols
%  A B | C D
%  E F | G H
%  ----+-----
%  I J | K L
%  M N | O P

F_  = sym_tab_qam(2,2);
G_  = sym_tab_qam(2,3);
J_  = sym_tab_qam(3,2);
K_  = sym_tab_qam(3,3);

O_ = 0;

% qam16 = [];
% for ii=1:1:4
%    for jj=1:1:4
%       x = 1+(jj-1)*2;
%       y = 1+(ii-1)*2;
%       qam16 = [qam16 x+y*1j];
% endfor

clear -g SYMS;
global SYMS = [F_ K_ G_ J_]; % qpsk
% global SYMS = qam16;
SYMS
Z_ = -1;
X_ = 1;

function sym = RS  
    global SYMS;
    sym = SYMS(floor(unifrnd(1,columns(SYMS)+1)));
endfunction

S = 2;

tx_symbols = [...
% -8   -7   -6   -5   -4   -3   -2   -1    0    1    2    3    4    5    6    7
%  1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16
% DATA FRAME
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
% SYNC FRAME
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   O_   O_   O_   O_   O_   O_   O_   O_   RS   RS   RS   RS;
   RS   RS   RS   RS   O_   O_   O_   O_   O_   O_   O_   O_   RS   RS   RS   RS;
   RS   RS   RS   RS   O_   X_   O_   Z_   O_   X_   O_   Z_   RS   RS   RS   RS;
   RS   RS   RS   RS   O_   X_   O_   Z_   O_   X_   O_   Z_   RS   RS   RS   RS;
   RS   RS   RS   RS   O_   X_   O_   Z_   O_   X_   O_   Z_   RS   RS   RS   RS;
   RS   RS   RS   RS   O_   X_   O_   Z_   O_   X_   O_   Z_   RS   RS   RS   RS;
   RS   RS   RS   RS   O_   O_   O_   O_   O_   O_   O_   O_   RS   RS   RS   RS;
   RS   RS   RS   RS   O_   O_   O_   O_   O_   O_   O_   O_   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
% DATA FRAME
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS   RS;
];
% -8   -7   -6   -5   -4   -3   -2   -1    0    1    2    3    4    5    6    7
%  1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16

N_FFT_FRAME = rows(tx_symbols);
N_SC   = 16;
N_SC_U = 16;
N_SYNC_SYM = 4;
CP_LEN  = 4;
SYM_LEN = N_SC+CP_LEN;
ACOR_SIZE = SYM_LEN*N_SYNC_SYM/2;

tx_symbols = reshape(conj(tx_symbols'),1,N_SC*N_FFT_FRAME);
tx_baseband = ofdmEncode(tx_symbols,N_SC_U,N_SC,CP_LEN);

txPlot = figure(1,'position', get(0,'screensize'));

tx_baseband_max = max(tx_baseband.*conj(tx_baseband));
tx_baseband_mean = mean(tx_baseband.*conj(tx_baseband));
printf("tx_baseband max=%d\n", tx_baseband_max);
printf("tx_baseband_mean mean=%d\n", tx_baseband_mean);
printf("tx_baseband_mean papr=%d\n", log(tx_baseband_max/tx_baseband_mean));

% Channel Condition
channel_response = [1 -0.3 0.2 0 0 0 0.2 -0.3]; % expected sync at 478-482 (CP=4)
tx_baseband = conv(channel_response, tx_baseband);

tx_baseband_acorr = autocorr(tx_baseband,40);

subplot(2,2,1);
plotComplex(tx_baseband);
grid()
title('TX Baseband');
xlabel('Sample');
ylabel('Amplitude');
subplot(2,2,3);
plotComplex(tx_baseband_acorr);
grid()
title('TX Baseband Autocorrelation');
xlabel('Sample');
ylabel('Amplitude');

subplot(1,2,2);
plotConstellationsNC(tx_symbols);
grid()
title('TX Symbols');
xlabel('I');   
ylabel('Q');
axis([-2 2 -2 2]);

%print (txPlot, "out/autocorr_exp_tx_plots.png", "-dpng","-S1920,1080")
%print (txPlot, "out/autocorr_exp_tx_plots.png", "-dpng","-S960,540")
%print (txPlot, "out/autocorr_exp_tx_plots.png", "-dpng","-S480,270")
% close(txPlot)

% Signal Detection
[max_acorr_value max_acorr_value_index] = max(tx_baseband_acorr);
printf("max_acorr_value=%d\n", max_acorr_value);
printf("max_acorr_value_index=%d\n", max_acorr_value_index);

N_TO_DECODE = 20;
max_acorr_value_index = max_acorr_value_index + 1; % magic adjust

function rx_symbols = decodeAt(tx_symbols, tx_baseband, max_acorr_value_index,N_SC_U,N_SC,CP_LEN,N_TO_DECODE,SYM_LEN)
   rx_baseband_synched = tx_baseband(max_acorr_value_index:max_acorr_value_index+N_TO_DECODE*SYM_LEN);
   rx_symbols = ofdmDecode(rx_baseband_synched,N_SC_U,N_SC,CP_LEN,N_TO_DECODE);

   rxPlot = figure(2,'position', get(0,'screensize'));
   for ii=1:1:2
      for jj=1:1:8
         % continue;
         index = (ii-1)*8+jj;
         subplot(4,8,index);
         plotConstellationsNC(rx_symbols(index:N_SC:end));
         grid();
         title(sprintf('RX SC [%d]', index));
         xlabel('I');   
         ylabel('Q');
         axis([-2 2 -2 2]);
      endfor
   endfor
   for ii=1:1:2
      for jj=1:1:8
         % continue;
         index = (ii-1)*8+jj;
         subplot(4,8,16+index);
         plotConstellationsNC(tx_symbols(index:N_SC:end));
         grid();
         title(sprintf('TX SC [%d]', index));
         xlabel('I');   
         ylabel('Q');
         axis([-2 2 -2 2]);
      endfor
   endfor
endfunction

decodeAt(tx_symbols, tx_baseband, max_acorr_value_index,N_SC_U,N_SC,CP_LEN,N_TO_DECODE,SYM_LEN);

% print (rxPlot, "out/autocorr_exp_rx_plots.png", "-dpng","-S1920,1080");
% close(rxPlot)