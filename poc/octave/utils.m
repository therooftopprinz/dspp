pkg load signal
pkg load ltfat
function mixed = complexMixer(a,b)
    mixed = real(a).*real(b).+(j*imag(a).*imag(b));
endfunction

function plotComplex(data)
    plot(real(data),'-b',imag(data),'-r')
endfunction

function fang = normalizeAngle(angles)
    fang = mod(angles.+(4*pi),2*pi);
    for i = 1:length(fang)
        if fang(i) > pi;
            fang(i) = fang(i) - 2*pi;
        endif
        if fang(i) < -pi
            fang(i) = fang(i) + 2*pi;
        endif
    endfor
endfunction

function data_baseband = ofdmEncode(data_symbols,sc_in_use,fft_size,cyclic_prefix_size)
    assert(fft_size>=sc_in_use,'Error in fft size!');
    assert(mod(length(data_symbols),sc_in_use)==0,'Invalid number of sc_in_use!')
    assert(mod(sc_in_use,2)==0,'sc_in_use only even!')
    assert(mod(fft_size,2)==0,'fft_size only even!')
    unused_sc = fft_size-sc_in_use;
    data_baseband = [];
    printf('ofdmEncode: %d \n',length(data_symbols))
    for i=1:sc_in_use:length(data_symbols)
        ofdm_f_frame = circshift([data_symbols(i:i+sc_in_use-1) zeros(1,unused_sc)],-floor(sc_in_use/2),2);
        ofdm_t_frame = ifft(ofdm_f_frame);
        % printf('ofdm frame: %d\n',length(data_baseband)+1);
        cyclic=[];
        if (cyclic_prefix_size>0)
            cyclic = ofdm_t_frame(fft_size-cyclic_prefix_size+1:fft_size);
        endif
        pre = [cyclic ofdm_t_frame];
        pre = pre.*rot90(tukeywin(length(pre),0.1)); % Tukeywin Windowing 5% Front And Back Rolloff
        data_baseband = [data_baseband pre];
    endfor
endfunction


function data_symbols = ofdmDecode(data_baseband,sc_in_use,fft_size,cyclic_prefix_size,frames_to_decode=0)
    assert(fft_size>=sc_in_use,'Error in fft size!');
    assert(mod(sc_in_use,2)==0,'sc_in_use only even!')
    assert(mod(fft_size,2)==0,'fft_size only even!')
    back_time_offset = 0;
    if(cyclic_prefix_size>0)
        back_time_offset = ceil(0.05*(fft_size+cyclic_prefix_size)) % Use Cyclic Prefix Samples For 5% Back Rolloff
    endif
    odfm_t_frame_size = cyclic_prefix_size + fft_size;
    n_s_t_frames = (frames_to_decode)*(fft_size+cyclic_prefix_size);
    if (n_s_t_frames==0)
        n_s_t_frames = length(data_baseband);
    endif
    data_symbols = [];
    printf('ofdmDecode: %d \n',length(data_baseband))
    for i=1:odfm_t_frame_size:n_s_t_frames
        front = i+cyclic_prefix_size-back_time_offset;
        center = i+cyclic_prefix_size;
        back = i+cyclic_prefix_size+fft_size-1-back_time_offset;
        t_f = circshift(data_baseband(front:back),-back_time_offset,2);
        % printf('ofdm frame: %d N(%d) (%d:%d) (%d:%d) [%d:%d]\n',i,length(t_f),center,back,front,center-1,front,back);
        ofdm_f_frame = fft(t_f);
        ofdm_f_frame = circshift(ofdm_f_frame,floor(sc_in_use/2),2)(1:sc_in_use);
        data_symbols = [data_symbols ofdm_f_frame];
    endfor
endfunction

function [axis line1 line2] = plotFft(data, fs=1)
    nn = floor(length(data)/2);
    np = nn;
    if (rem(length(data),2)==0)
        np = np-1;
    endif
    x = -nn:1:np;
    x = fs.*x./length(data);
    fang = unwrap(angle(data));
    [axis line1 line2] = plotyy(x,log(abs(data)),x,fang);
endfunction

function [axis line1 line2] = plotFftRaw(data, fs=1)
    nn = floor(length(data)/2);
    np = nn;
    if (rem(length(data),2)==0)
        np = np-1;
    endif
    x = -nn:1:np;
    x = fs.*x./length(data);
    [axis line1 line2] = plotyy(x,real(data),x,imag(data));
endfunction


RED     = [1 0 0];
GREEN   = [0 1 0];
BLUE    = [0 0 1];
YELLOW  = [1 1 0];
CYAN    = [0 1 1];
MAGENTA = [1 0 1];
DRED    = [0.5 0 0];
DGREEN  = [0 0.5 0];
DBLUE   = [0 0 0.5];
DYELLOW = [0.5 0.5 0];
DCYAN   = [0 0.5 0.5];
DMAGENTA= [0.5 0 0.5];
BLACK   = [0 0 0];
function plotConstellations(symbols,colors)
    scatter(real(symbols),imag(symbols),50.*ones(1,length(symbols)),colors)
endfunction

function plotConstellationsNC(symbols)
    scatter(real(symbols),imag(symbols),50.*ones(1,length(symbols)))
endfunction

function rv=winCyle(cyclic_prefix_size,signal)
    signal_size = length(signal);
    cyclic=[];
    if (cyclic_prefix_size>0)
        cyclic = signal(signal_size-cyclic_prefix_size+1:signal_size);
    endif
    rv = [cyclic signal];
    rv = rv.*rot90(tukeywin(length(rv),0.1));
endfunction

function ro = rotateComplex(complex_num,rot_angle)
    r = abs(complex_num);
    o = angle(complex_num);
    o += rot_angle;
    ro = complex(r*cos(o),r*sin(o));
endfunction

function scs = getSubcarrierSymbols(scIdx, numOfSc, ofdmSymbols)
  scs = [];
  idx = [scIdx:numOfSc:length(ofdmSymbols)];
  for i = idx
    scs = [scs ofdmSymbols(i)];
  endfor
endfunction

function acor = autocorr(signal, corsize)
  acor = [];
  tail = [];
  for i=corsize:corsize:length(signal)-corsize
    s1 = signal(i-corsize+1:i);
    s2 = signal(i+1:i+corsize);
    corr = xcorr(s1, s2);
    head = corr(1:corsize);
    acor = [acor head.+[tail zeros(1, length(head)-length(tail))]];
    tail = corr(corsize+1:end);
  endfor
  acor = [acor zeros(1, corsize)];
endfunction

function samples = tone(sample_rate, frequency, time)
    num_samples = sample_rate*time;
    samples = e.^(j*2*pi*(frequency/sample_rate)*[0:1:num_samples-1]);
endfunction

function samples = zadoffChuSeq(u,N)
    n = 0:1:N;
    samples = e.^((-j*pi*u.*n).*(n+mod(N,2))/N);
endfunction