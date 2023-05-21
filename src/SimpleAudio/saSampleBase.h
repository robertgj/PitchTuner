/** \file saSampleBase.h
    \brief Public interface for a simple fixed point or integer audio sample.
    
 \section fixed_point_sec Fixed point arithmetic
 
 This section describes the fixed point arithmetic implemented when
 \c saSampleBase is not a floating point data type.
 
 Approximate a real number by \f$a=A.2^{2n}+A_{h}.2^{n}+A_{l}\f$ where \f$A\f$
 is an integer and \f$A_{h}\f$ and \f$A_{l}\f$ are \f$n\f$ bit integers and the
 binary point is at \f$2^{2n}\f$.  

 Approximate multiplication by:
 \f[
 \begin{align*}
  a.b = & \left[(A.2^{2n}+A_{h}.2^{n}+A_{l}).(B.2^{2n}+B_{h}.2^{n}+B_{l})
          \right].2^{-2n} \\
      = & A.B.2^{2n}   + \ldots \\
        & A.B_{h}.2^{n} +   A_{h}.B.2^{n}  +  \ldots \\
        & A.B_{l} + A_{h}.B_{h} + A_{l}.B  +  \ldots \\
        & A_{h}.B_{l}.2^{-n} + A_{l}.B_{h}.2^{-n} +  \ldots \\
        & A_{l}.B_{l}.2^{-2n}
 \end{align*}
 \f]

 Discard the last term and collect the other terms so that:
 \f[
 \begin{align*}
  a.b \approx & \left[(A.2^{n}+A_{h}).(B.2^{n}+B_{h})\right] + 
          \left[(A.2^{n}+A_{h}).B_{l}\right].2^{-n}  + 
          \left[(B.2^{n}+B_{h}).A_{l}\right].2^{-n} \\
      = & \left[(a.2^{-n}).(b.2^{-n})\right] + 
          \left[(a.2^{-n}).B_{l}\right].2^{-n} + 
          \left[(b.2^{-n}).A_{l}\right].2^{-n}
 \end{align*}
 \f]

 Approximate signed division by:
 \f[
 \begin{align*}
 a/b & \approx a.\left[(\frac{2^{4n}}{b}).2^{-2n}\right]
 \end{align*}
 \f]
 Division requires that the length of the standard signed
 integral type be greater than \f$(2^{4n}+1)\f$ bits.
*/

// StdC++ headers
#include <iosfwd>
#include <stdexcept>
#include <limits>
#include <type_traits>
#include <cstdint>

#include "saDeviceType.h"

#ifndef __SASAMPLEBASE_H__
#define __SASAMPLEBASE_H__

namespace SimpleAudio
{
  template <typename T, typename L, bool S, bool O, bool U> class saSampleBase;

  /// Forward declaration of << for saSampleBase
  template <typename T, typename L, bool S, bool O, bool U> std::ostream& 
  operator<<(std::ostream&, const saSampleBase<T,L,S,O,U>&);

  /// \class saSampleBase
  /// \brief A class for audio samples
  /// \tparam Stored type               
  /// \tparam Internal long type        
  /// \tparam Saturate on overflow      
  /// \tparam Overflow causes exception
  /// \tparam Underflow causes exception
  template < typename T, 
             typename L, 
             bool S,     
             bool O,     
             bool U >    
  class saSampleBase
  {
  private:
    /// The sample!
    T sample;

    /// Compile-time assertions
    void CompileTimeAssertions()
    {
      static_assert(std::is_signed<T>(), "expect T signed");
      static_assert(std::is_signed<L>(), "expect L signed");
      static_assert(std::is_signed<saDeviceType>(),"expect saDeviceType signed");
      static_assert(std::is_integral<saDeviceType>(),
                    "expect saDeviceType integral");
      static_assert(std::is_floating_point<T>() == std::is_floating_point<L>(),
                    "expect both T and L floating point or integral types");
      static_assert(std::numeric_limits<L>::digits > 
                    2*(std::numeric_limits<T>::digits),
                    "expect digits of L > 2 * digits of T ");
      static_assert(std::numeric_limits<T>::digits > 
                    std::numeric_limits<saDeviceType>::digits,
                    "expect digits of T > digits of saDeviceType");
      static_assert(std::numeric_limits<T>::digits > saInternalBinaryPoint,
                    "expect digits of T > saInternalBinaryPoint");
    }

