/*****************************************************************************
*
* ALPS Project: Algorithms and Libraries for Physics Simulations
*
* ALPS Libraries
*
* Copyright (C) 1994-2004 by Matthias Troyer <troyer@itp.phys.ethz.ch>,
*                            Beat Ammon <ammon@ginnan.issp.u-tokyo.ac.jp>,
*                            Andreas Laeuchli <laeuchli@itp.phys.ethz.ch>,
*                            Synge Todo <wistaria@comp-phys.org>
*
* This software is part of the ALPS libraries, published under the ALPS
* Library License; you can use, redistribute it and/or modify it under
* the terms of the license, either version 1 or (at your option) any later
* version.
* 
* You should have received a copy of the ALPS Library License along with
* the ALPS Libraries; see the file LICENSE.txt. If not, the license is also
* available from http://alps.comp-phys.org/.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT 
* SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE 
* FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE, 
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
* DEALINGS IN THE SOFTWARE.
*
*****************************************************************************/

/* $Id$ */

#ifndef ALPS_ALEA_SIMPLEOBSERVABLE_H
#define ALPS_ALEA_SIMPLEOBSERVABLE_H

#include <alps/config.h>
#include <alps/alea/observable.h>
#include <alps/xml.h>

#ifdef ALPS_HAVE_HDF5
# include <H5Cpp.h>
using namespace H5;
#endif

namespace alps {

//=======================================================================
// AbstractSimpleObservable
//
// Observable class interface
//-----------------------------------------------------------------------

template <class T> class SimpleObservableEvaluator;

template <class T>
class AbstractSimpleObservable: public Observable
{
public:
  //@{
  //@name Type members
        
  /// the data type of the observable
  typedef T value_type;

  /// the data type of averages and errors
  typedef typename obs_value_traits<T>::result_type result_type;
    
  typedef typename obs_value_traits<result_type>::slice_iterator slice_iterator;
  /// the count data type: an integral type
  typedef std::size_t count_type;

  /// the data type for autocorrelation times
  typedef typename obs_value_traits<T>::time_type time_type;
  //@}
  
  AbstractSimpleObservable(const std::string& name="") : Observable(name) {}
  virtual ~AbstractSimpleObservable() {}
  //@{
  //@name Properties of the observable
                  
  /// the number of measurements
  virtual count_type count() const =0;
  
  /// the mean value
  virtual result_type mean() const =0;
  
  /// the variance
  virtual result_type variance() const { return result_type();}
  
  /// the error
  virtual result_type error() const =0;

  /// is information about the minimum and maximum value available?
  
  virtual bool has_minmax() const { return false;}
  
  /// the minimum value
  virtual value_type min() const { return value_type();}
  
  /// the maximum value
  virtual value_type max() const { return value_type();}
  
  /// is autocorrelation information available ? 
  virtual bool has_tau() const { return false;}
    
  /// the autocorrelation time, throws an exception if not available
  virtual time_type tau() const { return time_type();}

  /// is variance  available ? 
  virtual bool has_variance() const { return false;}

  //@}


  // virtual void set_thermalization(uint32_t todiscard) = 0;
  // virtual uint32_t get_thermalization() = 0;
  
  //@{
  //@name binning information
  /// the number of bins
  virtual count_type bin_number() const { return 0;}
  /// the number of measurements per bin
  virtual count_type bin_size() const { return 0;}
  /// the value of a bin
  virtual const value_type& bin_value(count_type) const 
  { boost::throw_exception(std::logic_error("bin_value called but no bins present")); return *(new value_type());}
  /// the number of bins with squared values
  virtual count_type bin_number2() const { return 0;}
  /// the squared value of a bin
  virtual const value_type& bin_value2(count_type) const 
  { boost::throw_exception(std::logic_error("bin_value2 called but no bins present")); return *(new value_type());}

  //@}
  
  
  //@{ 
  //@name Slicing of observables
          
