/**
 \file ButterworthFilter.h
 \brief Implemetation of Butterworth filters as a cascade of second-order
        state-space sections

  This file implements Butterworth second- and third-order high-pass filters
  and a Butterworth fourth-order low-pass filter as a cascade of low-noise
  second-order state-variable sections or as the parallel combination of
  two all-pass state-variable sections.

  For a low-pass Butterworth filter, the squared magnitude response is [1] :
   \f[
       \left|\hat{H}(s)\right|^{2} = \frac{1}{1+\omega^{2n}}
   \f]

  The \f$2n\f$ poles of the Butterworth squared-magnitude response are
  evenly spaced around a circle in the \f$s\f$-plane. In the left half
  \f$s\f$-plane the angles are:
   \f[
   \theta_{k}=\frac{\pi}{2}\left(1+\frac{2k-1}{n}\right)\;,\quad 1\le k\le n
   \f]

  If the cutoff frequency in the \f$s\f$-plane is \f$\omega_{c}\f$ then
  first order low-pass sections are:
   \f[
       \frac{\omega_{c}}{s+\omega_{c}}
   \f]

  and second order low-pass sections are:
   \f[
       \frac{\omega_{c}^{2}}{s^{2}-2\omega_{c}\cos\theta_{k}s+\omega_{c}^{2}}
   \f]

  The \f$s\f$-plane Butterworth response is transformed to the \f$z\f$-plane
  by:
   \f[
       H(z) = \hat{H}\left(\frac{1-z^{-1}}{1+z^{-1}}\right)
   \f]

  On the \f$z\f$-plane unit circle, \f$z=e^{\imath\Omega T}\f$, where
  \f$T\f$ is the sampling interval and:
   \f[
       H(e^{\imath\Omega T}) =
         \hat{H}\left(\imath\tan\frac{\Omega T}{2}\right)
   \f]

  For each filter, first find the Butterworth pole locations and the
  corresponding transfer function for each second-order section:
   \f[
   H(z)=d+\frac{q_1 z^{-1} + q_2 z^{-2}}{1 + p_1 z^{-1} + p_2 z^{-2}}
   \f]

  Next, find the optimal low-noise second order state variable sections [2]
  or, for high-pass filters the Stoyanov [3] LS1 and LS2a, or
  Gray-and-Markel [4] GM1 and GM2, first and second  order state variable
  sections. The Stoyanov and Gray-and-Markel all-pass filters are
  "structurally lossless" in the sense of \e Vaidyanathan et al. [5] ;
  the all-pass response is retained when the coefficients are truncated.

  See:
  
  [1] Appendix 6A, "Design of Analog Filter Prototypes", in "Digital
      Signal Processing", R. A. Roberts and C. T. Mullis, Addison-Wesley,
      1987, ISBN 0-201-16350-0
      
  [2] "New second-order state-space structures for realizing low round off
      noise digital filters", B. W. Bomar, IEEE Transactions on Acoustics, 
      Speech and  Signal Processing, Vol. ASSP-33, pp. 106-110, Feb. 1985
      
  [3] "Design and Realization of Efficient IIR Digital Filter Structures
      Based on Sensitivity Minimizations", G. Stoyanov, Z. Nikolova, K. Ivanova
      and V. Anzova, Proceedings TELSIKS 2007, pp. 299-308, Sept. 2007
      
  [4] "Digital Lattice And Ladder Filter Synthesis", A. H. Gray and J. D.
      Markel, IEEE Transactions on Audio and Electroacoustics, Vol. 21, No. 6,
      pp. 491-500, Dec. 1973
      
  [5] "A New Approach to the Realization of Low-Sensitivity IIR Digital Filters",
      P. P. Vaidyanathan,S. K. Mitra and Y. Nuevo, IEEE Transactions on
      Acoustics, Speech and Signal Processing, Vol. ASSP-34, No. 2, pp. 350-361,
      April 1986
  
  ### Testing with Octave ###
     
  1. Create a noise sequence in Octave:
       \code
       Fs = 48000
       N = Fs*10
       n = rand(N,1)-0.5;
       n = 0.5*n/std(n);
       audiowrite('noise.wav',n,Fs);
       \endcode

  2. Run Butterworth_test:
       \code
       bin/ButterworthFilter_test --inFile noise.wav --outFile y.wav 
                                  --seconds 10 --cutoff 100 --high_pass_GM
       \endcode

  3. Analyse the result in Octave:
       \code
       [y,Fs]=audioread('y.wav');
       Nfreq=16384
       td=xspec(n,y,2*Nfreq); 
       plot((0:(Nfreq-1))*Fs/(2*Nfreq), 20*log10(abs(td(1:Nfreq))))
       axis([0 200 -50 5]);
       \endcode
*/