  public:
    // Specialisation of saSampleBase internal types
    constexpr static size_t saInternalNumBits = std::numeric_limits<T>::digits;
    constexpr static L saInternalMax =
      static_cast<L>(std::numeric_limits<T>::max());
    /// If T is floating point then std::numeric_limits<T>::min() is ~ 1e-38
    constexpr static L saInternalMin = std::is_integral<T>() ?
      static_cast<L>(std::numeric_limits<T>::min()) : -saInternalMax;
    constexpr static size_t saInternalBinaryPoint = 15;
    constexpr static T saInternalOne = static_cast<T>(1<<saInternalBinaryPoint);
    constexpr static L saInputMax = saInternalMax/static_cast<L>(saInternalOne);
    constexpr static L saInputMin = saInternalMin/static_cast<L>(saInternalOne);

    // Destructor
    ~saSampleBase() throw() {}

    /// Default constructor
    saSampleBase() : sample(0) { }

    /// Constructor
    /// \param a initial value
    saSampleBase(const saSampleBase& a) : sample(a.sample) { }
 
    /// Cnstructor
    /// \param value int16_t initial value
    saSampleBase(const int16_t value) : sample(0)
    {
      L result = value;
      result *= static_cast<L>(saInternalOne);
      CheckResult<int16_t>(value, result);
      sample = static_cast<T>(result);
    }

    /// Cnstructor
    /// \param value uint16_t initial value
    saSampleBase(const uint16_t value) : sample(0)
    {
      L result = value;
      result *= static_cast<L>(saInternalOne);
      CheckResult<uint16_t>(value, result);
      sample = static_cast<T>(result);
    }

    /// Cnstructor
    /// \param value int32_t initial value
    saSampleBase(const int32_t value) : sample(0)
    {
      L result = value;
      result *= static_cast<L>(saInternalOne);
      CheckResult<int32_t>(value, result);
      sample = static_cast<T>(result);
    }

    /// Cnstructor
    /// \param value uint32_t initial value
    saSampleBase(const uint32_t value) : sample(0)
    {
      L result = value;
      result *= static_cast<L>(saInternalOne);
      CheckResult<uint32_t>(value, result);
      sample = static_cast<T>(result);
    }

    /// Cnstructor
    /// \param value int64_t initial value
    saSampleBase(int64_t value) : sample(0)
    {
      L result = static_cast<L>(value)*saInternalOne;

      if( (U) && (value != 0) && (result == 0) )
        {
          throw ( std::underflow_error("saSampleBase(int64_t): underflow") );
        }
      
      if ( (O) && ((value > static_cast<int64_t>(saInputMax)) ||
                   (value < static_cast<int64_t>(saInputMin))) )
        {
          throw ( std::overflow_error("saSampleBase(int64_t): overflow") );
        }

      sample = static_cast<T>(result);
      
      if (S)
        {
          if ( value > static_cast<int64_t>(saInternalMax) )
            { 
              sample = static_cast<T>(saInternalMax); 
            }
          else if ( value < static_cast<int64_t>(saInternalMin) )
            { 
              sample = static_cast<T>(saInternalMin); 
            }
        }
    }
    
    /// Cnstructor
    /// \param value uint64_t initial value
    saSampleBase(size_t value) : sample(0)
    {
      sample = static_cast<T>(value)*saInternalOne;

      if( (U) && (value != 0) && (sample == 0) )
        {
          throw ( std::underflow_error("saSampleBase(size_t): underflow") );
        }
      
      if ( (O) &&
           (static_cast<double>(value) > static_cast<double>(saInputMax)) )
        {
          throw ( std::overflow_error("saSampleBase(size_t): overflow") );
        }
      
      if (S)
        {
          if ( static_cast<double>(value) > static_cast<double>(saInternalMax) )
            { 
              sample = static_cast<T>(saInternalMax); 
            }
        }
    }

