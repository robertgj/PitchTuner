/// \file saCircBuff.h

#include <iosfwd>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <atomic>
#include <semaphore>
#include <type_traits>

#include <cassert>
#include <cstddef>
#include <cstring>

#ifndef __SACIRCBUFF_H__
#define __SACIRCBUFF_H__

namespace SimpleAudio
{

  /// Exception classes for the circular buffer

  /// \class saCircBuffOverRun
  /// Exception class for run-time buffer overrun
  class saCircBuffOverRun : public std::runtime_error
  {
  public:
    /// Buffer overrun exception handler
    /// \param s what string
    explicit saCircBuffOverRun(const std::string &s) : std::runtime_error(s) {}
  };
  
  /// \class saCircBuffUnderRun
  /// Exception class for run-time buffer underrun
  class saCircBuffUnderRun : public std::runtime_error
  {
  public:
    /// Buffer underrun exception handler
    ///\param s what string
    explicit saCircBuffUnderRun(const std::string &s) : std::runtime_error(s) {}
  };
  
  /// \class saCircBuffCantCopy
  /// Exception class for run-time logic error
  class saCircBuffCantCopy : public std::logic_error
  {
  public:
    /// Buffer logic error, cannot complete the copy
    /// \param s what string
    explicit saCircBuffCantCopy(const std::string &s) : std::logic_error(s) {}
  };

  /// \class saCircBuffDefaultConverter
  /// Template class for default sample conversion.
  /// \tparam S source type
  /// \tparam D destination type
  template <typename S, typename D>
  struct saCircBuffDefaultConverter
  { 
    /// Default conversion functor.
    /// There may be a loss of precision when copying.
    /// \param t input of type S
    /// \returns t output of type T
    D operator()(const S& t) const { return static_cast<D>(t); } 
  };

  /// \class saCircBuffCopyByteWise
  /// Template class for byte-wise copying 
  /// \tparam T source and destination type
  template<typename T>
  struct saCircBuffCopyByteWise
  {
    /// Byte-wise copy functor.
    /// There may be a loss of precision when copying. 
    /// \param dst output buffer of type T*
    /// \param src input buffer of type T*
    /// \param num number of samples to be copied
    void operator()(T* const dst, const T* const src, const std::size_t num) 
    {
      // Caller must ensure that space is available in dst
      memcpy(&(dst[0]), &(src[0]), num*sizeof(T));
    }
  };

  /// \class saCircBuffCopyImplicit
  /// Template class for copying with implicit type conversion
  /// \tparam S source type
  /// \tparam D destination type
  template<typename S, typename D>
  struct saCircBuffCopyImplicit
  {
    /// Implicit copy functor.
    /// Assumes that the compiler handles conversion.
    /// There may be a loss of precision when copying. 
    /// \param dst output buffer of type D*
    /// \param src input buffer of type S*
    /// \param num number of samples of type S to be copied
    void operator()(D* const dst, const S* const src, const std::size_t num) 
    { 
      //Caller must ensure that space is available in dst
      std::copy(src, src+num, dst);
    }
  };

  /// \class saCircBuffCopyExplicit
  /// Template class for copying with explicit type conversion
  /// \tparam S source type
  /// \tparam D destination type (default S)
  /// \tparam C conversion functor
  template<typename S, 
           typename D=S, 
           typename C=saCircBuffDefaultConverter<S,D>>
  struct saCircBuffCopyExplicit
  {
    /// Explicit copy functor.
    /// Uses the template conversion functor.
    /// There may be a loss of precision when copying. 
    /// \param dst output buffer of type D*
    /// \param src input buffer of type S*
    /// \param num number of samples of type S copied
    void operator()(D* const dst, const S* const src, const std::size_t num)
    {
      // Caller must ensure that space is available in dst
      std::transform(src, src+num, dst, C());
    }
  };