// StdC++ headers
#include <memory>
#include <numbers>

// StdC headers
#include <cmath>

// Local include files
#include "Filter.h"
#include "saSample.h"

using namespace SimpleAudio;

#ifndef __BUTTERWORTHFILTER_H__
#define __BUTTERWORTHFILTER_H__

/// \struct pdq_t
/// \brief Represetation of a second order transfer function
typedef struct _pqd_t { float p1=0, p2=0, q1=0, q2=0, d=0; } pqd_t;

/// \struct Abcd_t
/// \brief Represetation of a second order state variable filter
typedef struct _Abcd_t {
  float a11=0, a12=0, a21=0, a22=0, b1=0, b2=0, c1=0, c2=0, d=0; } Abcd_t;

/// Utility function to convert the wc and theta specification of a second
/// order Butterworth high-pass filter to pdq_t format
static inline void
HighPassButterworthWcThetaToPqd(const float wc, const float theta, pqd_t &pqd)
{
  float lambda = cosf(theta);
  float k1 = wc*wc;
  float k2 = 2*wc*lambda;
  float d = 1/(1+k1-k2);
  float p1 = 2*(k1-1)*d;
  float p2 = (1+k1+k2)*d;
  float q1 = (-2-p1)*d;
  float q2 = (1-p2)*d;

  pqd = { p1, p2, q1, q2, d };
}

/// Utility function to convert the wc and theta specification of a second
/// order Butterworth low-pass filter to pdq_t format
static void
LowPassButterworthWcThetaToPqd(const float wc, const float theta, pqd_t &pqd)
{
  float lambda = cosf(theta);
  float k1 = wc*wc;
  float k2 = 2*wc*lambda;
  float d  = k1/(1+k1-k2);
  float p1 = 2*(k1-1)/(1+k1-k2);
  float p2 = (1+k1+k2)/(1+k1-k2);
  float q1 = (2-p1)*d;
  float q2 = (1-p2)*d;

  pqd = { p1, p2, q1, q2, d };
}

/// Utility function to convert the pdq_t specification of a second
/// order filter to low-noise Abcd_t format
static void
PqdToLowNoiseAbcd(const pqd_t pqd, Abcd_t &Abcd)
{
  // Design equations for a minimum noise 2nd order state variable section
  float p1 = pqd.p1;
  float p2 = pqd.p2;
  float q1 = pqd.q1;
  float q2 = pqd.q2;
  float d = pqd.d;

  float v1 = q2/q1;
  float v2 = sqrtf((v1*v1)-(p1*v1)+p2);
  float v3 = v1-v2;
  float v4 = v1+v2;
  float v5 = p2-1;
  float v6 = p2+1;
  float v7 = v5*((v6*v6)-(p1*p1));
  float v8 = (p1*p1/4)-p2;
  
  float b1 = sqrtf(v7/((2*p1*v3) - (v6*(1+(v3*v3)))));
  float b2 = sqrtf(v7/((2*p1*v4) - (v6*(1+(v4*v4)))));
  float a21 = sqrtf((((b2*b2)+v5)*v8)/((b1*b1)+v5));
  float a11 = -p1/2;
  float a22 = a11;
  float a12 = v8/a21;
  float c1 = q1/(2*b1);
  float c2 = q1/(2*b2);

  Abcd = { a11, a12, a21, a22, b1, b2, c1, c2, d };
} 

/// \class StateVariable2ndOrderSection
/// \brief Representation of a second order state-variable filter
/// \tparam T the type of the internal states
template <typename T>
class StateVariable2ndOrderSection : public Filter<T>
{
public:
  StateVariable2ndOrderSection () { } 
  
  ~StateVariable2ndOrderSection() throw() {}

