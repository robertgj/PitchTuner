/// \file Filter.h
/// \brief Templated functional that supports STL filter operations

#ifndef __FILTER_H__
#define __FILTER_H__

/// \class Filter
/// \brief A virtual function object for a filter
/// \tparam T the type of the input and output
template <typename T>
class Filter
{
public:
  Filter() {} ;
  virtual ~Filter() throw() { }
  virtual T operator() (T& u) = 0;
};

/// \class updateFilter
/// \brief A function object intended to avoid copying filters in STL operations
/// \tparam T the type of the input and output
template <typename T>
class updateFilter
{
public:
  updateFilter ( Filter<T>* _f ) : f(_f) { }

  updateFilter ( const updateFilter& _filter ) : f(_filter.f) {}

  /// Perform the filter update
  /// \param u the input
  /// \return the filter section output value
  T operator() (T& u) 
  {
    return (*f)(u);
  }

private:
  /// Pointer to the filter object
  Filter<T>* f;
};

#endif