  /// \class saCircBuff
  /// \brief SimpleAudio circular buffer class
  ///
  /// Implementation of a lock-free, thread-safe circular buffer.
  ///   - \c m_back is an atomic variable that indicates the next availabe
  ///     free sample storage location in the circular buffer and is only
  ///     modified by \c Insert() after the copy is complete. The internal size
  ///     of the buffer is set to one greater than that requested so that the
  ///     storage at \c m_back is always empty.
  ///   - \c m_front is an atomic variable that indicates the start of sample
  ///     storage in the circular buffer and is only modified by \c Extract()
  ///     after the copy is complete.
  ///   - \c Insert() reads \c m_front once before calculating the space
  ///     available for writing.
  ///   - \c Extract() reads \c m_back once before calculating the number of
  ///     samples available for reading.
  ///   - the samples available for reading from the circular buffer lie between
  ///     \c m_front and \c m_back. If \c m_back < \c m_front then the sample
  ///     storage wraps around the end of the buffer memory back to the start.
  ///   - if \c m_front == \c m_back then the buffer is empty.
  ///   - \c Clear() sets both \c m_front and \c m_back to 0. You should
  ///     \e not call \c Clear() from a producer or consumer thread!
  ///
  /// See: "Writing Lock-Free Code: A Corrected Queue", H. Sutter,
  /// Dr Dobb's Journal, September 29, 2008.
  ///
  /// \tparam StoredType
  /// \tparam InputType (defaults to \c StoredType)
  /// \tparam OutputType (defaults to \c StoredType)
  /// \tparam IC input copy functor from \c InputType to \c StoredType
  /// \tparam OC output copy functor from \c StoredType to \c OutputType
  /// \tparam DoOverRunException allow overrun exceptions (default is true)
  /// \tparam DoUnderRunException allow underrun exceptions (default is true)
  template <typename StoredType,
            typename InputType=StoredType,
            typename OutputType=StoredType,
            typename IC=saCircBuffCopyByteWise<StoredType>,
            typename OC=IC,
            bool DoOverRunException = true,
            bool DoUnderRunException = true> 
  class saCircBuff
  {
  public:

    /// Constructor
    /// \param numSamples capacity of the circular buffer
    saCircBuff(std::size_t numSamples) : 
      m_array(numSamples+1, 0),
      m_back(0),
      m_front(0)
    {
      // Make this a compile-time error!
      bool noCopy = std::is_trivially_copyable<StoredType>();
      if ( !noCopy )
        {
          throw ( saCircBuffCantCopy("no bit-wise copy") );
        }
    }

    /// Destructor
    ~saCircBuff() throw() { }

    /// Maximum number of samples in the circular buffer.
    /// \return the maximum number of samples in the circular buffer
    std::size_t Capacity() const { return m_array.size() - 1; }

    /// Clear the circular buffer.
    /// You should \e not call this function from a producer or consumer thread!
    void Clear() 
    { 
      m_back = m_front = 0; 
    }

    /// Insert samples into the circular buffer.
    /// Two separate copies may be required. 
    /// 
    /// \param num_to_write number of samples to copy
    /// \param buf buffer to copy from
    /// \return number of samples actually copied
    std::size_t Insert( const std::size_t num_to_write,
                        const InputType * const buf = nullptr)
    {
      // Sanity checks
      if (buf == nullptr)
        {
          if (DoOverRunException)
            {
              throw ( saCircBuffOverRun("Insert() from nullptr") );
            }
          else
            {
              return 0;
            }
        }

      if (m_array.size() == 1)
        {
          if (DoOverRunException)
            {
              throw ( saCircBuffOverRun("Insert() into zero-length buffer") );
            }
          else
            {
              return 0;
            }
        }

      // Find the actual the space available in the circular buffer
      std::size_t first = m_front;
      std::size_t space_available_to_write, num_written;
      if (first <= m_back)
        {
          space_available_to_write = first + (m_array.size() - 1) - m_back;
        }
      else
        {
          space_available_to_write = first - m_back - 1;
        }

      // Sanity check (prevents g++ static analyser warning!)
      if (space_available_to_write >= m_array.size())
        {
          throw ( saCircBuffCantCopy("Insert() space_available_to_write") );
        }

      // Set number of samples actually copied
      if (num_to_write > space_available_to_write)
        {
          if (DoOverRunException)
            {
              throw ( saCircBuffOverRun("Insert() overrun") );
            }
          else
            {
              num_written = space_available_to_write;
            }
        }
      else
        {
          num_written = num_to_write;
        }

      if (num_written == 0)
        {
          return 0;
        }

      // Do the copy. Split it if needed. Update m_back when done.
      if ( (m_back + num_written) > m_array.size())
        {
          std::size_t first_write = m_array.size() - m_back;
          if (first_write > 0)
            {
              InputCopy(&(m_array[m_back]), buf, first_write);
            }
          std::size_t second_write = num_written - first_write;
          if (second_write > 0)
            {
              InputCopy(&(m_array[0]), &(buf[first_write]), second_write);
            }
          m_back = second_write;
        }
      else
        {
          InputCopy(&(m_array[m_back]), buf, num_written);
          std::size_t last = m_back + num_written;
          if (last == m_array.size())
            {
              last = 0;
            }
          m_back = last;
        }

      // Done
      return num_written;
    }