  /// Initialise the state variable filter coefficients
  /// \param Abcd a structure containing the coefficients
  void Init (const Abcd_t Abcd) 
  {
    a11 = Abcd.a11; a12 = Abcd.a12; a21 = Abcd.a21; a22 = Abcd.a22;
    b1 = Abcd.b1; b2 = Abcd.b2; c1 = Abcd.c1; c2 = Abcd.c2; d = Abcd.d;
    x1 = 0; x2 = 0;
  }

  /// Perform the state update
  /// \param u the input
  /// \return the filter section output value
  T operator()(T& u)
  {
    // State variable update
    T x1n = (a11*x1) + (a12*x2) + (b1*u);
    T x2n = (a21*x1) + (a22*x2) + (b2*u);
    T y   =  (c1*x1) +  (c2*x2) +  (d*u);
    x1 = x1n;
    x2 = x2n;
    return y;
  }

private:
  /// Filter coefficients
  T a11, a12, a21, a22, b1, b2, c1, c2, d;
  
  /// Filter state
  T x1, x2;
  
  // Prevent copying
  StateVariable2ndOrderSection(StateVariable2ndOrderSection&);
  StateVariable2ndOrderSection& 
  operator=(StateVariable2ndOrderSection&);
};

/// \class StateVariable1stOrderSection
/// \brief Representation of a first order state-variable filter
/// \tparam T the type of the internal states
template <typename T>
class StateVariable1stOrderSection : public Filter<T>
{
public:
  StateVariable1stOrderSection () : x(0) { }
  
  /// Initialise the state variable filter coefficients
  /// \param _a a 
  /// \param _b b
  /// \param _c c
  /// \param _d d
  void Init (float _a, float _b, float _c, float _d) 
  {
    a = _a; b = _b; c = _c; d = _d;
  }

  ~StateVariable1stOrderSection() throw() {}

  /// Perform the state update
  /// \param u the input
  /// \return the filter section output value
  T operator()(T& u)
  {
    T xn = (a*x) + (b*u);
    T y  = (c*x) + (d*u);
    x  = xn;
    return y;
  }
  
private:
  /// Filter coefficients
  T a, b, c, d;
  
  /// Filter state
  T x;
  
  // Prevent copying
  StateVariable1stOrderSection(StateVariable1stOrderSection&);
  StateVariable1stOrderSection& 
  operator=(StateVariable1stOrderSection&);
};

/// \class ButterworthHighPass2ndOrderFilter
/// \brief Implementation of a Butterworth second order state-variable high
/// pass filter
/// \tparam T the type of the internal states
template <typename T>
class ButterworthHighPass2ndOrderFilter : public Filter<T>
{
public:
  /// Constructor
  /// \param cutoffFrequency the filter high pass cutoff frequency
  /// \param sampleRate the sample rate of the input waveform 
  ButterworthHighPass2ndOrderFilter(float cutoffFrequency,
                                    std::size_t sampleRate)
  {
    // pi
    const float pi = std::numbers::pi_v<float>;

    // Warp the frequency scale
    float wc = tanf(pi*cutoffFrequency/(float)sampleRate);

    // Find the Butterworth pole positions and z-plane transfer function
    pqd_t pqd;
    HighPassButterworthWcThetaToPqd(wc, 3*pi/4, pqd);

    // Find the state variable description
    Abcd_t Abcd;
    PqdToLowNoiseAbcd(pqd, Abcd);

    // Initialise the filter
    f.Init(Abcd);
  }
  
  ~ButterworthHighPass2ndOrderFilter() throw() { }

  /// Filter a sample
  /// \param u the input
  /// \return the filter output value
  T operator()(T& u)
  {
    T y = f(u);
    return y;
  }

private:
  /// The filter section
  StateVariable2ndOrderSection<T> f;
};

