#pragma once

#include <typeinfo>
#include <algorithm>
#include <vector>

namespace extstl
{

template <bool FROM=false,bool INCLUSIVETO=false> struct vectorrange
{
	vectorrange()
	{
		if( FROM )
			m_nFrom = 0;
		if( INCLUSIVETO )
			m_nInclusiveTo = -1;
	}
	vectorrange(const int nA){ASSERT(FROM != INCLUSIVETO);if(FROM)m_nFrom=nA;else if(INCLUSIVETO)m_nInclusiveTo=nA;}
	vectorrange(const int nFrom,const int nInclusiveTo){ASSERT(FROM && INCLUSIVETO);if(FROM)m_nFrom=nFrom;if(INCLUSIVETO)m_nInclusiveTo=nInclusiveTo;}
	vectorrange(const vectorrange& o){*this=o;}
	
	bool isempty( void ) const { return getsize() < 1; }
	bool isvalid( const int nSize ) const { if( FROM && ( getfrom() >= nSize || getfrom() < 0 ) ) return false; if( INCLUSIVETO && ( getinclusiveto() >= nSize || getinclusiveto() < 0 ) ) return false; return true; }
	int getfrom( void ) const { ASSERT( FROM ); return m_nFrom; }
	int getinclusiveto( void ) const { ASSERT( INCLUSIVETO ); return m_nInclusiveTo; }
	int getsize( void ) const { return getinclusiveto() - getfrom() + 1; }

	template <typename VECTOR> typename std::vector<typename VECTOR::T_t>::const_iterator getbegin( const VECTOR *p ) const { ASSERT( p && !p->isempty() ); return FROM ? p->get().cbegin() + getfrom() : p->get().cbegin(); }
	template <typename VECTOR> typename std::vector<typename VECTOR::T_t>::const_iterator getend( const VECTOR *p ) const { ASSERT( p && !p->isempty() ); return INCLUSIVETO ? ( ( getinclusiveto() == int( p->get().size() - 1 ) ) ? p->get().cend() : p->get().cbegin() + getinclusiveto() + 1 ) : p->get().cend(); }
	
	vectorrange& operator =( const vectorrange& o ) { if( FROM ) m_nFrom=o.m_nFrom; if( INCLUSIVETO ) m_nInclusiveTo=o.m_nInclusiveTo; return *this; }
protected:
	int m_nFrom;
	int m_nInclusiveTo;
};

template <typename V> struct vectorequalpred
{
	bool operator()(const V::template T_t& spLhs, const V::template T_t& spRhs) const { return spLhs == spRhs; }
};

template <typename V,enum V::E_t ID> struct vectorlesspred : public std::binary_function<V::template T_t, V::template T_t, bool>
{
	bool operator()(const V::template T_t& spLhs, const V::template T_t& spRhs) const { return spLhs < spRhs; }
	static V::template E_t getid( void ) {return ID;}
};

template <typename V> struct vectorequalderefpred
{
	bool operator()(const V::template T_t& spLhs, const V::template T_t& spRhs) const { return *spLhs == *spRhs; }
};

template <typename V,enum V::E_t ID> struct vectorlessderefpred : public std::binary_function<V::template T_t, V::template T_t, bool>
{
	bool operator()(const V::template T_t& spLhs, const V::template T_t& spRhs) const { return *spLhs < *spRhs; }
	static V::template E_t getid( void ) {return ID;}
};

template <typename VECTOR,typename PRED> struct vectorunsorted
{
	using U_t = PRED;		// type accessor

	vectorunsorted(){}
	template <bool FROM=false,bool INCLUSIVETO=false> bool getindex(const VECTOR *pV, typename VECTOR::T_t p, const PRED& pred, int& n,const vectorrange<FROM,INCLUSIVETO>& r = vectorrange<FROM,INCLUSIVETO>()) const
	{
		const int nSize = static_cast<int>( pV->get().size() );
		if( !r.isvalid(nSize) )
		{
			ASSERT( false );
			return false;
		}

		typename std::vector<typename VECTOR::T_t>::const_iterator i = getiterator<FROM,INCLUSIVETO>(pV,r,p,pred);
		if( i == pV->get().cend()) return false;
		n = static_cast<int>( std::distance(pV->get().cbegin(), i) );
		return true;
	}
protected:
	template <bool FROM,bool INCLUSIVETO> typename std::vector<typename VECTOR::T_t>::const_iterator getiterator(const VECTOR *pV,const vectorrange<FROM,INCLUSIVETO>& r, typename VECTOR::T_t p,const PRED& pred) const
	{
		auto i = FROM ? r.getbegin(pV) : pV->get().cbegin();
		auto end = INCLUSIVETO ? r.getend(pV) : pV->get().cend();

		for (; i != end; ++i)
			if (pred((*i),p))
				return i;
		return pV->get().cend();
	}
};

template <typename VECTOR,typename SORTEDPRED,typename UNSORTEDPRED> struct vectorsorted
{
	using S_t = SORTEDPRED;			// type accessor
	using U_t = UNSORTEDPRED;		// type accessor

	vectorsorted(){}
	template <bool FROM=false,bool INCLUSIVETO=false> bool getindex(const VECTOR *pV, typename VECTOR::T_t p, const SORTEDPRED& sortedpred, const UNSORTEDPRED& unsortedpred, int& n,const vectorrange<FROM,INCLUSIVETO>& r = vectorrange<FROM,INCLUSIVETO>()) const
	{
		ASSERT( pV );
		if( !pV )
			return false;
		if( pV->getsorted() != sortedpred.getid() )
		{
			// default to slow sequential search
			ASSERT( false );
			return vectorunsorted<VECTOR,UNSORTEDPRED>().getindex<FROM,INCLUSIVETO>(pV,p,unsortedpred,n,r);
		}

		const int nSize = static_cast<int>( pV->get().size() );
		if( !r.isvalid(nSize) )
		{
			ASSERT( false );
			return false;
		}
		
		typename std::vector<typename VECTOR::T_t>::const_iterator i_pV = getiterator<FROM,INCLUSIVETO>(pV,r,p,sortedpred,unsortedpred);
		if( i_pV == pV->get().cend()) return false;
		n = static_cast<int>( std::distance(pV->get().cbegin(), i_pV) );
		return true;
	}
	template <bool FROM=false,bool INCLUSIVETO=false> bool getfirstnotbeforeindex(const VECTOR *pV, typename VECTOR::T_t p, const SORTEDPRED& sortedpred, int& n,const vectorrange<FROM,INCLUSIVETO>& r = vectorrange<FROM,INCLUSIVETO>()) const
	{
		ASSERT( pV );
		if( !pV )
			return false;
		ASSERT( pV->getsorted() == sortedpred.getid() );
		if( pV->getsorted() != sortedpred.getid() )
			return false;

		const int nSize = static_cast<int>( pV->get().size() );
		if( !r.isvalid(nSize) )
		{
			ASSERT( false );
			return false;
		}

		auto i = getfirstnotbeforeiterator<FROM,INCLUSIVETO>(pV,r,p,sortedpred);
		n = static_cast<int>( std::distance(pV->get().cbegin(), i) );
		return true;
	}
	void sort( VECTOR *pV, const SORTEDPRED& sortedpred )
	{
		ASSERT( pV );
		if( !pV )
			return;
		if( pV->getsorted() == sortedpred.getid() )
			return;
		if( pV->get().size() > 1 )
			std::sort( pV->getmutable().begin(), pV->getmutable().end(), sortedpred);
		pV->setsorted( sortedpred.getid() );
	}
	template <typename VECTOR_B=VECTOR> bool diff( const VECTOR *pA, const VECTOR_B *pB, VECTOR *pANOTB, VECTOR_B *pBNOTA, VECTOR *pAANDB, const SORTEDPRED& sortedpred, const UNSORTEDPRED& unsortedpred )
	{
		if( !pA || !pB || pA->getsorted() != sortedpred.getid() )
		{
			ASSERT(false);
			return false;
		}

		constexpr bool bSameVectorType = std::is_same_v<VECTOR,VECTOR_B>;

		if( bSameVectorType ? pB->getsorted() != sortedpred.getid() : pB->getsorted() != VECTOR_B::E_t::null )
		{
			ASSERT(false);
			return false;
		}

		ASSERT((!pANOTB || pANOTB->getsorted() == VECTOR::E_t::null ) &&
				 (!pBNOTA || pBNOTA->getsorted() == VECTOR_B::E_t::null ) &&
				 (!pAANDB || pAANDB->getsorted() == VECTOR::E_t::null));

		auto i_pA = pA->get().cbegin(), iend_pA = pA->get().cend();
		auto i_pB = pB->get().cbegin(), iend_pB = pB->get().cend();
		for (; i_pA != iend_pA && i_pB != iend_pB; )
			if (sortedpred((*i_pA),(*i_pB)))
			{
				if (pANOTB)
					pANOTB->push_back(*i_pA);
				++i_pA;
			}
			else
			if (unsortedpred((*i_pA),(*i_pB)))
			{
				if (pAANDB)
					pAANDB->push_back(*i_pA);
				++i_pA;
				++i_pB;
			}
			else
			{
				if (pBNOTA)
					pBNOTA->push_back(*i_pB);
				++i_pB;
			}
		
		ASSERT(i_pA == iend_pA || i_pB == iend_pB);
		if (pANOTB)
			while (i_pA != iend_pA)
			{
				pANOTB->push_back(*i_pA);
				++i_pA;
			}
		if (pBNOTA)
			while (i_pB != iend_pB)
			{
				pBNOTA->push_back(*i_pB);
				++i_pB;
			}
		
		if (pANOTB && pANOTB->getsorted()==VECTOR::E_t::null)
			pANOTB->setsorted(pA->getsorted());
		if (pBNOTA && pBNOTA->getsorted()==VECTOR_B::E_t::null)
			pBNOTA->setsorted(pB->getsorted());
		if (pAANDB && pAANDB->getsorted()==VECTOR::E_t::null)
			pAANDB->setsorted(pA->getsorted());

		return true;
	}
protected:
	template <bool FROM,bool INCLUSIVETO> typename std::vector<typename VECTOR::T_t>::const_iterator getiterator(const VECTOR *pV, const vectorrange<FROM,INCLUSIVETO>& r, typename VECTOR::T_t p, const SORTEDPRED& sortedpred, const UNSORTEDPRED& unsortedpred) const
	{
		// get first iterator representing position >= val
		typename std::vector<typename VECTOR::T_t>::const_iterator i_pV = getfirstnotbeforeiterator<FROM,INCLUSIVETO>(pV,r,p,sortedpred);
		if( i_pV == pV->get().cend() )
			return i_pV;
		return unsortedpred(*i_pV,p) ? i_pV : pV->get().cend();
	}
	template <bool FROM,bool INCLUSIVETO> typename std::vector<typename VECTOR::T_t>::const_iterator getfirstnotbeforeiterator(const VECTOR *pV,const vectorrange<FROM,INCLUSIVETO>& r, typename VECTOR::T_t p, const SORTEDPRED& sortedpred) const
	{
		// get first iterator representing position >= val
		const int nSize = static_cast<int>( pV->get().size() );
		typename std::vector<typename VECTOR::T_t>::const_iterator begin = FROM ? r.getbegin(pV) : pV->get().cbegin();
		typename std::vector<typename VECTOR::T_t>::const_iterator end = INCLUSIVETO ? r.getend(pV) : pV->get().cend();
		typename std::vector<typename VECTOR::T_t>::const_iterator i_pV = std::lower_bound(begin, end, p, sortedpred);
		return i_pV;
	}
};

}