    /// Cnstructor
    /// \param value float initial value
    saSampleBase(float value) : sample(0)
    {
      sample = static_cast<T>(value*static_cast<float>(saInternalOne));

      if ( (U) && (value != 0) && (sample == 0) )
        {
          throw ( std::underflow_error("saSampleBase(float): underflow") );
        }
      
      if ( (O) && ((static_cast<double>(value) >
                    static_cast<double>(saInputMax)) || 
                   (static_cast<double>(value) <
                    static_cast<double>(saInputMin))) )
        {
          throw ( std::overflow_error("saSampleBase(float): overflow") );
        }
      
      if (S)
        {
          if ( static_cast<double>(value) >
               static_cast<double>(saInternalMax) )
            { 
              sample = static_cast<T>(saInternalMax); 
            }
          else if ( static_cast<double>(value) <
                    static_cast<double>(saInternalMin) )
            { 
              sample = static_cast<T>(saInternalMin); 
            }
        }
    }

    /// Cnstructor
    /// \param value double initial value
    saSampleBase(double value) : sample(0)
    {
      sample = static_cast<T>(value*static_cast<double>(saInternalOne));

      if ( (U) && (value != 0) && (sample == 0) )
        {
          throw ( std::underflow_error("saSampleBase(float): underflow") );
        }
      
      if ( (O) && ((value > static_cast<double>(saInputMax)) || 
                   (value < static_cast<double>(saInputMin))) )
        {
          throw ( std::overflow_error("saSampleBase(float): overflow") );
        }
      
      if (S)
        {
          if (value > static_cast<double>(saInternalMax))
            { 
              sample = static_cast<T>(saInternalMax); 
            }
          else if (value < static_cast<double>(saInternalMin)) 
            { 
              sample = static_cast<T>(saInternalMin); 
            }
        }
    }

    /// Conversion from saSampleBase
    /// \tparam NewType convert saSampleBase to NewType
    /// \return converted value
    template <typename NewType>
    NewType to() const
    {
      if ( ! std::is_signed<NewType>() )
        {
          throw ( std::logic_error("to<unsigned> not supported") );
        }
      else if (std::is_floating_point<NewType>())
        {
          // Can't specialise for floating point types
          NewType result =
            static_cast<NewType>(sample) / static_cast<NewType>(saInternalOne);
          if ( (U) && (sample != 0) && (result == 0) )
            {
              throw (std::underflow_error("to<floating_point>: underflow"));
            }
          return result;
        } 
      else
        {
          L tmp = static_cast<L>(sample) / static_cast<L>(saInternalOne);
          if( (U) && (sample != 0) && (tmp == 0) )
            {
              throw ( std::underflow_error("to: underflow"));
            }

          L NewTypeMax = saInternalMax;
          L NewTypeMin = saInternalMin;
            
          if (static_cast<double>(std::numeric_limits<NewType>::max()) <
              static_cast<double>(saInternalMax))
            {
              NewTypeMax = static_cast<L>(std::numeric_limits<NewType>::max());
              NewTypeMin = static_cast<L>(std::numeric_limits<NewType>::min());
            }
          
          if ( (O) && ((tmp > NewTypeMax) || (tmp < NewTypeMin)) )
            {
              throw ( std::overflow_error("to: overflow") );
            }
            
          if (S)
            {
              if (tmp > NewTypeMax)
                { 
                  tmp = NewTypeMax;
                }
              else if (tmp < NewTypeMin)
                { 
                  tmp = NewTypeMin;
                }
            }
          NewType result = static_cast<NewType>(tmp);
          return result;
        }
    }

    /// Conversion from saSampleBase to saDeviceType
    /// \tparam saDeviceType typename of input type
    /// \return converted value
    template <typename saDeviceType>
    saDeviceType toSink() const
    { 
      T result = sample;

      if ( (O) && ((result >= saInternalOne) || (result < -saInternalOne)) )
        {
          throw ( std::overflow_error("toSink<> overflow") );
        }
      
      if (S)
        {
          if (result >= saInternalOne) 
            { 
              result = saInternalOne - 1; 
            }
          else if (result < -saInternalOne) 
            { 
              result = -saInternalOne; 
            }
        }

      static int shift = saInternalBinaryPoint - 
        std::numeric_limits<saDeviceType>::digits;

      if (shift > 0)
        {
          result /= static_cast<T>(1<<(shift));
        }
      else if (shift < 0)
        {
          result *= static_cast<T>(1<<(-shift));
        }

      if( (U) && (shift > 0) && (sample != 0) && (result == 0) )
        {
          throw ( std::underflow_error("toSink: underflow") );
        }
      
        return static_cast<saDeviceType>(result);
    }