/// \class ButterworthLowPass4thOrderFilter
/// \brief Implementation of a Butterworth fourth order state-variable low
/// pass filter as the cascade of two second order low noise state variable
/// filter sections
/// \tparam T the type of the internal states
template <typename T>
class ButterworthLowPass4thOrderFilter : public Filter<T>
{
public: 
  /// Constructor
  /// \param cutoffFrequency the filter high pass cutoff frequency
  /// \param sampleRate the sample rate of the input waveform 
  ButterworthLowPass4thOrderFilter(float cutoffFrequency, 
                                   std::size_t sampleRate)
  {
    // pi
    float pi = std::numbers::pi_v<float>;

    // Warp the frequency scale
    float wc = tanf(pi*cutoffFrequency/(float)sampleRate);
                 
    // Find the Butterworth pole positions in the upper left s-plane quadrant
    // For a 4th order filter use a cascade of two second order sections
    // with poles at 5*pi/8,11*pi/8 and 7*pi/8,9*pi/8 on the s-plane wc circle.

    pqd_t pqd;
    Abcd_t Abcd;
    
    LowPassButterworthWcThetaToPqd(wc, 5*pi/8, pqd);
    PqdToLowNoiseAbcd(pqd, Abcd);
    f1.Init(Abcd);
        
    LowPassButterworthWcThetaToPqd(wc, 7*pi/8, pqd);
    PqdToLowNoiseAbcd(pqd, Abcd);
    f2.Init(Abcd);
  }

  ~ButterworthLowPass4thOrderFilter() throw() { }

  /// Filter a sample with a cascade of second order sections
  /// \param u the input
  /// \return the filter output value
  T operator()(T& u)
  {
    T tmp = f1(u);
    T y   = f2(tmp);
    return y;
  }
  
private:
  /// The filter sections
  StateVariable2ndOrderSection<T> f1;
  StateVariable2ndOrderSection<T> f2;
};

/// \class ButterworthHighPass3rdOrderFilter
/// \brief Implementation of a Butterworth third order state-variable high
/// pass filter as the parallel sum of Gray and Markel second order and first
/// order allpass filters
/// \tparam T the type of the internal states
template <typename T>
class ButterworthHighPass3rdOrderFilter : public Filter<T>
{
public:
  /// Constructor
  /// \param cutoffFrequency the filter high pass cutoff frequency
  /// \param sampleRate the sample rate of the input waveform 
  ButterworthHighPass3rdOrderFilter(float cutoffFrequency,
                                    std::size_t sampleRate)
  {
    // pi
    const float pi = std::numbers::pi_v<float>;

    // Warp the frequency scale
    float wc = tanf(static_cast<float>(cutoffFrequency) * pi /
                    static_cast<float>(sampleRate));

    // Get the Butterworth pole positions in the upper left s-plane quadrant
    // For a 3rd order filter use a cascade of first and second order sections
    // with poles at 2*pi/3, 4*pi/3 and pi on the s-plane wc circle.
    float theta = 2*pi/3;

    // Implement the filter as the parallel combination of Gray and Markel
    // GM2 and GM1 all-pass filters
    
    // Initialise Gray and Markel GM2 second order section

    // Find the denominator polynomial a0 + a1/z + a2/(z^2)
    float lambda = cosf(theta);
    float a0=1-(2*wc*lambda)+(wc*wc);
    float a1=2*((wc*wc)-1);
    // float a2=1+(2*wc*lambda)+(wc*wc); // Not used!
    float re_r2=-a1/(2*a0);
    float im_r2=(4*wc*sinf(theta))/(2*a0);
    float abs_r2=sqrtf((re_r2*re_r2)+(im_r2*im_r2));
    float arg_r2=atan2f(im_r2,re_r2);
 
    // Find the coefficients of the Gray and Markel GM2 all-pass filter
    float B1=-2*abs_r2*cosf(arg_r2);
    float B2=abs_r2*abs_r2;
    float k2=B2;
    float e1=-1;
    float k1=B1/(1+B2);
    float e2=-1;

    // Convert the Gray and Markel GM2 all-pass filter section to state
    // variable form
    float a11=-k1;
    float a12=(k1*e1)+1;
    float a21=((k1*e1)-1)*k2;
    float a22=-k2*k1;
    float b1=0;
    float b2=(k2*e2)+1;
    float c1=((e1*k1-1)*(e2*k2))-(e1*k1)+1;
    float c2=k1*(1-(e2*k2));
    float d=k2;
    Abcd_t Abcd = {a11,a12,a21,a22,b1,b2,c1,c2,d};
    f2.Init(Abcd);

    // Initialise Gray and Markel GM1 first order section

    // Find the root of the 1st order denominator polynomial 1-r1/z
    float r1=(1-wc)/(1+wc);

    // Find the coefficient of the Gray and Markel GM1 filter
    float k=-r1;
    float e=-1;
    
    // Convert the Gray and Markel GM1 all-pass filter section to state
    // variable form
    float a=-k;
    float b=1+(k*e);
    float c=1-(k*e);
    float dd=k;
    f1.Init(a,b,c,dd);
  } 
 
