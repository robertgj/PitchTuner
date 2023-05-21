% heedf0.m

clear all

fs=10000;
flp=1000;
fhp=200;
subSample=1;
msWindow=25;
msTmax=10;
msTsample=1;
threshold=0.1;

# Extract heed.wav
status=system("uudecode wav/heed.wav.bz2.b64 && bunzip2 -f heed.wav.bz2");
if status
  error("Extract heed.wav failed");
endif

% Run PitchTracker_test
cmd = sprintf( ...
"bin/PitchTracker_test --debug --removeDC --sampleRate %d --subSample %d \
--msWindow %d --inputLpFilterCutoff %d --baseLineHpFilterCutoff %d \
--msTsample %d --msTmax %d --threshold %f --file heed.wav > heedf0.txt",
fs, subSample, msWindow, flp, fhp, msTsample, msTmax, threshold);
printf("Running %s\n",cmd);
status=system(cmd);
if status
  error("PitchTracker_test failed");
endif

% Show waveform
[heed,wfs]=audioread("heed.wav");
nheed=length(heed);
plot((0:(nheed-1))*1000/wfs,heed)
xlabel("Time(ms)");
print("heed","-dpng");
close();

% Show CDT
cdt=load("cdt.txt");
msLags=(0:(columns(cdt)-1))*(1000/fs);
plot(msLags,[cdt(round(150/msTsample),:);threshold*ones(1,columns(cdt))])
xlabel("Lags(ms)");
print("heed_cdt_150ms","-dpng");
close
plot(msLags,[cdt(round(250/msTsample),:);threshold*ones(1,columns(cdt))])
xlabel("Lags(ms)");
print("heed_cdt_250ms","-dpng");
close();

% Show pitch
f0=load("heedf0.txt");
t=msTsample*(0:(rows(cdt)-1));
plot(t,f0(1:rows(cdt)))
xlabel("Time(ms)");
ylabel("Pitch estimate(Hz)");
print("heedf0","-dpng");
close();

