// MillsType.cpp: implementation of the MillsType class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "meccano.h"
#include "MillsType.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MillsType::MillsType()
{
	iType = MILLS_TYPE_NONE;
}

MillsType::~MillsType()
{

}

void MillsType::ClearType()
{
	SetType(MILLS_TYPE_NONE);
}

void MillsType::SetType(int iType)
{
	m_iType = iType;
}

int MillsType::AddType(int iType)
{
	assert( (m_iType&iType)==MILLS_TYPE_NONE );
	m_iType = m_iType|iType;
}

int MillsType::GetType()
{
	return m_iType;
}

bool MillsType::HasStandard(SideOfBayEnum eSide)
{
	int		iType;
	bool	bResult;
	iType = GetType();

	bResult = true;

	if( (iType&MILLS_TYPE_MISSING_STND_TEST)>0 )
	{
		switch( eSide )
		{
		case( NE ):
			if( (iType&MILLS_TYPE_MISSING_STND_NE)>0 )
				bResult = false;
			break;
		case( SE ):
			if( (iType&MILLS_TYPE_MISSING_STND_SE)>0 )
				bResult = false;
			break;
		case( SW ):
			if( (iType&MILLS_TYPE_MISSING_STND_SW)>0 )
				bResult = false;
			break;
		case( NW ):
			if( (iType&MILLS_TYPE_MISSING_STND_NW)>0 )
				bResult = false;
			break;
		default:
			assert(false);
		}
	}
	return bResult;
}

bool MillsType::RemoveStandard(SideOfBayEnum eSide)
{
	if( !HasStandard(eSide) )
	{
		assert( false );
		return false;
	}

	switch( eSide )
	{
	case( NE ):
		AddType( MILLS_TYPE_MISSING_STND_NE );
		break;
	case( SE ):
		AddType( MILLS_TYPE_MISSING_STND_SE );
		break;
	case( SW ):
		AddType( MILLS_TYPE_MISSING_STND_SW );
		break;
	case( NW ):
		AddType( MILLS_TYPE_MISSING_STND_NW );
		break;
	default:
		assert(false);
		return false;
	}
	return true;
}

intArray MillsType::GetMissingStandards()
{

}

intArray MillsType::GetExistingStandards()
{

}

bool MillsType::HasHalfSize(SideOfBayEnum eSide)
{
	int		iType;
	bool	bResult;
	iType = GetType();

	bResult = false;

	if( (iType&MILLS_TYPE_CONNECT_TEST)>0 )
	{
		switch( eSide )
		{
		case( NNE ):
			if( (iType&MILLS_TYPE_CONNECT_NNW)>0 )
				bResult = true;
			break;
		case( ENE ):
			if( (iType&MILLS_TYPE_CONNECT_ESE)>0 )
				bResult = true;
			break;
		case( ESE ):
			if( (iType&MILLS_TYPE_CONNECT_ENE)>0 )
				bResult = true;
			break;
		case( SSE ):
			if( (iType&MILLS_TYPE_CONNECT_SSW)>0 )
				bResult = true;
			break;
		case( SSW ):
			if( (iType&MILLS_TYPE_CONNECT_SSE)>0 )
				bResult = true;
			break;
		case( WSW ):
			if( (iType&MILLS_TYPE_CONNECT_WNW)>0 )
				bResult = true;
			break;
		case( WNW ):
			if( (iType&MILLS_TYPE_CONNECT_WSW)>0 )
				bResult = true;
			break;
		case( NNW ):
			if( (iType&MILLS_TYPE_CONNECT_NNE)>0 )
				bResult = true;
			break;
		default:
			assert(false);
		}
	}
	return bResult;
}

bool MillsType::HasGap(SideOfBayEnum eSide)
{
	int		iType;
	bool	bResult;
	iType = GetType();

	bResult = false;

	if( (iType&MILLS_TYPE_CONNECT_TEST)>0 )
	{
		switch( eSide )
		{
		case( NNE ):
			if( (iType&MILLS_TYPE_CONNECT_NNE)>0 )
				bResult = true;
			break;
		case( ENE ):
			if( (iType&MILLS_TYPE_CONNECT_ENE)>0 )
				bResult = true;
			break;
		case( ESE ):
			if( (iType&MILLS_TYPE_CONNECT_ESE)>0 )
				bResult = true;
			break;
		case( SSE ):
			if( (iType&MILLS_TYPE_CONNECT_SSE)>0 )
				bResult = true;
			break;
		case( SSW ):
			if( (iType&MILLS_TYPE_CONNECT_SSW)>0 )
				bResult = true;
			break;
		case( WSW ):
			if( (iType&MILLS_TYPE_CONNECT_WSW)>0 )
				bResult = true;
			break;
		case( WNW ):
			if( (iType&MILLS_TYPE_CONNECT_WNW)>0 )
				bResult = true;
			break;
		case( NNW ):
			if( (iType&MILLS_TYPE_CONNECT_NNW)>0 )
				bResult = true;
			break;
		default:
			assert(false);
		}
	}
	return bResult;
}

bool MillsType::SetGap(SideOfBayEnum eSide)
{
	if( HasGap(eSide) )
	{
		assert( false );
		return false;
	}

	switch( eSide )
	{
	case( NNE ):
		AddType(MILLS_TYPE_CONNECT_NNW);
		break;
	case( ENE ):
		AddType(MILLS_TYPE_CONNECT_ESE);
		break;
	case( ESE ):
		AddType(MILLS_TYPE_CONNECT_ENE);
		break;
	case( SSE ):
		AddType(MILLS_TYPE_CONNECT_SSW);
		break;
	case( SSW ):
		AddType(MILLS_TYPE_CONNECT_SSE);
		break;
	case( WSW ):
		AddType(MILLS_TYPE_CONNECT_WNW);
		break;
	case( WNW ):
		AddType(MILLS_TYPE_CONNECT_WSW);
		break;
	case( NNW ):
		AddType(MILLS_TYPE_CONNECT_NNE);
		break;
	default:
		assert(false);
		return false;
	}
	return true;
}


intArray MillsType::GetGaps()
{

}

intArray MillsType::GetHalfs()
{

}

bool MillsType::IsMillsTypeSet()
{

}