  /** slice the data type using a single argument.
      This can easily be extended when needed to more data types. 
      @param s the slice
      @param newname optionally a new name for the slice. Default is the
                     same name as the original observable
      */
       
  template <class S>
  SimpleObservableEvaluator<typename obs_value_slice<T,S>::value_type>
    slice(S s, const std::string& newname="") const;

  template <class S>
  SimpleObservableEvaluator<typename obs_value_slice<T,S>::value_type>
    operator[](S s) const { return slice(s);}
  //@}
#ifndef ALPS_WITHOUT_OSIRIS
  void extract_timeseries(ODump& dump) const;
#endif

  void write_xml(oxstream&, const boost::filesystem::path& = boost::filesystem::path()) const;
  void write_xml_scalar(oxstream&, const boost::filesystem::path&) const;
  void write_xml_vector(oxstream&, const boost::filesystem::path&) const;

  virtual std::string evaluation_method(Target) const { return "";}

private:
  virtual void write_more_xml(oxstream&, slice_iterator = slice_iterator()) const {}
};

//=======================================================================
// RecordableObservable
//
// an observable that can store new measurements
//-----------------------------------------------------------------------

template <class T=double, class SIGN=int32_t>
class RecordableObservable
{
public:
  typedef T value_type;
  typedef SIGN sign_type;

  /// just a default constructor  
  RecordableObservable() {}

  /// add another measurement to the observable
  virtual void operator<<(const value_type& x) =0;
  /// add another measurement to the observable
  virtual void add(const value_type& x) { operator<<(x);}
  /// add an explcitly signed measurement to the observable
  virtual void add(const value_type& x, sign_type s) { 
    if (s==1)
      add(x);
    else
      boost::throw_exception(std::logic_error("Called add of unsigned dobservable with a sign that is not 1"));
  }
 
  /// is it thermalized ? 
  virtual bool is_thermalized() const =0;
 
 };

//=======================================================================
// SimpleObservable
//
// Observable class with variable autocorrelation analysis and binning
//-----------------------------------------------------------------------

template <class T,class BINNING>
class SimpleObservable: public AbstractSimpleObservable<T>, public RecordableObservable<T>
{
public:
  typedef typename AbstractSimpleObservable<T>::value_type value_type;
  typedef typename AbstractSimpleObservable<T>::time_type time_type;
  typedef typename AbstractSimpleObservable<T>::count_type count_type;
  typedef typename AbstractSimpleObservable<T>::result_type result_type;
  typedef typename AbstractSimpleObservable<T>::slice_iterator slice_iterator;
  typedef BINNING binning_type;

  BOOST_STATIC_CONSTANT(int,version=(obs_value_traits<T>::magic_id+ (binning_type::magic_id << 16)));
  /// the constructor needs a name and optionally specifications for the binning strategy
  SimpleObservable(const std::string& name,const binning_type& b)
   : AbstractSimpleObservable<T>(name), b_(b) {}

  SimpleObservable(const std::string& name="" ,uint32_t s=0)
   : AbstractSimpleObservable<T>(name), b_(s) {}
   
  uint32_t version_id() const { return version;}
  
  Observable* clone() const {return new SimpleObservable<T,BINNING>(*this);}

  ALPS_DUMMY_VOID output(std::ostream&) const;

  ALPS_DUMMY_VOID reset(bool forthermalization=false) 
  {
    b_.reset(forthermalization);
    ALPS_RETURN_VOID
  }

  result_type mean() const {return b_.mean();}
  bool has_variance() const { return b_.has_variance();}
  result_type variance() const {return b_.variance();}
  result_type error() const {return b_.error();}
  count_type count() const {return b_.count();}
  bool has_minmax() const { return b_.has_minmax();}
  value_type min() const {return b_.min();}
  value_type max() const {return b_.max();}
  bool has_tau() const { return b_.has_tau;}
  time_type tau() const  { return b_.tau();}
  
