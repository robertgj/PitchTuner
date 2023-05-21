function [a11,a12,a21,a22,b1,b2,c1,c2,d] = svbutt(n,fc,fs,pass,opt)

% [a11,a12,a21,a22,b1,b2,c1,c2,d] = svbutt(n,fc,fs,pass,opt)
% svbutt finds the state variable equations for an order n (where n
% is even) Butterworth digital filter with cutoff fc and sample 
% rate fs. Currently, pass may be 'hp' for high pass response or 
% 'lp' for low pass response. The filter is realised as a cascade 
% of second order sections. For opt='direct', the filter is direct 
% form. For opt='bomar3' the filter sections are Bomar type III.
% For opt='minimum' the filter sections are optimum.

if nargin < 3 
   error('Sorry, expected at least three input arguments');
end 
if nargin > 5 
   error('Sorry, expected at most five input arguments');
end 
if rem(n,2) ~= 0 | n<2 
    error('Sorry, expected even order filter');
end
if fc >= (fs/2)
    error('Sorry, expected fc < fs/2');
end
if any(pass ~= 'lp') & any(pass ~= 'hp') & any(pass ~= 'ex')
    error('Sorry, expected lp or hp');
end
opt=opt(1:3);
if any(opt ~= 'dir') & any(opt ~= 'bom') & any(opt ~= 'min')
    error('Sorry, expected type direct, bomar3, or minimum');
end

% Warp the frequency scale
wc = tan(pi*fc/fs);

% Find the transfer function as a set of vectors d, p1, p2, q1, q2
% each of length n/2. Each index into these vectors
% corresponds to a second order section with transfer function:
%    H(z)= d   +       (q1/z) + (q2/(z*z))
%                  _______________________
%                  1 + (p1/z) + (p2/(z*z))
% Warp the frequency scale
wc = tan(pi*fc/fs);

% Get the Butterworth pole positions in the upper left s plane quadrant
lambda = exp(sqrt(-1)*pi*(((1:2:n-1)/(2*n))+0.5))';

% Calculate some intermediate values
k1 = wc*wc*ones(size(lambda));
k2 = 2*wc*real(lambda);
if all(pass == 'lp')
    d = k1./(1+k1-k2);
    p1 = 2*(k1-1)./(1+k1-k2);
    p2 = (1+k1+k2)./(1+k1-k2);
    q1 = (2-p1).*d;
    q2 = (1-p2).*d;
end
if all(pass == 'hp')
    d = ones(size(lambda))./(1+k1-k2);
    p1 = 2*(k1-1).*d;
    p2 = (1+k1+k2).*d;
    q1 = (-2-p1).*d;
    q2 = (1-p2).*d;
end
if all(pass == 'ex')
    n=6;
%    d=[0.00098 0.000945 0.0009325];
%    p1=[-1.9641 -1.9112 -1.8819];
%    p2=[0.96802 0.91498 0.88563];
    d=[0.0009325 0.000945 0.00098];
    p1=[-1.8819 -1.9112 -1.9641];
    p2=[0.88563 0.91498 0.96802];
    q1=d.*(2-p1);
    q2=d.*(1-p2);
end

% Find the state variable equations
if all(opt == 'dir')
    % Find the corresponding second order section 
    % direct form equations:
    %   x(k+1) = A*x(k) + B*u(k)
    %   y(k)   = C*x(k) + D*u(k)
    a11 = zeros(size(p1));
    a12 = ones(size(p1));
    a21 = -p2;
    a22 = -p1;
    b1  = zeros(size(p1));
    b2  = ones(size(p1));
    c1  = q2;
    c2  = q1;
end
if all(opt == 'bom')
    % Find the corresponding second order section 
    % Bomar Type III state variable equations:
    a11 = -p1/2;
    a12 = sqrt(1+((p1.*p1/4).*(p2-3)./(1+p2)));
    a21 = ((p1.*p1/4)-p2)./a12;
    a22 = a11;
    b1 = zeros(size(p1));
    b2 = sqrt((1-p2).*((1+p2).*(1+p2)-(p1.*p1)));
    b2 = b2./sqrt((((1+p2).*(1+(p1.*p1/4)))-(p1.*p1)));
    c1 = (q2+(a11.*q1))./(a12.*b2);
    c2 = q1./b2;
end
if all(opt == 'min')
    % Find the corresponding optimal second order section
    % state variable equations:
    % First find some intermediate results
    v1 = q2./q1;
    v2 = sqrt((v1.*v1)-(p1.*v1)+p2);
    v3 = v1-v2;
    v4 = v1+v2;
    v5 = p2-1;
     v6 = p2+1;
    v7 = v5.*((v6.*v6)-(p1.*p1));
    v8 = (p1.*p1/4)-p2;
    % Now do the design equations
    a11 = -p1/2;
    a22 = a11;
    b1 = sqrt(v7./((2*p1.*v3) - (v6.*(1+(v3.*v3)))));
    b2 = sqrt(v7./((2*p1.*v4) - (v6.*(1+(v4.*v4)))));
    a21 = sqrt((((b2.*b2)+v5).*v8)./((b1.*b1)+v5));
    a12 = v8./a21;
    c1 = q1./(2*b1);
    c2 = q1./(2*b2);
end
