/** \file PitchTracker.cc
    \brief A C++ implementation of the YIN pitch estimation algorithm
    See PitchTuner.h for a description of the YIN algorithm
 
 \section poly_interp_sec Polynomial Interpolation

 This section describes the algorithm for polynomial interpolation of the
 pitch estimate at the first cumulative distance function minimum.
 
 \e Golub and \e van \e Loan (in \e Matrix \e Computations, 3rd edition, 1996),
 define the Vandermonde matrix, \f$V\f$, as:
 \f[
 \begin{align}
 V=V(x_{0},\ldots,x_{n})=\left[\begin{array}{cccc}
 1 & 1 & \ldots & 1\\
 x_{0} & x_{1} & \ldots & x_{n}\\
 \vdots & \vdots &  & \vdots\\
 x_{0}^{n} & x_{1}^{n} & \ldots & x_{n}^{n}
 \end{array}\right]
 \end{align}
 \f]
 Solution of \f$V^{T}a=f\f$ is equivalent to polynomial interpolation:
 \f[
 \begin{equation}
 p(x)=\sum_{j=0}^{n}a_{j}x^{j}
 \end{equation}
 \f]
 where \f$p(x_{i})=f_{i}\f$ for \f$i=0\ldots n\f$.
 
 For distinct \f$x_{i}\f$ there is a unique polynomial of degree \e n that
 interpolates
 \f$(x_{0},f_{0}),\ldots\f$,\f$(x_{n},f_{n})\f$.
 
 \e Golub and \e van \e Loan (Section 4.6.1) show an algorithm for polynomial
 interpolation. Given \f$x(0\ldots n)\in\mathbb{R}^{n+1}\f$ with distinct entries
 and \f$f=f(0\ldots n)\in\mathbb{R}^{n+1}\f$, the following pseudo-code
 overwrites \f$f\f$ with the solution \f$a=a(0\ldots n)\f$ to the Vandermonde
 system \f$V(x_{0},\ldots,x_{n})^{T}a=f\f$:
 \code
     for k=0:b-1
       for i=n:-1:k+1
         f(i)=(f(i)-f(i-1))/(x(i)-x(i-k-1))
       end
     end
 
     for k-n-1:-1:0
       for i=k:n-1
         f(i)=f(i)-f(i+1)x(k)
       end
     end
 \endcode
 <hr>
 This algorithm requires \f$5n^{2}/2\f$ flops.
  
 Newton's divided differences interpolation polynomial converts the Vandermonde
 matrix to lower triangular form. The divided differences can be written
 in the form of a table:
 \f[
 \begin{align}
 \begin{array}{cccc}
 x_{0} & y_{0}\\
  &  & \left[y_{1},y_{0}\right]=\frac{y_{1}-y_{o}}{x_{1}-x_{0}}\\
   x_{1} & y_{1} &  &
 \left[y_{2},y_{1},y_{0}\right]=\frac{\frac{y_{2}-y_{1}}{x_{2}-x_{1}}
                      -\frac{y_{1}-y_{0}}{x_{1}-x_{0}}}{x_{2}-x_{0}}\\
  &  & \left[y_{2},y_{1}\right]=\frac{y_{2}-y_{1}}{x_{2}-x_{1}} & \vdots\\
 x_{2} & y_{2} & \vdots\\
 \vdots & \vdots
 \end{array}
 \end{align}
 \f]
 
 The interpolating polynomial uses the topmost entry in each column
 as coefficients. More precisely, for a set of \f$n+1\f$ data points
 \f$(x_{0},f_{0}),\ldots\f$,\f$(x_{n},f_{n})\f$
 where no two \f$x_{i}\f$ are the same , the interpolation polynomial
 in the Newton form is a linear combination of Newton basis polynomials:
 \f[
 \begin{equation}
 N(x)=\sum_{j=0}^{n}c_{j}n_{j}(x)
 \end{equation}
 \f]
 with the Newton basis polynomials defined as:
 \f[
 \begin{equation}
 n_{j}(x)=\prod_{i=0}^{j-1}(x-x_{i})
 \end{equation}
 \f]
 and the coefficients defined as:
 \f[
 \begin{equation}
 c_{j}=\left[y_{0},\ldots,y_{j}\right]
 \end{equation}
 \f]
 where \f$\left[y_{0},\ldots,y_{j}\right]\f$ is the notation for divided
 differences. Thus the Newton polynomial can be written as:
 \f[
 \begin{equation}
   N(x)=\left[y_{0}\right]+\left[y_{0},y_{1}\right](x-x_{0})
   +\ldots+\left[y_{0},\ldots,y_{k}\right](x-x_{0})
   \ldots(x-x_{n})
 \end{equation}
 \f]
 Using the basis polynomials \f$n_{j}(x)\f$ we have to solve:
 \f[
 \begin{align}
 \left[\begin{array}{ccccc}
 1 &  & \ldots &  & 0\\
 1 & x_{1}-x_{0} & & &\\
 1 & x_{2}-x_{0} & (x_{2}-x_{0})(x_{2}-x_{1}) & & \\
 \vdots & \vdots &  & \ddots & \vdots\\
 1 & x_{k}-x_{0} & \ldots &  & \prod_{j=0}^{n}(x_{n}-x_{j})
 \end{array}\right]\left[\begin{array}{c}
 c_{0}\\
 \vdots\\
 c_{n}
 \end{array}\right]=\left[\begin{array}{c}
 y_{0}\\
 \vdots\\
 y_{n}
 \end{array}\right]
 \end{align}
 \f]
 
 This matrix can be solved recursively by
 \f[
 \begin{equation}
 \sum_{i=0}^{j}c_{i}n_{i}(x_{j})=y_{j},\,j=0,\ldots,n
 \end{equation}
 \f]
 
 For a quadratic:
 \f[
 \begin{align*}
 c_{0}&=y_{0}\\
 c_{0}+c_{1}(x_{1}-x_{0})&=y_{1}\\
   c_{0}+c_{1}(x_{2}-x_{0})+c_{2}(x_{2}-x_{0})
   (x_{2}-x_{1})&=y_{2}
 \end{align*}
 \f]
 so:
 \f[
 \begin{align*}
 c_{0}&=y_{0}\\
 c_{1}&=\frac{1}{x_{1}-x_{0}}\left[y_{1}-c_{0}\right]\\
 c_{2}&=\frac{1}{x_{2}-x_{1}}\left[\frac{1}{x_{2}-x_{0}}
   (y_{2}-c_{0})-c_{1}\right]
 \end{align*}
 \f]
 
 Expanding \f$p(x)\f$:
 \f[
 \begin{align*}
   p(x) & = 
 c_{0}+c_{1}(x-x_{0})+c_{2}(x-x_{0})(x-x_{1})\\
  & =  (c_{0}-x_{0}(c_{1}-c_{2}x_{1}))
   +(c_{1}-c_{2}(x_{1}+x_{0}))x
   +c_{2}x^{2}
 \end{align*}
 \f]
 
 Recall that for \f$p(x)=ax^{2}+bx+c\f$ with \f$a>0\f$, the minimum
 occurs at \f$(-\frac{b}{2a},c-\frac{b^{2}}{4a})\f$.
*/