  virtual bool is_thermalized() const { return b_.is_thermalized();}
  uint32_t get_thermalization() const { return b_.get_thermalization();}
  bool can_set_thermalization() const { return b_.can_set_thermalization();}
  
  void operator<<(const T& x) { b_ << x;}
 

  //@{
  //@name Additional binning member functions
 
  count_type bin_size() const { return b_.bin_size();}
  /// resize bins to contain at least the given number of entries 
  void set_bin_size(count_type s) {b_.set_bin_size(s);}
  
  count_type bin_number() const { return b_.filled_bin_number();}
  count_type bin_number2() const { return b_.filled_bin_number2();}
  /// get the maximum number of bins
  count_type max_bin_number() const { return b_.max_bin_number();}
  /** set the maximum number of bins
      This will be the maximum number from now on if additional measurements are performed.
   */
  void set_bin_number(count_type n) {b_.set_bin_number(n);}
  const value_type& bin_value(count_type n) const {return b_.bin_value(n);}
  const value_type& bin_value2(count_type n) const {return b_.bin_value2(n);}
  
  //@}
    
#ifndef ALPS_WITHOUT_OSIRIS
  virtual void save(ODump& dump) const;
  virtual void load(IDump& dump);
  void extract_timeseries(ODump& dump) const { b_.extract_timeseries(dump);}
#endif

  ALPS_DUMMY_VOID compact() { b_.compact(); ALPS_RETURN_VOID }
  virtual std::string evaluation_method(Target t) const 
  { return (t==Mean || t== Variance) ? std::string("simple") : b_.evaluation_method();}

private:
  Observable* convert_mergeable() const;
  void write_more_xml(oxstream& oxs, slice_iterator it) const;
  binning_type b_;
};

#ifndef BOOST_NO_INCLASS_MEMBER_INITIALIZATION
template <class T, class BINNING> const int SimpleObservable<T,BINNING>::version;
#endif


//=======================================================================
// AbstractBinning
//-----------------------------------------------------------------------

template <class T>
class AbstractBinning
{
 public: 
  typedef T value_type;
  typedef typename obs_value_traits<T>::time_type time_type;

  AbstractBinning(uint32_t=0) { }

  time_type tau()                  const { boost::throw_exception(std::logic_error("Called non-implemented function of AbstractBinning")); return time_type(); }
  uint32_t max_bin_number()        const { return 0; }
  uint32_t bin_number()            const { return 0; }
  uint32_t filled_bin_number()     const { return 0; }
  uint32_t filled_bin_number2()     const { return 0; }
  uint32_t bin_size()              const { return 0; }
  const value_type& bin_value(uint32_t  ) const {
    boost::throw_exception(std::logic_error("Binning is not supported for this observable"));
    return *(new value_type); // dummy return
  } 
  const value_type& bin_value2(uint32_t  ) const {
    boost::throw_exception(std::logic_error("Binning is not supported for this observable"));
    return *(new value_type); // dummy return
  } 
#ifndef ALPS_WITHOUT_OSIRIS
  void extract_timeseries(ODump& dump) const { dump << 0 << 0 << 0;}
#endif

  bool can_set_thermalization() const { return false;}
  bool has_minmax() const { return true;} // for now
  bool has_variance() const { return true;} // for now
  bool is_thermalized() const { return thermalized_;}
  void reset (bool for_thermal) 
  {
    thermalized_=for_thermal;
  }
  void compact() {}
  void write_scalar_xml(oxstream&) const {}
  template <class IT> void write_vector_xml(oxstream&, IT) const {}

#ifndef ALPS_WITHOUT_OSIRIS
  virtual void save(ODump& dump) const;
  virtual void load(IDump& dump);
#endif
  std::string evaluation_method() const { return "simple";}
private:
  bool thermalized_;  
};

//=======================================================================
// Implementations
//=======================================================================

template <bool ARRAY_VALUED>
struct output_helper
{
  template <class X> static void output(const X& b, std::ostream& out)
  {
    b.output_scalar(out);
  }

