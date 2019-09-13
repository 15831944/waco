// AutobuildToolbar.h: interface for the AutobuildToolbar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOBUILDTOOLBAR_H__57487734_302B_11D4_9F94_0008C7999B1D__INCLUDED_)
#define AFX_AUTOBUILDTOOLBAR_H__57487734_302B_11D4_9F94_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifdef _DEBUG
	#undef _DEBUG
	#include "adui.h"
	#include "acui.h"
	#define _DEBUG
#else
	#include "adui.h"
	#include "acui.h"
#endif

class AutobuildToolbar : public CAcUiDockControlBar  
{
public:
	AutobuildToolbar();
	virtual ~AutobuildToolbar();

};

#endif // !defined(AFX_AUTOBUILDTOOLBAR_H__57487734_302B_11D4_9F94_0008C7999B1D__INCLUDED_)