// StdC++ headers
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <iterator>
#include <numeric>

// StdC headers
#include <cmath>
#include <ctime>
#include <assert.h>

// Local include files
#include "saSample.h"
#include "saInputStream.h"
#include "PreProcessor.h"
#include "PitchTracker.h"

using namespace SimpleAudio;

/// \class PitchTrackerImpl
/// \brief Implementation of the YIN pitch estimation algorithm
class PitchTrackerImpl : public PitchTracker
{
public:
  /// \brief Constructor for PitchTrackerImpl
  /// \param _sampleRate sample rate in Hz
  /// \param _WindowLength window size in samples
  /// \param _MaxLags correlation width in samples
  /// \param _threshold cumulative difference threshold
  /// \param _removeDC if true, remove slowly varying DC
  /// \param _debug
  PitchTrackerImpl( float _sampleRate, 
                    std::size_t _WindowLength, 
                    std::size_t _MaxLags, 
                    float _threshold,
                    bool _removeDC,
                    bool _debug);
  
  /// Destructor for PitchTracker
  ~PitchTrackerImpl() throw();

  /// Read a single pitch value from the pitch tracker
  ///  \return \c float
  float EstimatePitch( const std::deque<saSample>& );

  /// Read the voiced flag from the pitch tracker
  ///  \return \c bool
  bool Voiced() { return voiced; }

private:
  /// Disallow assignment to PitchTrackerImpl
  PitchTrackerImpl& operator=( const PitchTrackerImpl& );
  