  ~ButterworthHighPass3rdOrderFilter() throw() { }

  /// Filter a sample with the parallel combination of a second order and
  /// a first order section.
  /// \param u the input
  /// \return the filter output value
  T operator()(T& u)
  {
    T y1 = f1(u);
    T y2 = f2(u);
    T y = (y2-y1)/2;
    
    return y;
  }

private:
  /// The filter sections
  StateVariable1stOrderSection<T> f1;
  StateVariable2ndOrderSection<T> f2;
};

/// \class ButterworthHighPassStoyanov3rdOrderFilter
/// \brief Implementation of a Butterworth third order state-variable high
/// pass filter as the parallel sum of Stoyanov second order and first
/// order allpass filters
/// \tparam T the type of the internal states
template <typename T>
class ButterworthHighPassStoyanov3rdOrderFilter : public Filter<T>
{
public:
  /// Constructor
  /// \param cutoffFrequency the filter high pass cutoff frequency
  /// \param sampleRate the sample rate of the input waveform 
  ButterworthHighPassStoyanov3rdOrderFilter(float cutoffFrequency,
                                            std::size_t sampleRate)
  {
    // pi
    const float pi = std::numbers::pi_v<float>;

    // Warp the frequency scale
    float wc = tanf(static_cast<float>(cutoffFrequency) * pi /
                    static_cast<float>(sampleRate));

    // Get the Butterworth pole positions in the upper left s-plane quadrant
    // For a 3rd order filter use a cascade of first and second order sections
    // with poles at 2*pi/3, 4*pi/3 and pi on the s-plane wc circle.
    float theta = 2*pi/3;

    // Implement the filter as the parallel combination of
    // Stoyanov LS2a and LS1 all-pass filters
    
    // Initialise Stoyanov LS2a second order section

    // Find the denominator polynomial a0 + a1/z + a2/(z^2)
    float lambda = cosf(theta);
    float a0=1-(2*wc*lambda)+(wc*wc);
    float a1=2*((wc*wc)-1);
    // float a2=1+(2*wc*lambda)+(wc*wc); // Not used!
    float re_r2=-a1/(2*a0);
    float im_r2=(4*wc*sinf(theta))/(2*a0);
    float abs_r2=sqrtf((re_r2*re_r2)+(im_r2*im_r2));
    float arg_r2=atan2f(im_r2,re_r2);
 
    // Find the coefficients of the Stoyanov LS2a all-pass filter
    float B1=-2*abs_r2*cosf(arg_r2);
    float B2=abs_r2*abs_r2;
    float C2=1-B2;
    float C1=(B1+2-C2)/2;

    // Convert the Stoyanov LS2a all-pass filter section to state variable form
    float a11 = 1-C1;
    float a12 = -C1;
    float a21 = -C2-C1+2;
    float a22 = -C2-C1+1;
    float b1  = C1;
    float b2  = C2+C1-2;
    float c1  = C2;
    float c2  = C2;
    float d   = 1-C2;
    Abcd_t Abcd = {a11,a12,a21,a22,b1,b2,c1,c2,d};
    f2.Init(Abcd);

    // Initialise Stoyanov LS1 first order section

    // Find the root of the 1st order denominator polynomial 1-r1/z
    float r1 = (1-wc)/(1+wc);

    // Find the coefficient of the Stoyanov LS1 filter
    float C = 1-r1;

    // Convert the Stoyanov LS1 all-pass filter section to state variable form
    // (Note the missing - at the left of the output in Stoyanov's Figure 3(b).)
    float a = 1-C;
    float b = C;
    float c = 2-C;
    float dd = C-1;
    f1.Init(a,b,c,dd);
  }
 
  ~ButterworthHighPassStoyanov3rdOrderFilter() throw() { }

  /// Filter a sample with the parallel combination of a second order and
  /// a first order section.
  /// \param u the input
  /// \return the filter output value
  T operator()(T& u)
  {
    T y1 = f1(u);
    T y2 = f2(u);
    T y = (y2-y1)/2;
    
    return y;
  }

private:
  /// The filter sections
  StateVariable1stOrderSection<T> f1;
  StateVariable2ndOrderSection<T> f2;
};

#endif
