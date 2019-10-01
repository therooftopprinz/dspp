cazac = zadoffChuSeq(25,139);
tsig = [zeros(1,140) cazac cazac zeros(1,140)];

tsigPlot = figure(1,'position', get(0,'screensize'));
subplot(1,3,1);
plotComplex(tsig);
subplot(1,3,2);
plotComplex(autocorr(tsig,139));
subplot(1,3,3);
plotComplex(xcorr(tsig, cazac));