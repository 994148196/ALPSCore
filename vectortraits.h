/***************************************************************************
* ALPS++ library
*
* alps/vectortraits.h   A class to store parameters
*
* $Id$
*
* Copyright (C) 1999-2002 by Matthias Troyer <troyer@itp.phys.ethz.ch>,
*                            Synge Todo <wistaria@comp-phys.org>,
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
**************************************************************************/

#ifndef ALPS_VECTORTRAITS_H
#define ALPS_VECTORTRAITS_H

#include <alps/config.h>
#include <alps/typetraits.h>
#include <alps/functional.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>
#ifdef HAS_VALARRAY
# include <valarray>
#endif

namespace alps {

/*
template <class CONTAINER> 
struct VectorTraits {
  typedef typename CONTAINER::value_type value_type;
};
template <class T> 
struct VectorTraits<std::vector<T> > {
  typedef std::vector<T> CONTAINER;
  typedef typename CONTAINER::value_type value_type;
  typedef typename TypeTraits<value_type>::norm_t norm_type;
  typedef typename CONTAINER::iterator iterator;
  typedef typename CONTAINER::const_iterator const_iterator;
  typedef typename CONTAINER::size_type size_type;
  static const bool is_complex = TypeTraits<value_type>::is_complex;
};
*/

template <class CONTAINER> 
struct VectorTraits
{
  typedef typename CONTAINER::value_type value_type;
  typedef typename TypeTraits<value_type>::norm_t norm_type;
  typedef typename CONTAINER::iterator iterator;
  typedef typename CONTAINER::const_iterator const_iterator;
  typedef typename CONTAINER::size_type size_type;
  static const bool is_complex = TypeTraits<value_type>::is_complex;
};

template <class T> 
struct VectorTraits<std::valarray<T> > {
  typedef T value_type;
  typedef typename TypeTraits<value_type>::norm_t norm_type;
  typedef T* iterator; 
  typedef const T* const_iterator; 
  typedef std::size_t size_type;
  static const bool is_complex = TypeTraits<value_type>::is_complex;
};

template <class T, std::size_t sz> 
struct VectorTraits<T[sz]> {
  typedef T value_type;
  typedef typename TypeTraits<value_type>::norm_t norm_type;
  typedef T* iterator; 
  typedef const T* const_iterator; 
  typedef std::size_t size_type;
  static const bool is_complex = TypeTraits<value_type>::is_complex;
};

namespace vectorops {

// standard containers

template <class C>
inline typename VectorTraits<C>::iterator begin(C& c) { return c.begin();}

template <class C>
inline typename VectorTraits<C>::iterator end(C& c) { return c.end();}

template <class C>
inline typename VectorTraits<C>::const_iterator const_begin(const C& c) { return c.begin();}

template <class C>
inline typename VectorTraits<C>::const_iterator const_end(const C& c) { return c.end();}
  
template <class C>
inline typename VectorTraits<C>::size_type size(const C& c) { return c.size();}

template <class C>
inline typename VectorTraits<C>::value_type* data(C& c) { return &c[0];}

template <class C>
inline const typename VectorTraits<C>::value_type* data(const C& c) { return &c[0];}

template <class C>
inline typename VectorTraits<C>::norm_type infinity_norm(const C & c) 
{
  return std::accumulate(const_begin(c), const_end(c), typename VectorTraits<C>::norm_type(0.), absmax<typename VectorTraits<C>::value_type>());
}

template <class C>
inline typename VectorTraits<C>::norm_type one_norm(const C& c) 
{
  return std::accumulate(const_begin(c), const_end(c), typename VectorTraits<C>::norm_type(0.), add_abs<typename VectorTraits<C>::value_type>());
}

template <class C>
inline typename VectorTraits<C>::norm_type two_norm2(const C& c)
{
  return std::accumulate(const_begin(c), const_end(c), typename VectorTraits<C>::norm_type(0.), add_abs2<typename VectorTraits<C>::value_type>());
}

template <class C>
inline typename VectorTraits<C>::norm_type two_norm(const C& c) 
{
  return std::sqrt(two_norm2(c));
}

template <class C, class X>
inline void scale(C& c, X val) 
{
  std::transform(begin(c),end(c),begin(c), 
    bind2nd(multiplies<typename VectorTraits<C>::value_type,X,typename VectorTraits<C>::value_type>(),val));
}
  
template <class C>
inline void add(C& c1, const C& c2) 
{
  std::transform(const_begin(c1),const_end(c1),const_begin(c2),begin(c1),std::plus<typename VectorTraits<C>::value_type>());
}

template <class C>
inline void subtract(C& c1, const C& c2) 
{
  std::transform(const_begin(c1),const_end(c1),const_begin(c2),begin(c1),std::minus<typename VectorTraits<C>::value_type>());
}

template <class C, class X>
inline void add_scaled(C& c1, const C& c2, X val) 
{
  std::transform(const_begin(c1),const_end(c1),const_begin(c2),begin(c1), 
			plus_scaled<typename VectorTraits<C>::value_type,X>(val));
}

template <class C, class X>
inline void subtract_scaled(C& c1, const C& c2, X val) 
{
  std::transform(const_begin(c1),const_end(c1),const_begin(c2),begin(c1), 
			minus_scaled<typename VectorTraits<C>::value_type,X>(val));
}

template <class C>
inline typename VectorTraits<C>::norm_type normalize(C& c) 
{
  typename VectorTraits<C>::norm_type norm= two_norm(c);
  scale(c,1./norm);
  return norm;
}
  
template <class C>
inline void resize(C& c, std::size_t n) 
{
  if(c.size()!=n)
   	c.resize(n);
}

template <class C>
inline void assign(C& c, typename VectorTraits<C>::value_type x) 
{
  c.assign(c.size(),x);
}

template <class C>
inline void assign(C& c, const C& x) 
{
  c.assign(x.begin(),x.end());
}

template <class C>
inline void swap(C& c1, C& c2) 
{
  c1.swap(c2);
}

template <class C>
inline typename VectorTraits<C>::value_type scalar_product(const C& c1, const C& c2) 
{
  return std::inner_product(const_begin(c1),const_end(c1),const_begin(c2),
                               typename VectorTraits<C>::value_type(), std::plus<typename VectorTraits<C>::value_type>(),
                               conj_mult<typename VectorTraits<C>::value_type>());
}
  
template <class C>
inline void deallocate(C& c) 
{
  c = C();
}
  
  
template <class T> inline typename VectorTraits<std::valarray<T> >::iterator 
begin(std::valarray<T>& c) { return &(c[0]);}

template <class T> inline typename VectorTraits<std::valarray<T> >::iterator 
end(std::valarray<T>& c) {return begin(c)+c.size();}

template <class T> inline typename VectorTraits<std::valarray<T> >::const_iterator 
const_begin(const std::valarray<T>& c) { return &(const_cast<std::valarray<T>&>(c)[0]);}

template <class T> inline typename VectorTraits<std::valarray<T> >::const_iterator 
const_end(const std::valarray<T>& c) {return const_begin(c)+c.size();}

template <class T> 
inline T* data(std::valarray<T>& c) { return begin(c);}

template <class T> 
inline const T* data(const std::valarray<T>& c) { return const_begin(c);}

template <class T, class X>
inline void scale(std::valarray<T>& c, X val) 
{
  c*=T(val);
}
  
template <class T>
inline void add(std::valarray<T>& c1, const std::valarray<T>& c2) 
{
  c1+=c2;
}

template <class T>
inline void subtract(std::valarray<T>& c1, const std::valarray<T>& c2) 
{
  c1-=c2;
}

template <class T>
inline void swap(std::valarray<T>& c1, std::valarray<T>& c2) 
{
  for (std::size_t i=0;i<c1.size();++i)
    std::swap(c1[i],c2[i]);
}

template <class T>
inline void assign (std::valarray<T>& c1, const std::valarray<T>& c2) 
{
  c1.resize(c2.size());
  c1=c2;
}

template <class T>
inline void assign (std::valarray<T>& c1, const T& x) 
{
  c1=x;
}

template <class T>
inline void deallocate(std::valarray<T> & c) { c.resize(0);}
  
