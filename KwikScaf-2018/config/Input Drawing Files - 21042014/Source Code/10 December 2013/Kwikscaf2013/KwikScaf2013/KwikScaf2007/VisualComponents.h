// VisualComponents.h: interface for the VisualComponents class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VISUALCOMPONENTS_H__A4C7E716_82C5_11D4_9FD9_0008C7999B1D__INCLUDED_)
#define AFX_VISUALCOMPONENTS_H__A4C7E716_82C5_11D4_9FD9_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "componentList.h"
#include "MeccanoDefinitions.h"	// Added by ClassView

class Controller;

class VisualComponents : public ComponentList  
{
public:
	int AddComponent( Component *pComponent );
	void Serialize(CArchive &ar);
	void SetAllComponentsVisible();
	void CleanUp(bool bShowProgress);
	DirtyFlagEnum GetDirtyFlag() const;
	void SetDirtyFlag( DirtyFlagEnum dfDirtyFlag );
	Controller *GetController() const;
	void SetController( Controller *pController );
	void Delete3DView();
	bool Create3DView( int &iPos );
	VisualComponents();
	virtual ~VisualComponents();

protected:
	DirtyFlagEnum m_dfDirtyFlag;
	Controller *m_pController;
};

#endif // !defined(AFX_VISUALCOMPONENTS_H__A4C7E716_82C5_11D4_9FD9_0008C7999B1D__INCLUDED_)
