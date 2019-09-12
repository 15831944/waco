// KwikDouble.h: interface for the KwikDouble class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KWIKDOUBLE_H__A003C804_373E_11D4_9F99_0008C7999B1D__INCLUDED_)
#define AFX_KWIKDOUBLE_H__A003C804_373E_11D4_9F99_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class KwikDouble : public double  
{
public:
	KwikDouble();
	virtual ~KwikDouble();

	bool operator==(const KwikDouble kdValue) const;
	bool operator!=(const KwikDouble kdValue) const;
};

#endif // !defined(AFX_KWIKDOUBLE_H__A003C804_373E_11D4_9F99_0008C7999B1D__INCLUDED_)
