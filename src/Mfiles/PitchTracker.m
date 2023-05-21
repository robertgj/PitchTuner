function [yr,r,d,cmd,dmin]=PitchTracker(x,fs,dn)
  # Pitch estimator.
  # Inputs;
  #   x - signal
  #   fs - signal sample rate
  #   dn - down-sampling ratio after prefiltering (try dn=1)
  # Outputs:
  #   yr - low-pass filtered and down-sampled input signal
  #   r - autocorrelation
  #   d - difference function
  #   cmd - cumulative mean difference
  #   dmin - minimum of cmd (pitch estimate)
  #
  # Possible test signals with pitch 200Hz and sample rate 40000Hz are:
  #   ramp200=kron(ones(1,20),(-0.995:0.01:0.995));
  #   [yr,r,d,cmd,dmin]=PitchTracker(sin200,40000,4);
  #   sin200=sin(2*pi*(0:3999)*200/40000);
  #   [yr,r,d,cmd,dmin]=PitchTracker(sin200,40000,4);
  #
  # See "YIN, a fundamental frequency estimator for speech and music", 
  # deCheveine,A. amd Kawahara, H., 
  # J. Acoust. Soc. Am. 111(4). April 2002, pp.1917-1930

if (nargin ~= 3) || (nargout ~= 5)
  print_usage ("[yr,r,d,cmd,dmin]=PitchTracker(x,fs,dn)");
endif

# constants
# fs is sample rate
# dn is sub-sampling after prefilter
thresh=0.1;
M=31;           
N=(2*M)+1;             # prefilter length
fc=1000;               # low-pass prefilter cutoff
W=0.025;               # processing window size (sec)
fsr=fs/dn;             # sub-sampled sample rate
WS=fix(W*fsr);         # processing window size (samples)
maxT=0.020;            # maximum period (seconds)
maxTS=fix(maxT*fsr);   # maximum period (samples)
minT=1/fc;             # minimum period (seconds)
minTS=fix(minT*fsr);   # minimum period (samples)

# Low pass filter to fc with hamming window
h=2*(fc/fs)*sinc(2*(fc/fs)*(-M:M)).*hamming(N)';
h=h/sum(h);
y=filter(h,1,x);

# Resample
yr=y(1:dn:length(y));

# Step 1: Autocorrelation
# For convenience place the zero lags in a separate vector so
# that the lag index into r(time,lag) corresponds to the actual lag
rS=length(yr)-maxTS-WS;
r=zeros(rS, maxTS);
r0=zeros(rS,1);
s=0:(WS-1);
for ds=1:rS
  r0(ds)=sum(yr(s+ds).^2);
  for T=1:maxTS
    r(ds,T)=sum(yr(s+ds).*yr(s+ds+T));
  endfor
endfor

# Step 2: Difference function
# Ignore difference for tau=0
d=zeros(rS-maxTS, maxTS);
for ds=1:rS-maxTS
  for t=1:maxTS
    d(ds, t)=r0(ds)+r0(ds+t)-2*r(ds, t);
  endfor
endfor

# Step 3: Cumulative mean normalised difference
cmd=zeros(size(d));
for ds=1:rS-maxTS
  cs=cumsum(d(ds, :))./(1:maxTS);
  cmd(ds, :)=d(ds, :)./cs;
endfor

# Step 4: Absolute threshold
# Step 5: Parabolic interpolation
dmin=zeros(1,rS-maxTS);
for ds=1:rS-maxTS
  for t=2:maxTS-1
    if cmd(ds,t-1) >= cmd(ds,t) && cmd(ds,t) <= cmd(ds,t+1) && cmd(ds,t)<thresh
      c0=cmd(ds,t-1);
      c1=cmd(ds,t)-c0;
      c2=((cmd(ds,t+1)-c0)/2)-c1;
      a=c2;
      b=c1-(c2*((2*t)-1));
      dmin(ds)=-0.5*b/a;
      dmin(ds)=fsr/dmin(ds);
      break;
    endif
  endfor
endfor

# Step 6: Best local estimate
# Not sure what is happening here!?!

endfunction