  /// Disallow copy constructor of PitchTrackerImpl
  PitchTrackerImpl( const PitchTrackerImpl& );

  /// Sample rate in Hz
  float sampleRate;

  /// Window size in samples
  std::size_t WindowLength; 

  /// Correlation width in samples
  std::size_t MaxLags; 

  /// Threshold for cumulative difference
  float threshold;

  /// Remove slowly varying DC
  bool removeDC;

  // Debug
  bool debug;

  /// Voiced flag
  bool voiced;

  /// Save pitch estimate for debugging
  std::vector<float> pitchList;

  /// Save DC for debugging
  std::list< std::vector<saSample> > dDCList;

  /// Save difference for debugging
  std::list< std::vector<saSample> > dtList;

  /// Save cumulative difference for debugging
  std::list< std::vector<saSample> > cdtList;

  /// Save minimum of CDT for debugging
  std::vector<float> minCDTList;
};

PitchTrackerImpl::PitchTrackerImpl(float _sampleRate, 
                                   std::size_t _WindowLength, 
                                   std::size_t _MaxLags, 
                                   float _threshold,
                                   bool _removeDC,
                                   bool _debug)
  : sampleRate( _sampleRate ),  
    WindowLength( _WindowLength ),
    MaxLags( _MaxLags ),
    threshold( _threshold ),
    removeDC( _removeDC ),
    debug( _debug ),
    voiced( false )
{
}

PitchTrackerImpl::~PitchTrackerImpl() throw()
{
  if (debug)
    {
      {
        std::ofstream of("pitch.txt");
        copy(pitchList.begin(), pitchList.end(), 
             std::ostream_iterator<float>(of, "\n"));
      } 
      {
        std::ofstream of("dDC.txt");
        for(std::list< std::vector<saSample> >::iterator j = dDCList.begin(); 
            j != dDCList.end(); 
            j++)
          {
            copy(j->begin(), j->end(), 
                 std::ostream_iterator<saSample>(of, " "));
            of << "\n";
          }
      }
      {
        std::ofstream of("dt.txt");
        for(std::list< std::vector<saSample> >::iterator j = dtList.begin(); 
            j != dtList.end(); 
            j++)
          {
            copy(j->begin(), j->end(), 
                 std::ostream_iterator<saSample>(of, " "));
            of << "\n";
          }
      }
      {
        std::ofstream of("cdt.txt");
        for(std::list< std::vector<saSample> >::iterator j = cdtList.begin(); 
            j != cdtList.end(); 
            j++)
          {
            copy(j->begin(), j->end(), 
                 std::ostream_iterator<saSample>(of, " "));
            of << "\n";
          }
      }
      {
        std::ofstream of("minCDT.txt");
        copy(minCDTList.begin(), minCDTList.end(), 
             std::ostream_iterator<float>(of, "\n"));
      }
    }
}