    /// Extract samples from the circular buffer.
    /// Two separate copies may be required. 
    /// 
    /// \param num_to_read number of samples to copy
    /// \param buf buffer to copy into
    /// \return number of samples actually copied
    std::size_t Extract(OutputType * const buf = nullptr,
                        const std::size_t num_to_read = 1)
    {
      // Sanity checks
      if (buf == nullptr)
        {
          if (DoUnderRunException)
            {
              throw ( saCircBuffUnderRun("Extract() from nullptr") );
            }
          else
            {
              return 0;
            }
        }

      if (m_array.size() == 1)
        {
          if (DoUnderRunException)
            {
              throw ( saCircBuffUnderRun("Extract() from zero-length buffer") );
            }
          else
            {
              return 0;
            }
        }
      
      // Find the actual the number of samples available in the circular buffer
      std::size_t last = m_back;
      std::size_t samples_available_to_read, num_read;
      if (m_front <= last)
        {
          samples_available_to_read = last - m_front;
        }
      else
        {
          samples_available_to_read = last + m_array.size() - m_front;
        }

      // Sanity check (prevents g++ static analyser warning!)
      if (samples_available_to_read >= m_array.size())
        {
          throw( saCircBuffCantCopy("Extract() samples_available_to_read") );
        }
      
      // Set number of samples actually copied
      if (num_to_read > samples_available_to_read)
        {
          if (DoUnderRunException)
            {
              throw ( saCircBuffUnderRun("Extract() underrun") );
            }
          else
            {
              num_read = samples_available_to_read;
            }
        }
      else
        {
          num_read = num_to_read;
        }

      if (num_read == 0)
        {
          return 0;
        }

      // Do the copy. Split it if needed. Update m_front when done.
      if ((m_front + num_read) > m_array.size())
        {
          std::size_t first_read = m_array.size() - m_front;
          if (first_read > 0)
            {
              OutputCopy(&(buf[0]), &(m_array[m_front]), first_read);
            }
          std::size_t second_read = num_read - first_read;
          if (second_read > 0)
            {
              OutputCopy(&(buf[first_read]), &(m_array[0]), second_read);
            }
          m_front = second_read;
        }
      else
        {
          OutputCopy(&(buf[0]), &(m_array[m_front]), num_read);
          std::size_t first = m_front + num_read;
          if (first == m_array.size())
            {
              first = 0;
            }
          m_front = first;
        }
    
      // Done
      return num_read;
    }

  private:
    /// \privatesection

    std::vector<StoredType> m_array;
    ///< circular buffer storage
    
    std::atomic<std::size_t> m_back;
    ///< m_back is to the next available storage location, owned by Insert()
    ///< Other than within Insert(), m_array[m_back] never contains a valid 
    ///< sample
    
    std::atomic<std::size_t> m_front;
    ///< m_front is the oldest value in the circular buffer, owned by Extract()
    
    IC InputCopy;
    ///< Input copy functor for \c InputType to \c StoredType

    OC OutputCopy;
    ///< Output copy functor for \c StoredType to \c OutputType
  }; 

} // namespace SimpleAudio

#endif