 /* 
template <class T, std::size_t sz>
inline T* begin(T[sz]& c) { return c;}

template <class T, std::size_t sz>
inline const T* const_begin(const T[sz] & c) { return c;}
 
template <class T, std::size_t sz>
inline T* end(T[sz]& c) { return c+sz;}

template <class T, std::size_t sz>
inline const T* const_end(const T[sz] & c) { return c+sz;}
 
template <class T, std::size_t sz>
inline T* data(T[sz]& c) { return c;}

template <class T, std::size_t sz>
inline const T* data(const T[sz] & c) { return c;}
  
template <class T, std::size_t sz>
inline void assign(T[sz] & c, value_type x) 
{
  std::fill(c,c+sz,x);
}
  
template <class T, std::size_t sz>
inline void assign(T[sz] & c, const container& x) 
  {
    std::copy(x,x+sz,c);
  }
  
template <class T, std::size_t sz>
inline void swap(T[sz] & c1, T[sz] & c2) 
{
  for (std::size_t i=0;i<size(c1);++i)
    std::swap(c1[i],c2[i]);
}

template <class T, std::size_t sz>
inline std::size_t size(const T[sz]& c) { return sz;}

template <class T, std::size_t sz>
inline void deallocate(T[sz]& c) {}

*/

} // namespace vectorops

} // namespace alps

#endif // ALPS_VECTORTRAITS_H
