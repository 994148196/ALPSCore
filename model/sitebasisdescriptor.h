/***************************************************************************
* ALPS++/model library
*
* model/sitebasisdescriptor.h    a class describing site basis states
*
* $Id$
*
* Copyright (C) 2003-2003 by Matthias Troyer <troyer@comp-phys.org>,
*                            Axel Grzesik <axel@th.physik.uni-bonn.de>
*
* This software is part of the ALPS library, published under the 
* ALPS Library License; you can use, redistribute it and/or modify 
* it under the terms of the License, either version 1 or (at your option) 
* any later version.
*
* You should have received a copy of the ALPS Library License along with 
* the ALPS Library; see the file License.txt. If not, the license is also 
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
**************************************************************************/

#ifndef ALPS_MODEL_SITEBASISDESCRIPTOR_H
#define ALPS_MODEL_SITEBASISDESCRIPTOR_H

#include <alps/model/quantumnumber.h>
#include <cstddef>
#include <stack>
#include <utility>
#include <vector>

#include <iostream>

namespace alps {

template<class I>
class SiteBasisDescriptor : public std::vector<QuantumNumber<I> >
{
public:
  typedef typename std::vector<QuantumNumber<I> >::const_iterator const_iterator;
  
  SiteBasisDescriptor() : num_states_(0) { }
  SiteBasisDescriptor(const XMLTag&, std::istream&);
  void write_xml(oxstream&) const;