/// Estimate the pitch from the waveform in y
/// \param y a list of saSample values
/// \return the pitch estimate
float PitchTrackerImpl::EstimatePitch(const std::deque<saSample>& y)
{
  // Sanity check
  float pitch = 0;
  voiced = false;
  if (y.size() < WindowLength+MaxLags+1)
    {
      throw std::runtime_error("Not enough samples");
    }

  // Sample autocorrelation
  std::vector<saSample> r(MaxLags+1,0);
  r[0] = std::inner_product(y.rbegin(), 
                            y.rbegin()+static_cast<long>(WindowLength), 
                            y.rbegin(), 
                            saSample(0));
  if ( r[0] < threshold )
    {
      return pitch;
    }

  // Loop calculating autocorrelations 
  saSample dDC0 = 
    std::accumulate( y.rbegin(),
                     y.rbegin()+static_cast<long>(WindowLength),
                     saSample(0) );
  saSample dDC = 0;
  saSample dT = 0;
  saSample dTsum = 0;
  saSample cdT = 0;
  float minT = 0;
  float minCDT = 1;
  bool minFound = false;
  std::vector<saSample> vdT(MaxLags+1,0);
  std::vector<saSample> vdDC(MaxLags+1,0);
  std::vector<saSample> vcdT(MaxLags+1,0);
  vcdT[0] = 1;
  for (unsigned int T=1; T<=MaxLags; T++)
    {
      saSample t = static_cast<saSample>(T);
      
      // Calculate this lag. Recall that the newest sample is at y.end()
      r[T] = std::inner_product(y.rbegin(),
                                y.rbegin()+static_cast<long>(WindowLength), 
                                y.rbegin()+static_cast<long>(T),
                                saSample(0));

      // Calculate autocorrelation at this lag
      saSample r0T = std::inner_product(y.rbegin()+T, 
                                        y.rbegin()+T+
                                        static_cast<long>(WindowLength), 
                                        y.rbegin()+T, 
                                        saSample(0));

      // Difference function
      dT = r[0] + r0T - (2*r[T]);

      // Remove DC component
      // Test with:
      // fs=48000; f0=480;
      // t=0.25*sin(2*pi*(0:((fs/10)-1))*f0/fs)+(0.5*((0:((fs/10)-1))/(fs/10)));
      // audiowrite("sin480HzDC.wav",t,fs);
      if (removeDC)
        {
          dDC = dDC0 - std::accumulate(y.rbegin()+T,
                                       y.rbegin()+T+
                                       static_cast<long>(WindowLength),
                                       saSample(0));
          dDC *= dDC;
          dDC /= static_cast<saSample>(WindowLength);
          dT -= dDC;
        }
      
      // Cumulative difference
      dTsum += dT;
      cdT = dTsum == 0 ? 0 : (dT*t)/dTsum;
      vcdT[T] = cdT;

      // Check for a minimum
      if ( !minFound &&
           (T>=2) && 
           (vcdT[T-2] != 0) &&
           (vcdT[T] != 0) &&
           (vcdT[T-2] >= vcdT[T-1]) && 
           (vcdT[T] >= vcdT[T-1]) )
        {
          // Parabolic interpolation
          saSample c0 = vcdT[T-2];
          saSample c1 = vcdT[T-1]-c0;
          saSample c2 = ((vcdT[T]-c0)/2)-c1;
          if (c2 == 0)
            {
              continue;
            }

          // Coefficients of p(x) = a*x^2 + b*x + c
          saSample a = c2;
          saSample b = c1 - (c2*((2*t)-3));
          saSample c = c0 - ((t-2)*(c1-(c2*(t-1))));

          // Find minimum value and period
          saSampleTo<float> conv;
          minT = -conv(b)/conv(2*a);
          float minY =  conv(c) - (conv(b*b)/conv(4*a));

          // Store the minimum
          if (minY < minCDT)
            {
              minCDT = minY;
            }

          // Pitch found?
          if ( minY < threshold )
            {
              minFound = true;
              voiced = true;
              pitch = sampleRate/minT;
            }
        }

      if (debug)
        {
          vdT[T] = dT;
          if (removeDC)
            {
              vdDC[T] = dDC;
            }
          continue;
        }
      else if(minFound)
        {
          break;
        }
    }

  // Done
  if (debug)
    {
      dDCList.push_back(vdDC);
      dtList.push_back(vdT);
      cdtList.push_back(vcdT);
      pitchList.push_back(pitch);
      minCDTList.push_back(minCDT);
    }
  
  return pitch;
}

PitchTracker* PitchTrackerCreate( float sampleRate, 
                                  std::size_t WindowLength, 
                                  std::size_t MaxLags, 
                                  float threshold,
                                  bool removeDC,
                                  bool debug)
{
 return new PitchTrackerImpl(sampleRate, WindowLength, MaxLags, threshold,
                             removeDC, debug);
}
