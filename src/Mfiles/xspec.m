function Txy = xspec(x,y,m)

% Txy = xspec(x,y,m)
% cross performs FFT analysis of the two 
% sequences x and y using the Welch method of power spectrum
% estimation. The x and y sequences of n points are divided into
% k sections of m points each (m must be a power of two) with 
% overlap noverlap=m/2. Using an m-point FFT, successive sections 
% are Hanning windowed, FFT'd and accumulated.
% Make sure x and y are column vectors
x = x(:);		
y = y(:);
% Build the window
n = max(size(x));	                % Number of data points
noverlap = m/2;
k = fix((n-noverlap)/(m-noverlap));	% Number of windows
index = 1:m;
w = .5*(1 - cos(2*pi*(1:m)'/(m+1)));
w = w';	                                % Hanning window
KMU = k*norm(w)^2;                      % Normalizing scale factor
Pxx = zeros(1,m);                       % Dual sequence case.
Pxy = Pxx; 

for i=1:k
    dx = detrend(x(index))';
    dy = detrend(y(index))';

    xw = w.*dx;
    yw = w.*dy;

    index = index + (m - noverlap);
    Xx = fft(xw);
    Yy = fft(yw);
    Yy2 = abs(Yy).^2;
    Xx2 = abs(Xx).^2;
    Xy  = Yy .* conj(Xx);
    Pxx = Pxx + Xx2;
    Pxy = Pxy + Xy;
end

% Select first half and let 1st point (DC value) be replaced with 2nd point
select = [2 2:m/2];
Pxx = Pxx(select);
Pxy = Pxy(select);

% Done
Txy = Pxy./Pxx;