    /// Conversion from saDeviceType to saSampleBase
    /// \tparam saDeviceType typename of input type
    /// \param s input value
    template<typename saDeviceType> 
    void fromSource(const saDeviceType s)
    {
      
      if ( std::is_unsigned<saDeviceType>() )
        {
          throw ( std::logic_error("fromSource<unsigned> not supported") );
        }
      
      if (std::is_floating_point<saDeviceType>())
        {
          throw ( std::logic_error("fromSource<floating_point> not supported") );
        }
      
      static int shift = saInternalBinaryPoint - 
        std::numeric_limits<saDeviceType>::digits;

      L result = static_cast<L>(s) *
        static_cast<L>((shift >= 0 ? (1<<shift) : (1>>shift)));
      
      CheckResult<saDeviceType>(s, result);

      sample = static_cast<T>(result);
    }

    /// Peek at internal values
    /// \return saSampleBase internal value
    T Get() const { return sample; }

    // Operators
    saSampleBase operator*() { return *this; }

    saSampleBase* operator&() { return this; }

    saSampleBase operator-() 
    {
      L result = -static_cast<L>(sample);
      if (std::is_integral<T>())
        {
          CheckResult<T>(sample, result);
        }
      saSampleBase newSample;
      newSample.sample = static_cast<T>(result);
      return newSample;
    }

    saSampleBase& operator=(const saSampleBase& rhs) 
    {
      sample = rhs.sample;
      return *this;
    }

    saSampleBase operator+=(const saSampleBase& rhs)
    { 
      L result = static_cast<L>(sample) + static_cast<L>(rhs.sample); 
      CheckResult<bool>(false, result); // No underflow
      sample = static_cast<T>(result);
      return *this; 
    }

    saSampleBase operator-=(const saSampleBase& rhs)
    { 
      L result = static_cast<L>(sample) - static_cast<L>(rhs.sample);
      CheckResult<bool>(false, result); // No underflow
      sample = static_cast<T>(result);
      return *this;
    }

    saSampleBase operator*=(const saSampleBase& rhs)
    { 
      sample = ABonC(sample, rhs.sample, saInternalOne); 
      return *this; 
    }

    saSampleBase operator/=(const saSampleBase& rhs)
    {
      sample = ABonC(sample, saInternalOne, rhs.sample);
      return *this;
    }

    bool operator==(const saSampleBase& rhs) const
    {
      return (sample == rhs.sample);
    }

    bool operator<(const saSampleBase& rhs) const 
    {
      return (sample < rhs.sample);
    }

    bool operator!=(const saSampleBase<T,L,S,O,U>& rhs) 
    {
      return (sample != rhs.sample);
    }

    bool operator>(const saSampleBase<T,L,S,O,U>& rhs)
    {
      return (sample > rhs.sample);
    }

    bool operator>=(const saSampleBase<T,L,S,O,U>& rhs)
    {
      return (sample >= rhs.sample);
    }

    bool operator<=(const saSampleBase<T,L,S,O,U>& rhs)
    {
      return (sample <= rhs.sample);
    }

  private:
    /// Private utility functions

    /// Check the result for saturation, overflow and underflow
    /// \tparam C typename of input value
    /// \param value input value
    /// \param result result to be checked
    template <typename C>
    void CheckResult(C value, L &result)
    {
      if ( (U) && (value != 0) && (result == 0) )
        {
          throw ( std::underflow_error("saSample: underflow") );
        }
      
      if ( (O) && ((result > saInternalMax) || (result < saInternalMin)) )
        {
          throw ( std::overflow_error("saSample: overflow") );
        }
      
      if (S)
        {
          if (result > saInternalMax)
            {
              result = saInternalMax;
            }
          else if (result < saInternalMin)
            {
              result = saInternalMin;
            }
        }
    }
    
    T ABonC(T const a, T const b, T const c)
    {
      L result;
      
      if (c == 0)
        {
          throw ( std::range_error("saSample: divide by zero") );
        }
      
      result = static_cast<L>(a);
      result *= static_cast<L>(b);
      result /= static_cast<L>(c);
    
      CheckResult<bool>((a != 0) && (b != 0), result);
      
      return static_cast<T>(result);
    }

  };

