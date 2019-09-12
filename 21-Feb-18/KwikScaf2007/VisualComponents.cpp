// VisualComponents.cpp: implementation of the VisualComponents class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "meccano.h"
#include "VisualComponents.h"
#include "Component.h"
#include "Controller.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VisualComponents::VisualComponents()
{
	SetController(NULL);
	SetDirtyFlag(DF_CLEAN);
}

VisualComponents::~VisualComponents()
{

}

bool VisualComponents::Create3DView( int &iPos )
{
	Component *pComponent;
	int i;
	for(i=0; i<GetNumberOfComponents(); i++ )
	{
		pComponent = GetComponent(i);
		if( !pComponent->Create3DView() )
			return false;

		acedSetStatusBarProgressMeterPos(iPos++);
	}
	return true;
}

void VisualComponents::Delete3DView()
{
	Component *pComponent;
	int i;
	for(i=0; i<GetNumberOfComponents(); i++ )
	{
		pComponent = GetComponent(i);
		pComponent->Delete3DView();
	}
}

void VisualComponents::SetController(Controller *pController)
{
	m_pController = pController;
}

Controller * VisualComponents::GetController() const
{
	;//assert( m_pController!=NULL );
	return m_pController;
}

void VisualComponents::SetDirtyFlag(DirtyFlagEnum dfDirtyFlag)
{
	if( dfDirtyFlag==DF_CLEAN )
	{
		//set the dirty flag
		m_dfDirtyFlag = dfDirtyFlag;
		return;
	}

	if( m_dfDirtyFlag==DF_DELETE ||
		m_dfDirtyFlag==DF_DIRTY )
	{
		//don't worry about it, it is already marked
		return;
	}

	//set the dirty flag
	m_dfDirtyFlag = dfDirtyFlag;

	//tell the one above that it is dirty
	if( GetController()!=NULL )
		GetController()->SetDirtyFlag( DF_DIRTY );
}

DirtyFlagEnum VisualComponents::GetDirtyFlag() const
{
	return m_dfDirtyFlag;
}

void VisualComponents::CleanUp( bool bShowProgressBar )
{
	int			iFull, iPos;
	Component	*pComponent;

	if( bShowProgressBar )
	{
		iFull = GetNumberOfComponents();
		acedSetStatusBarProgressMeter( _T("Cleaning Visual Components"), 0, iFull );
		iPos = 0;
	}


	SetDirtyFlag( DF_CLEAN );

	for( int iVC=0; (GetNumberOfComponents()>0 && iVC<GetNumberOfComponents()); iVC++ )
	{
		if( bShowProgressBar )
			acedSetStatusBarProgressMeterPos(++iPos);

		pComponent = GetComponent(iVC);
		switch( pComponent->GetDirtyFlag() )
		{
		case(DF_CLEAN):
			//Fine
			break;
		case(DF_DELETE):
			if(!DeleteComponent(iVC) )
			{
				;//assert( false);
			}
			iVC--;
			break;
		default:
			;//assert( false );
		}
	}
}

void VisualComponents::SetAllComponentsVisible()
{
	int			i;
	Component	*pComponent;
	for( i=0; i<GetNumberOfComponents(); i++ )
	{
		pComponent = GetComponent( i );
		;//assert( pComponent!=NULL );
		pComponent->SetVisible(VIS_VISIBLE);
	}
}

void VisualComponents::Serialize(CArchive &ar)
{
	int					i, iSize, iTemp, iSystem;
	double				dLength;
	CString				sMsg, sTemp;
	Component			*pComponent;
	MaterialTypeEnum	eMaterial;
	ComponentTypeEnum	eType;

	if (ar.IsStoring())    // Store Object?
	{
		ar << VISUAL_COMPONENTS_VERSION_LATEST;

		//VISUAL_COMPONENTS_VERSION_1_0_0
		if( !GetController()->SaveSelectedOnly() )
		{
			iSize = GetSize();
		}
		else
		{
			iSize = 0;
			for( i=0; i<GetSize(); i++ )
			{
				pComponent	= GetComponent(i);
				if( pComponent->IsComponentSelected() )
				{
					iSize++;
				}
			}
		}
		ar << iSize;
		if( iSize>0 )
		{
			for( i=0; i<GetSize(); i++ )
			{
				pComponent	= GetComponent(i);
				if( !GetController()->SaveSelectedOnly() ||
					pComponent->IsComponentSelected() )
				{
					dLength		= pComponent->GetLengthCommon();
					eType		= pComponent->GetType();
					eMaterial	= pComponent->GetMaterialType();
					ar << dLength;
					iTemp = (int)eType;
					ar << iTemp;
					iTemp =  (int)eMaterial;
					ar << iTemp;
					pComponent->Serialize(ar);
				}
			}
		}
	}
	else
	{
		if( !GetController()->IsInsertingBlock() )
		{
			RemoveAll();
		}

		VersionNumber uiVersion;
		iSystem = (int)GetController()->GetSystem();
		ar >> uiVersion;
		switch (uiVersion)
		{
		case VISUAL_COMPONENTS_VERSION_1_0_0 :
			//LOAD_bool_IN_AR(bDummy);
			ar >> iSize;
			for( i=0; i<iSize; i++ )
			{
				ar >> dLength;
				ar >> iTemp;
				eType = (ComponentTypeEnum)iTemp;
				ar >> iTemp;
				eMaterial = (MaterialTypeEnum)iTemp;

				//Just use the controllers system setting,
				//	the serialize will override this value anyway
				pComponent = new Component( dLength, eType, eMaterial, GetController()->GetSystem() );
				AddComponent(pComponent);
				pComponent->Serialize(ar);
			}
			break;

		default:
			;//assert( false );
			if( uiVersion>VISUAL_COMPONENTS_VERSION_LATEST )
			{
				sMsg = _T("This file has been created with a newer version of KwikScaf than you currently have installed.\n");
				sMsg+= _T("To open this file you will need to upgrade your version of KwikScaf.\n");
				sMsg+= _T("Please refer to the About KwikScaf dialog box to find your current version of KwikScaf.\n\n");
			}
			else
			{
				sMsg = _T("An unidentified error has occured during loading of this file.\n");
				sMsg+= _T("Please contact the KwikScaf team for further information!\n\n");
			}
			sMsg+= _T("Details of error -\n");
      sMsg+= _T("Class: VisualComponents.\n");
      sTemp.Format( _T("Expected Version: %i.\nFile Version: %i."), VISUAL_COMPONENTS_VERSION_1_0_0, uiVersion );
			sMsg+= sTemp;
			MessageBox( NULL, sMsg, _T("Invalid File Version"), MB_OK );
			ar.Close();
		}
	}
}

int VisualComponents::AddComponent(Component *pComponent)
{
	int	iReturn;

	iReturn = ComponentList::AddComponent((SideOfBayEnum)0, 0, pComponent);

	if( iReturn>=0 )
	{
		pComponent->SetVisualComponentsPointer( this );
		pComponent->SetStockDetailsPointer();
		pComponent->SetID(iReturn);
	}

	return iReturn;
}