  const std::string& name() const { return name_;}
  bool valid(const std::vector<half_integer<I> >&) const;
  std::size_t num_states() const { if (!valid_ && !evaluate()) boost::throw_exception(std::runtime_error("Cannot evaluate quantum numbers in site basis " +name()));  return num_states_;}
  bool set_parameters(const Parameters&);
  const Parameters& get_parameters() const { return parms_;}
private:
  mutable bool valid_;
  bool evaluate() const;
  Parameters parms_;
  std::string name_;
  mutable std::size_t num_states_;
  void init_dependencies() const;
};

// ------------------------------- implementation ----------------------------------

template <class I>
bool SiteBasisDescriptor<I>::valid(const std::vector<half_integer<I> >& x) const
{
  if(!valid_ && !evaluate()) 
    boost::throw_exception(std::runtime_error("Cannot evaluate quantum numbers in site basis " +name()));
  if (size() != x.size())
    return false;
  for (int i=0;i<size();++i)
    if (!(*this)[i].valid(x[i]))
      return false;
  return true;
}

template <class I>
bool SiteBasisDescriptor<I>::set_parameters(const Parameters& p)
{ 
  for (Parameters::iterator it=parms_.begin();it!=parms_.end();++it)
    if (p.defined(it->key())) 
      it->value() = p[it->key()];
  evaluate();
  return valid_;
}

template <class I>
bool SiteBasisDescriptor<I>::evaluate() const
{
  valid_=true;
  Parameters q_parms_=get_parameters();
  for (const_iterator it=begin();it!=end();++it) {
    valid_ = valid_ && const_cast<QuantumNumber<I>&>(*it).set_parameters(q_parms_);
    if(!valid_) break;
    q_parms_[it->name()]=it->min();
  }
  if (valid_ && begin()!=end()) {
    num_states_=1;
    const_iterator rit=end()-1;
    while(const_cast<QuantumNumber<I>&>(*rit).set_parameters(parms_)) {
      if(rit->levels()==half_integer<I>::max()) {
        num_states_=std::numeric_limits<I>::max();
        return true;
      }
      num_states_ *= rit->levels();
      if(rit==begin()) break;
      --rit;
    }
    if( rit!=begin() ) {
      unsigned int n=0;
      typedef std::pair<const_iterator,Parameters> q_pair;
      std::stack<q_pair> s;
      const_iterator it=begin();
      Parameters p=q_parms_;
      const_cast<QuantumNumber<I>&>(*it).set_parameters(p);
      if(it->levels()==std::numeric_limits<I>::max()) {
        num_states_=std::numeric_limits<I>::max();
        return true;
      }      
      for(half_integer<I> q=it->min();q<=it->max();++q) {
        p[it->name()]=q;
        s.push(q_pair(it,p));
      }
      while(!s.empty()) {
        const_iterator it=s.top().first;
        Parameters      p=s.top().second;
        s.pop();
        const_iterator itt=it+1;
        if(itt==rit) {
          const_cast<QuantumNumber<I>&>(*itt).set_parameters(p);
          if(itt->levels()==std::numeric_limits<I>::max()) {
            num_states_=std::numeric_limits<I>::max();
            return true;
          }
          n+=itt->levels();
        }
        else {
          ++it;
          const_cast<QuantumNumber<I>&>(*it).set_parameters(p);
          if(it->levels()==std::numeric_limits<I>::max()) {
            num_states_=std::numeric_limits<I>::max();
            return true;
          }
          for(half_integer<I> q=it->min();q<=it->max();++q) {
            p[it->name()]=q;
            s.push(q_pair(it,p));
          }
        }
      }
      num_states_ *= n;
    }
  }
  return valid_;
}

template <class I>
SiteBasisDescriptor<I>::SiteBasisDescriptor(const XMLTag& intag, std::istream& is)
 : valid_(false)
{
  XMLTag tag(intag);
  name_ = tag.attributes["name"];
  if (tag.type!=XMLTag::SINGLE) {
    tag = parse_tag(is);
    while (tag.name!="/SITEBASIS") {
      if (tag.name=="QUANTUMNUMBER") 
        push_back(QuantumNumber<I>(tag,is));
      else if (tag.name=="PARAMETER") 
        parms_[tag.attributes["name"]]=tag.attributes["default"];
      if (tag.type!=XMLTag::SINGLE)
        tag = parse_tag(is);
      tag = parse_tag(is);
    }
    if (tag.name !="/SITEBASIS")
      boost::throw_exception(std::runtime_error("Illegal tag <" + tag.name + "> in <SITEBASIS> element"));
  } 
  init_dependencies();

  // I need this line, otherwise the expressions in quantumnumbers cannot be evaluated. Dirty patch. Looks like a bug. To be looked at again. Axel Grzesik, 07/08/03
  evaluate();
}

template<class I>
void SiteBasisDescriptor<I>::init_dependencies() const {
  for(const_iterator it=begin();it!=end();++it) 
    for(const_iterator jt=begin();jt!=it;++jt) 
      if(const_cast<QuantumNumber<I>&>(*it).depends_on(jt->name())) 
        const_cast<QuantumNumber<I>&>(*it).add_dependency(*jt);
}

template <class I>
void SiteBasisDescriptor<I>::write_xml(oxstream& os) const
{
  os << start_tag("SITEBASIS") << attribute("name", name());
  for (Parameters::const_iterator it=parms_.begin();it!=parms_.end();++it)
    os << start_tag("PARAMETER") << attribute("name", it->key()) 
       << attribute("default", it->value()) << end_tag("PARAMETER");
  for (const_iterator it=begin();it!=end();++it)
    os << *it;
  os << end_tag("SITEBASIS");
}

}

#ifndef BOOST_NO_OPERATORS_IN_NAMESPACE
namespace alps {
#endif

template <class I>
inline alps::oxstream& operator<<(alps::oxstream& out, const alps::SiteBasisDescriptor<I>& q)
{
  q.write_xml(out);
  return out;        
}

template <class I>
inline std::ostream& operator<<(std::ostream& out, const alps::SiteBasisDescriptor<I>& q)
{
  alps::oxstream xml(out);
  xml << q;
  return out;        
}

#ifndef BOOST_NO_OPERATORS_IN_NAMESPACE
} // namespace alps
#endif

#endif