  // Binary operators that need not be member functions
  template <typename T, typename L, bool S, bool O, bool U>
  saSampleBase<T,L,S,O,U> operator+(const saSampleBase<T,L,S,O,U>& lhs,
                                    const saSampleBase<T,L,S,O,U>& rhs)
  { 
    saSampleBase<T,L,S,O,U> result = lhs; 
    result += rhs; 
    return result; 
  }

  template <typename T, typename L, bool S, bool O, bool U, typename Trhs>
  saSampleBase<T,L,S,O,U> operator+(const saSampleBase<T,L,S,O,U>& lhs,
                                    const Trhs& rhs)
  { 
    saSampleBase<T,L,S,O,U> result = lhs; 
    result += rhs; 
    return result; 
  }

  template <typename T, typename L, bool S, bool O, bool U, typename Tlhs>
  saSampleBase<T,L,S,O,U> operator+(const Tlhs& lhs,
                                    const saSampleBase<T,L,S,O,U>& rhs)
  { 
    saSampleBase<T,L,S,O,U> result = lhs; 
    result += rhs; 
    return result; 
  }

  template <typename T, typename L, bool S, bool O, bool U>
  saSampleBase<T,L,S,O,U> operator-(const saSampleBase<T,L,S,O,U>& lhs,
                                    const saSampleBase<T,L,S,O,U>& rhs)
  { 
    saSampleBase<T,L,S,O,U> result = lhs; 
    result -= rhs; 
    return result; 
  }

  template <typename T, typename L, bool S, bool O, bool U, typename Trhs>
  saSampleBase<T,L,S,O,U> operator-(const saSampleBase<T,L,S,O,U>& lhs,
                                    const Trhs& rhs)
  { 
    saSampleBase<T,L,S,O,U> result = lhs; 
    result -= rhs; 
    return result; 
  }

  template <typename T, typename L, bool S, bool O, bool U, typename Tlhs>
  saSampleBase<T,L,S,O,U> operator-(const Tlhs& lhs,
                                    const saSampleBase<T,L,S,O,U>& rhs)
  { 
    saSampleBase<T,L,S,O,U> result = lhs; 
    result -= rhs; 
    return result; 
  }

  template <typename T, typename L, bool S, bool O, bool U>
  saSampleBase<T,L,S,O,U> operator*(const saSampleBase<T,L,S,O,U>& lhs,
                                    const saSampleBase<T,L,S,O,U>& rhs)
  { 
    saSampleBase<T,L,S,O,U> result = lhs; 
    result *= rhs; 
    return result; 
  }

  template <typename T, typename L, bool S, bool O, bool U, typename Trhs>
  saSampleBase<T,L,S,O,U> operator*(const saSampleBase<T,L,S,O,U>& lhs,
                                    const Trhs& rhs)
  { 
    saSampleBase<T,L,S,O,U> result = lhs; 
    result *= rhs; 
    return result; 
  }

  template <typename T, typename L, bool S, bool O, bool U, typename Tlhs>
  saSampleBase<T,L,S,O,U> operator*(const Tlhs& lhs,
                                    const saSampleBase<T,L,S,O,U>& rhs)
  { 
    saSampleBase<T,L,S,O,U> result = lhs; 
    result *= rhs; 
    return result; 
  }

  template <typename T, typename L, bool S, bool O, bool U>
  saSampleBase<T,L,S,O,U> operator/(const saSampleBase<T,L,S,O,U>& lhs,
                                    const saSampleBase<T,L,S,O,U>& rhs)
  { 
    saSampleBase<T,L,S,O,U> result = lhs; 
    result /= rhs; 
    return result; 
  }

  template <typename T, typename L, bool S, bool O, bool U, typename Trhs>
  saSampleBase<T,L,S,O,U> operator/(const saSampleBase<T,L,S,O,U>& lhs,
                                    const Trhs& rhs)
  { 
    saSampleBase<T,L,S,O,U> result = lhs; 
    result /= rhs; 
    return result; 
  }

  template <typename T, typename L, bool S, bool O, bool U, typename Tlhs>
  saSampleBase<T,L,S,O,U> operator/(const Tlhs& lhs,
                                    const saSampleBase<T,L,S,O,U>& rhs)
  { 
    saSampleBase<T,L,S,O,U> result = lhs; 
    result /= rhs; 
    return result; 
  }
}

#endif


