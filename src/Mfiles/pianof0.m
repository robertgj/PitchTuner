% pianof0.m

clear all

fs=48000;
subSample=2;
msTsample=1;
flp=1000;
fhp=200;
threshold=0.1;
Af0=440;
expectedf0=Af0*(2.^([-9,-7,-5,-4,-2,0,2,3]/12));
Cmajor_scale=["C4";"D4";"E4";"F4";"G4";"A4";"B4";"C5"];

% Write out pianof0.abc
fid=fopen("piano.abc","wt");
fprintf(fid,"X:1 \n\
T:Notes on piano \n\
M:4/4 \n\
L:1/4 \n\
Q:1/4=120 \n\
K:C \n\
C D E F G A B c\n");
fclose(fid);

% Run PitchTracker_test
status=system("abc2midi piano.abc -o piano.mid");
if status
  error("abc2midi failed");
endif
status=system(sprintf("timidity -OwMs1l -s%d piano.mid -o piano.wav", fs));
if status
  error("timidity failed");
endif
cmd=sprintf("bin/PitchTracker_test --debug --removeDC --sampleRate %d \
--subSample %d --inputLpFilterCutoff %d --baseLineHpFilterCutoff %d \
--threshold %f --msTsample %d --file piano.wav > pianof0.txt",
            fs, subSample, flp, fhp, threshold, msTsample);
printf("Running %s\n",cmd);
status=system(cmd);
if status
  error("PitchTracker_test failed");
endif

% Show waveform
[piano,wfs]=audioread("piano.wav");
npiano=200000;
plot((0:(npiano-1))/wfs,piano(1:npiano))
xlabel("Time(s)");
print("piano","-dpng");
close();

% Show results
pianof0=load("pianof0.txt");
plot((0:3999)*msTsample,pianof0(1:4000))
xlabel("Time(ms)");
ylabel("Pitch estimate(Hz)");
print("pianof0","-dpng");
close();

% Show minCDT
minCDT=load("minCDT.txt");
minCDT=minCDT(:);
plot((0:(length(minCDT)-1))*msTsample,[minCDT,threshold*ones(size(minCDT))])
xlabel("Time(ms)");
ylabel("Minimum of CDT");
print("pianof0minCDT","-dpng");
close();

% Average pitch estimates
start_sample = 100+(0:500:3500);
end_sample = 350+start_sample;
for k=1:length(start_sample)
  meanf0(k)=mean(pianof0(start_sample(k):end_sample(k)));
endfor
percent_errorf0=100*(meanf0-expectedf0)./expectedf0;
cents_errorf0=1200*log10(meanf0./expectedf0)/log10(2);

% Write latex table
fid=fopen("pianof0_table.tex","wt");
fprintf(fid,"\\begin{table}[H]\n\
\\centering\n\
\\begin{tabular}{|c|r|r|r|r|}\n\
\\hline\n\
Note&Nominal F0&Mean F0 estimate&Error(\\%%)&Error(cents)\\tabularnewline\n\
\\hline\n\
\\hline\n");
for k=1:length(expectedf0)
  fprintf(fid,"%s & %7.2f & %7.2f & %4.2f & %4.2f\\tabularnewline\n\\hline\n",
          Cmajor_scale(k,:),expectedf0(k),meanf0(k),
          percent_errorf0(k),cents_errorf0(k));
endfor
fprintf(fid,"\\end{tabular}\n\
\\caption{\\label{tab:F0-piano}Pitch estimates for a piano playing the\n\
  equal-tempered C Major scale}\n\
\\end{table}\n");
fclose(fid);

% Write markdown table
fid=fopen("pianof0_table.md","wt");
fprintf(fid,"Note | Nominal F0 | Mean F0 estimate | Error(%%) | Error(cents)\n");
fprintf(fid,"---- | ---------: | ---------------: | --------: | -----------:\n");
for k=1:length(expectedf0)
  fprintf(fid,"%s | %7.2f | %7.2f |  %4.2f | %4.2f\n",
          Cmajor_scale(k,:),expectedf0(k),meanf0(k),
          percent_errorf0(k),cents_errorf0(k));
endfor
fclose(fid);

