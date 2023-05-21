function [B,A]=sv2tf(a11,a12,a21,a22,b1,b2,c1,c2,d)

% [B,A]=sv2tf(a11,a12,a21,a22,b1,b2,c1,c2,d)
% Converts a cascade of second order sections to a numerator/denominator 
% transfer function H(z)= B(z)/A(z)

if nargin ~= 9
    error('Expected nine input arguments');
end

% Convert {a b c} to {p q} form
q1 = (c1.*b1) + (c2.*b2);
q2 = ((c1.*b2).*a12) + ((c2.*b1).*a21);
q2 = q2 - ((c1.*b1).*a22) - ((c2.*b2).*a11);
p1 = -(a11 + a22);
p2 = (a11.*a22) - (a12.*a21);

% Find B and A
B = 1;
A = 1;
for i=1:length(d)
  A = conv(A,[1, p1(i), p2(i)]);
  B = conv(B,[d(i), d(i)*p1(i)+q1(i), d(i)*p2(i)+q2(i)]);
end