  template <class X> static void write_xml(const X& b, oxstream& oxs, const boost::filesystem::path& fn_hdf5)
  {
    b.write_xml_scalar(oxs, fn_hdf5);
  }
  template <class X, class IT> static void write_more_xml(const X& b, oxstream& oxs, IT)
  {
    b.write_scalar_xml(oxs);
  }
};

template <>
struct output_helper<true>
{
  template <class T> static void output(const T& b, std::ostream& out)
  {
    b.output_vector(out);
  }

  template <class T> static void write_xml(const T& b, oxstream& oxs, const boost::filesystem::path& fn_hdf5)
  {
    b.write_xml_vector(oxs, fn_hdf5);
  }
  template <class X, class IT> static void write_more_xml(const X& b, oxstream& oxs, IT i)
  {
    b.write_vector_xml(oxs, i);
  }
};

} // end namespace alps

#include <alps/alea/simpleobseval.h>

namespace alps {

template <class T, class BINNING>
inline Observable* SimpleObservable<T,BINNING>::convert_mergeable() const
{
  return new SimpleObservableEvaluator<T>(*this);
}


template <class T,class BINNING>
ALPS_DUMMY_VOID
SimpleObservable<T,BINNING>::output(std::ostream& o) const 
{ 
  if(count()==0)
  {
    if(get_thermalization()>0)
    o << name() << " " << get_thermalization() << " thermalization steps, no measurements.\n";
  }
  else 
  {
    o << name ();
    output_helper<obs_value_traits<T>::array_valued>::output(b_,o);
  }
  ALPS_RETURN_VOID
}

template <class T,class BINNING>
void SimpleObservable<T,BINNING>::write_more_xml(oxstream& oxs, slice_iterator it) const 
{ 
  output_helper<obs_value_traits<T>::array_valued>::write_more_xml(b_, oxs, it);
}

#ifndef ALPS_WITHOUT_OSIRIS

template <class T,class BINNING>
inline void SimpleObservable<T,BINNING>::save(ODump& dump) const
{
  Observable::save(dump);
  dump << b_;
}

template <class T,class BINNING>
inline void SimpleObservable<T,BINNING>::load(IDump& dump) 
{
  Observable::load(dump);
  dump >> b_;
}

template <class T>
inline void AbstractBinning<T>::save(ODump& dump) const
{
  dump << thermalized_;
}

template <class T>
inline void AbstractBinning<T>::load(IDump& dump) 
{
  dump >> thermalized_;
}

#endif

template <class T> template <class S>
inline SimpleObservableEvaluator<typename obs_value_slice<T,S>::value_type> 
AbstractSimpleObservable<T>::slice (S s, const std::string& n) const
{
  if (dynamic_cast<const SimpleObservableEvaluator<T>*>(this)!=0)
    return dynamic_cast<const SimpleObservableEvaluator<T>*>(this)->slice(s,n);
  else
    return SimpleObservableEvaluator<T>(*this).slice(s,n);        
}

template <class T>
void AbstractSimpleObservable<T>::write_xml(oxstream& oxs, const boost::filesystem::path& fn_hdf5) const
{
  output_helper<obs_value_traits<T>::array_valued>::write_xml(*this, oxs, fn_hdf5);
}

#ifdef ALPS_HAVE_HDF5

template<class T>
struct HDF5Traits 
{
  static PredType pred_type() 
  {
    boost::throw_exception(runtime_error("HDF5Traits not implemented for this type"));
    return PredType::NATIVE_INT; // dummy return
  }
};

template<>
struct HDF5Traits<double> 
{
  static PredType pred_type() { return PredType::NATIVE_DOUBLE; } 
};

template<>
struct HDF5Traits<int>    
{
  static PredType pred_type() { return PredType::NATIVE_INT; } 
};

#ifdef ALPS_HAVE_VALARRAY
template<class T>
struct HDF5Traits<std::valarray<T> > 
{
  static PredType pred_type() { return HDF5Traits<T>::pred_type(); } 
};
#endif // ALPS_HAVE_VALARRAY

#endif // ALPS_HAVE_HDF5


template <class T>
#ifdef ALPS_HAVE_HDF5
void AbstractSimpleObservable<T>::write_xml_scalar(oxstream& oxs, const boost::filesystem::path& fn_hdf5) const
#else
void AbstractSimpleObservable<T>::write_xml_scalar(oxstream& oxs, const boost::filesystem::path&) const
#endif
{
  if (count())
  {
    std::string mm = evaluation_method(Mean);
    std::string em = evaluation_method(Error);
    std::string vm = evaluation_method(Variance);
    std::string tm = evaluation_method(Tau);

    oxs << start_tag("SCALAR_AVERAGE") << attribute("name", name());
    if (is_signed())
      oxs << attribute("signed","true");     

    oxs << start_tag("COUNT") << no_linebreak << count() << end_tag;

    oxs << start_tag("MEAN") << no_linebreak;
    if (mm != "") oxs << attribute("method", mm);
    int prec=int(4-std::log10(std::abs(error()/mean())));
    prec = (prec>=3 && prec<20 ? prec : 8);
    oxs << precision(mean(),prec) << end_tag;

    oxs << start_tag("ERROR") << no_linebreak;
    if (em != "") oxs << attribute("method", em);
    oxs << precision(error(), 3) << end_tag;

    if (has_variance()) {
      oxs << start_tag("VARIANCE") << no_linebreak;
      if (vm != "") oxs << attribute("method", vm);
      oxs << precision(variance(), 3) << end_tag;
    }
    if (has_tau()) {
      oxs << start_tag("AUTOCORR") << no_linebreak;
      if (tm != "") oxs << attribute("method", tm);
      oxs << precision(tau(), 3) << end_tag;
    }

#ifdef ALPS_HAVE_HDF5
    if (!fn_hdf5.empty() && bin_size() == 1) {
      //write tag for timeseries and the hdf5-file
      oxs << start_tag("TIMESERIES") << attribute("format", "HDF5")
          << attribute("file", fn_hdf5.leaf()) << attribute("set", name())
          << end_tag;

      //open the hdf5 file and write data
      H5File hdf5(fn_hdf5.native_file_string().c_str(),H5F_ACC_CREAT | H5F_ACC_RDWR);
      hsize_t dims[1];
      dims[0]=bin_number();
      DataSpace dataspace(1,dims);
      IntType datatype(HDF5Traits<T>::pred_type());
      DataSet dataset=hdf5.createDataSet(name().c_str(),datatype,dataspace);
      vector<T> data(bin_number());
      for(int j=0;j<bin_number();j++) data[j]=bin_value(j);
      dataset.write(&(data[0]),HDF5Traits<T>::pred_type());
    }
#endif
    write_more_xml(oxs);
    oxs << end_tag("SCALAR_AVERAGE");
  }
}

template <class T>
#ifdef ALPS_HAVE_HDF5
void AbstractSimpleObservable<T>::write_xml_vector(oxstream& oxs, const boost::filesystem::path& fn_hdf5) const
#else
void AbstractSimpleObservable<T>::write_xml_vector(oxstream& oxs, const boost::filesystem::path&) const
#endif
{
  if(count())
  {
    std::string mm = evaluation_method(Mean);
    std::string em = evaluation_method(Error);
    std::string vm = evaluation_method(Variance);
    std::string tm = evaluation_method(Tau);
    result_type mean_(mean());
    result_type error_(error());
    result_type variance_;
    result_type tau_;
    if(has_tau())
    {
      obs_value_traits<T>::resize_same_as(tau_,mean_);
      obs_value_traits<T>::copy(tau_,tau());
    }
    if(has_variance())
    {
      obs_value_traits<T>::resize_same_as(variance_,mean_);
      obs_value_traits<T>::copy(variance_,variance());
    }

    oxs << start_tag("VECTOR_AVERAGE")<< attribute("name", name())
        << attribute("nvalues", obs_value_traits<T>::size(mean()));
    if (is_signed())
      oxs << attribute("signed","true");     

    typename obs_value_traits<result_type>::slice_iterator it=obs_value_traits<result_type>::slice_begin(mean_);
    typename obs_value_traits<result_type>::slice_iterator end=obs_value_traits<result_type>::slice_end(mean_);
    while (it!=end)
    {
      oxs << start_tag("SCALAR_AVERAGE")
          << attribute("indexvalue", obs_value_traits<result_type>::slice_name(mean_,it));
      oxs << start_tag("COUNT") << no_linebreak << count() << end_tag;
      int prec=(count()==1) ? 20 : int(4-std::log10(std::abs(obs_value_traits<result_type>::slice_value(error_,it)/obs_value_traits<result_type>::slice_value(mean_,it)))); 
      
      oxs << start_tag("MEAN") << no_linebreak;
      if (mm != "") oxs << attribute("method", mm);
      oxs << precision(obs_value_traits<result_type>::slice_value(mean_, it), prec)
          << end_tag;
      
      oxs << start_tag("ERROR") << no_linebreak;
      if (em != "") oxs << attribute("method", em);
      oxs << precision(obs_value_traits<result_type>::slice_value(error_, it), 3)
          << end_tag;
      
      if (has_variance()) {
        oxs << start_tag("VARIANCE") << no_linebreak;
        if (vm != "") oxs << attribute("method", vm);
        oxs << precision(obs_value_traits<result_type>::slice_value(variance_, it), 3)
            << end_tag;
      }
      if (has_tau()) {
        oxs << start_tag("AUTOCORR") << no_linebreak;
        if (tm != "") oxs << attribute("method", tm);
        oxs << precision(obs_value_traits<time_type>::slice_value(tau_, it), 3)
            << end_tag;
      }
      
#ifdef ALPS_HAVE_HDF5
      if(!fn_hdf5.empty() && bin_size() == 1) {
        //write tag for timeseries and the hdf5-file
        oxs << start_tag("TIMESERIES") << attribute("format", "HDF5")
            << attribute("file", fn_hdf5.leaf()) << attribute("set", name())
            << end_tag;

        //open the hdf5 file and write data
        H5File hdf5(fn_hdf5.native_file_string().c_str(),H5F_ACC_CREAT | H5F_ACC_RDWR);
        hsize_t dims[1];
        dims[0]=bin_number();
        DataSpace dataspace(1,dims);
        IntType datatype(HDF5Traits<T>::pred_type());
        DataSet dataset=hdf5.createDataSet(name().c_str(),datatype,dataspace);
        vector<T> data(bin_number());
        for(int j=0;j<bin_number();j++) data[j]=bin_value(j)[it];
        dataset.write(&(data[0]),HDF5Traits<T>::pred_type());
      }
#endif
      write_more_xml(oxs,it);

      ++it;
      oxs << end_tag("SCALAR_AVERAGE");
    }
    oxs << end_tag("VECTOR_AVERAGE");
  }
}

} // end namespace alps


#ifndef ALPS_WITHOUT_OSIRIS

#ifndef BOOST_NO_OPERATORS_IN_NAMESPACE
namespace alps {
#endif

template<class T>
inline alps::ODump& operator<<(alps::ODump& od, const alps::AbstractBinning<T>& m)
{ m.save(od); return od; }

template<class T>
inline alps::IDump& operator>>(alps::IDump& id, alps::AbstractBinning<T>& m)
{ m.load(id); return id; }

#ifndef BOOST_NO_OPERATORS_IN_NAMESPACE
} // namespace alps
#endif

#endif // !ALPS_WITHOUT_OSIRIS

#endif // ALPS_ALEA_SIMPLEOBSERVABLE_H
