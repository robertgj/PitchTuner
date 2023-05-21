% gray_and_markel_high_pass_test.m

% Design a 3rd order Butterworth high-pass filter and implement it as
% the parallel combination of 1st and 2nd order Gray and Markel GM1 and
% GM2 all-pass filter sections. 
%
% See: 
% "Digital Lattice And Ladder Filter Synthesis", A. H. Gray and J. D. Markel,
% IEEE Transactions on Audio and Electroacoustics, Dec. 1973, Vol. 21, No. 6,
% pp. 491-500

clear all;

% Load the Octave signal package (for the butter() function)
pkg load signal

tol=1e-10;
nf=2^16;

% Filter specification
fs=48000; % sample rate
fp=200;   % high pass cutoff frequency
f=(0:(nf-1))'/(2*nf);
w=2*pi*f;

% Expected result
[n,d]=butter(3,2*fp/fs,"high");
H=freqz(n,d,w);

% Butterworth poles
wc=tan(pi*fp/fs); % Warp frequency axis
theta=2*pi/3;     % Poles equally spaced in angle

% 3rd order Butterworth transfer function
% 1st order section
n1=[1 -1];
d1=[(wc+1) (wc-1)];
r1=(1-wc)/(1+wc);
% 2nd order section
a0=1-(2*wc*cos(theta))+(wc*wc);
a1=2*((wc*wc)-1);
a2=1+(2*wc*cos(theta))+(wc*wc);
n2=conv(n1,n1);
d2=[a0 a1 a2];
% 3rd order transfer function
n3=conv(n1,n2);
d3=conv(d1,d2);
% Check pole locations
r2=(-a1+j*(4*wc*sin(theta)))/(2*a0);
if abs(roots([a0 a1 a2])(1)-r2) > 10*eps
  error("abs(roots([a0 a1 a2])(1)-r2) > 10*eps");
endif
% Compare with butter
[H3,w]=freqz(n3,d3,w);
if max(abs(H3-H)) > tol
  error("max(abs(H3-H)) > tol");
endif
% Check parallel all-pass combination
Nap3=0.5*(conv(fliplr(d2),d1)-conv(fliplr(d1),d2));
Dap3=conv(d1,d2);
[Hap3,w]=freqz(Nap3,Dap3,w);
if max(abs(Hap3-H)) > tol
  error("max(abs(Hap3-H3)) > tol");
endif

% 3rd order Butterworth implemented as parallel combination of Gray and Markel
% all-pass sections.
% State variable representation of 1st order section
k=-r1;
e=-1;
A1=-k;
B1=1+(e*k);
C1=1-(e*k);
D1=k;
% State variable representation of 2nd order section 
b1=-2*abs(r2)*cos(arg(r2));
b2=abs(r2)^2;
k2=b2;
e1=-1;
k1=b1/(1+b2);
e2=-1;
A2=[-k1, (e1*k1)+1; ((e1*k1)-1)*k2, -k2*k1];
B2=[0; (e2*k2)+1];
C2=[(((e1*k1)-1)*(e2*k2))-(e1*k1)+1 , k1*(1-(e2*k2))];
D2=[k2];
% Check
Hap1=Abcd2H(w,A1,B1,C1,D1);
Hap2=Abcd2H(w,A2,B2,C2,D2);
if max(abs(((Hap2-Hap1)/2)-H)) > tol
  error("max(abs(((Hap2-Hap1)/2)-H)) > tol");
endif

% 3rd order Butterworth implemented as parallel combination of Gray amd Markel
% all-pass sections with coefficients rounded to 16 bits
scale=2^15;
ks=round(k*scale)/scale;
A1s=-ks;
B1s=1+(e*ks);
C1s=1-(e*ks);
D1s=ks;
k1s=round(k1*scale)/scale;
k2s=round(k2*scale)/scale;
A2s=[-k1s, (e1*k1s)+1; ((e1*k1s)-1)*k2s, -k2s*k1s];
B2s=[0; (e2*k2s)+1];
C2s=[(((e1*k1s)-1)*(e2*k2s))-(e1*k1s)+1 , k1s*(1-(e2*k2s))];
D2s=[k2s];
% Check the filters are structurally loss-less
Hap1s=Abcd2H(w,A1s,B1s,C1s,D1s);
if max(abs(abs(Hap1s)-1)) > tol/1e4
  error("max(abs(abs(Hap1s)-1)) > tol/1e4");
endif
Hap2s=Abcd2H(w,A2s,B2s,C2s,D2s);
if max(abs(abs(Hap2s)-1)) > tol/1e4
  error("max(abs(abs(Hap2s)-1)) > tol/1e4");
endif
