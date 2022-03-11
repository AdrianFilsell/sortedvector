#pragma once

#include <vector>
#include <memory>
#include <set>
#include <map>
#include "vectorpredicate.h"

namespace extstl
{

class intvector;

template <typename T,typename E> class vector
{
public:

	using T_t = T;				// type accessor
	using E_t = E;				// type accessor

	vector(){m_nSorted=E::null;}
	vector(const vector& other){(*this)=other;}
	vector(const std::vector<T>& other){(*this)=other;}

	const std::vector<T>& get(void) const { return m_v; }
	bool isempty(void) const { return m_v.size() == 0; }
	E getsorted( void ) const { return m_nSorted; }
	bool unusedcapacity( void ) const { const size_t s = m_v.size(); const size_t c = m_v.capacity(); return c > s; }

	std::vector<T>& getmutable(void) { setsorted(E::null); return m_v; }
	void setsorted( const E n ) { m_nSorted=n; }
	void reserve(const int n) { m_v.reserve(n); }
	bool resize(const int n) { if( n < 0 ) return false; const int nPre = static_cast<int>(m_v.size()); if(nPre==n) return true; m_v.resize(n); if( n > nPre ) setsorted(E::null); return static_cast<int>(m_v.size()) == n; }
	bool shrink( const int n ) { if( n < 0 || n > m_v.size() ) return false; m_v.resize(m_v.size()-n); return true; }
	bool set(const int n,T p) { if( n < 0 || n >= m_v.size() ) return false; setsorted(E::null); m_v[n]=p; return true;}
	void push_back(T p) { setsorted(E::null); m_v.push_back(p); }
	bool insert(const int n, T p) { if( n < 0 || n > m_v.size() ) return false; m_v.insert(m_v.cbegin() + n, p); setsorted(E::null); return true; }
	bool insert(const int n, const vector<T,E> * ps) { return insert( n, ps, 0, ps?static_cast<int>( ps->get().size() - 1):0 ); }
	bool insert(const int n, const vector<T,E> * ps, const int nFrom, const int nInclusiveTo)
	{
		if( !ps || nFrom < 0 || nInclusiveTo < nFrom || nInclusiveTo > ps->get().size() - 1 )
			return false;
		if( n < 0 || n > m_v.size() )
			return false;
		std::vector<T>::template const_iterator i = ps->get().cbegin() + nFrom, end = (nInclusiveTo == (ps->get().size() - 1)) ? ps->get().cend() : ps->get().cbegin() + nInclusiveTo + 1;
		m_v.insert(m_v.cbegin()+n, i, end);
		setsorted(E::null);
		return true;
	}
	bool append(const std::vector<T> * ps) { if( !ps ) return false; if( ps->size() == 0 ) return true; m_v.insert(m_v.cend(), ps->cbegin(), ps->cend()); setsorted(E::null); return true; }
	bool append(const vector<T,E> * ps) { return insert(static_cast<int>( m_v.size() ), ps, 0, ps?static_cast<int>( ps->get().size() - 1):0); }
	bool append(const vector<T,E> * ps, const int nFrom, const int nInclusiveTo) { return insert(static_cast<int>( m_v.size() ), ps, nFrom, nInclusiveTo); }
	bool erase(const int nFrom, const int nNonInclusiveTo)
	{
		if( nNonInclusiveTo == nFrom + 1 )
		{
			if( nFrom < 0 || nFrom >= m_v.size() )
				return false;
			m_v.erase(m_v.cbegin() + nFrom); 
		}
		else
		{
			if( nFrom < 0 || nFrom >= m_v.size() || nNonInclusiveTo <= nFrom )
				return false;
			if( nNonInclusiveTo > static_cast<int>(m_v.size()) )
				return false;
			m_v.erase(m_v.cbegin() + nFrom, m_v.cbegin() + nNonInclusiveTo); 
		}
		return true;
	}
	void erase(const std::shared_ptr<intvector> p)
	{
		if( !p )
			return;
		std::shared_ptr<const intvector> sp = intvector::clonesort(p);
		erase(sp->get());
	}
	void erase(const std::vector<int>& vSorted)
	{
		if( vSorted.size() == 0 )
			return;
		auto i_v = vSorted.crbegin(), end = vSorted.crend();
		#ifdef _DEBUG
			int nLast = *i_v;
		#endif
		for( ; i_v != end ; ++i_v )
		{
			#ifdef _DEBUG
				ASSERT(*i_v <= nLast);
				nLast = *i_v;
			#endif
			const bool b = erase(*i_v,(*i_v)+1);
			ASSERT( b );
		}
	}
	void reverse(void) { const int nSize = static_cast<int>(m_v.size()); for( int n = 0 ; n < nSize/2 ; ++n ) { std::swap<T>(m_v[n],m_v[nSize-1-n]); } setsorted(E::null); }
	void clear(void) { m_v.clear(); setsorted(E::null);}

	vector<T,E>& operator =(const vector<T,E>& o ) { m_v=o.m_v; m_nSorted = o.m_nSorted;return *this; }
	bool operator == ( const vector<T,E>& other ) const { if( m_nSorted != other.m_nSorted ) return false; if( m_v.size() != other.m_v.size() ) return false; auto i = m_v.cbegin(), end = m_v.cend(), j = other.m_v.cbegin(); for( ; i != end ; ++i, ++j ) if( !((*i) == (*j)) ) return false; return true; }
private:
	std::vector<T> m_v;
	E m_nSorted;
};

struct intvectororder
{
	enum type { null = 0, primary };
};

class intvector : public vector<int,intvectororder::type>
{
public:
	intvector(){}
	std::shared_ptr<intvector> clone( void ) const { std::shared_ptr<intvector> sp{new intvector(*this)}; return sp; }
	static std::shared_ptr<const intvector> clonesort( const std::shared_ptr<const intvector> in )
	{
		if( !in || ( in->getsorted() == lesspred::getid() ) )
			return in;
		std::shared_ptr<intvector> out = in->clone();
		sorted().sort( out.get(), sorted::S_t() );
		return out;
	}

	intvector& operator =(const intvector& o ) { vector<int,intvectororder::type>::operator=(o); return *this; }

	// primary type comparison
	typedef vectorequalpred<intvector> equalpred;
	typedef vectorlesspred<intvector,intvectororder::primary> lesspred;
	typedef vectorsorted<intvector,lesspred,equalpred> sorted;
};

}
