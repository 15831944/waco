// KwikDouble.cpp: implementation of the KwikDouble class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "meccano.h"
#include "KwikDouble.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KwikDouble::KwikDouble()
{

}

KwikDouble::~KwikDouble()
{

}

bool KwikDouble::operator==(const KwikDouble kdValue) const
{
	if( *this>kdValue-ROUND_ERROR && *this<kdValue+ROUND_ERROR )
		return true;
	return false;
}

bool KwikDouble::operator!=(const KwikDouble kdValue) const
{
	return !( operator==(const KwikDouble kdValue) );
}
