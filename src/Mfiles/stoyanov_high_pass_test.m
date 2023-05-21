% stoyanov_high_pass_test.m

% Design a 3rd order Butterworth high-pass filter and implement it as
% the parallel combination of 1st and 2nd order Stoyanov LS1 and LS2a
% all-pass filter sections. These filter sections have low coefficient
% sensitivity for roots near z=1. Unfortunately, the 2nd order section
% is not "structurally passive", ie: not all-pass when the coefficients
% are rounded to fixed point values. In that case the Gray and Markel
% one-multiplier 2nd order section is preferred.
%
% See: "Design and Realization of Efficient IIR Digital Filter Structures
% Based on Sensitivity Minimizations", G. Stoyanov, Zl. Nikolova, K. Ivanova,
% V. Anzova", Proceedings TELSIKS 2007, pp. 299-308, Sept. 2007.

clear all;

% Load the Octave signal package (for the butter() function)
pkg load signal

tol=1e-10;

% Filter specification
fs=48000; % sample rate
fp=200;   % high pass cutoff frequency
[n,d]=butter(3,2*fp/fs,"high");
[H,w]=freqz(n,d,2^16);

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
H3=freqz(n3,d3,w);
if max(abs(H-H3)) > tol
  error("max(abs(H-H3)) > tol");
endif
% Check parallel all-pass transfer function
N3=0.5*(conv(fliplr(d2),d1)-conv(fliplr(d1),d2));
D3=conv(d1,d2);
H3c=freqz(N3,D3,w);
if max(abs(H-H3c)) > tol
  error("max(abs(H-H3c)) > tol");
endif

% 3rd order Butterworth implemented as parallel combination of Stoyanov all-pass
% State variable representation of 1st order section
C=1-r1;
A1=1-C;
B1=C;
C1=2-C;
D1=C-1;
% State variable representation of 2nd order section 
b1=-2*abs(r2)*cos(arg(r2));
b2=abs(r2)^2;
c2=1-b2;
c1=(b1+2-c2)/2;
A2=[1-c1, -c1; -c2-c1+2, -c2-c1+1];
B2=[c1;c2+c1-2];
C2=[c2,c2];
D2=1-c2;
% Convert state variable representation to transfer function form
Hap1=Abcd2H(w,A1,B1,C1,D1);
Hap2=Abcd2H(w,A2,B2,C2,D2);
if max(abs(((Hap2-Hap1)/2)-H)) > tol
  error("max(abs(Hpap3-H)) > tol");
endif

% 3rd order Butterworth implemented as parallel combination of Stoyanov all-pass
% with coefficients rounded to 10 bits
scale=2^15;
Cs=round(C*scale)/scale;
A1s=1-Cs;
B1s=Cs;
C1s=2-Cs;
D1s=Cs-1;
% State variable representation of 2nd order section 
c1s=round(c1*scale)/scale;
c2s=round(c2*scale)/scale;
A2s=[1-c1s, -c1s; -c2s-c1s+2, -c2s-c1s+1];
B2s=[c1s;c2s+c1s-2];
C2s=[c2s,c2s];
D2s=1-c2s;
% Check the filters are structurally loss-less
Hap1s=Abcd2H(w,A1s,B1s,C1s,D1s);
if max(abs(abs(Hap1s)-1)) > tol/1e4
  error("max(abs(abs(Hap1s)-1)) > tol/1e4");
endif
Hap2s=Abcd2H(w,A2s,B2s,C2s,D2s);
if max(abs(abs(Hap2s)-1)) > tol/1e4
  error("max(abs(abs(Hap2s)-1)) > tol/1e4");
endif
