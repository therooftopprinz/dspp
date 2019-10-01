sample_rate = 44100;

figure(1,'position',get(0,'screensize').*0.8);

testtone = tone(sample_rate, 5000, 200/sample_rate);
% testtone = real(testtone);
carrier  = tone(sample_rate, 10000, 200/sample_rate);
% carrier  = real(carrier);

testmsg = testtone.*carrier;

plotFftRaw(fftshift(fft(testmsg)), sample_rate);