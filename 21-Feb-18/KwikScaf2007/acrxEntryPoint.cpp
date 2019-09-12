// (C) Copyright 2002-2005 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- acrxEntryPoint.h
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"

#include "Component.h"
#include "MeccanoDefinitions.h"
#include "Main.h"
#include "Controller.h"
#include "AutoBuildTools.h"
#include "EntityReactor.h"
#include "Entity.h"
#include "MdiAware.h"
#include "ScaffoldArchive.h"
//#include "Security.h"
#include "HelpAboutDlg.h"
#include <windows.h>
#include "aced.h"


//-----------------------------------------------------------------------------
#define szRDS _RXST(_T(""))


#ifdef	USE_LINE_CLASS
#include "Line.h"
#endif	//#ifdef	USE_LINE_CLASS

// ****************************************************************
//  GLOBAL VARIABLE!!!  *******************************************
//  gpAsdkAppDocGlobals *******************************************
// ****************************************************************
bool						           gbOpenFileDialogActive;
bool						           bCreatingLapboards;
bool						           bCreatingTieTube;
AsdkAppDocGlobals			    *gpAsdkAppDocGlobals; 
AsdkMeccanoEditorReactor	*gpMeccanoEditorReactor;
Controller					      *gpController;
ScaffoldArchive				    *gpScaffoldArchive;
//Security					        *gpSecurity;
HelpAboutDlg				      *g_pSplashScreen;
CTime						           m_StartTime;
//HINSTANCE					        _hdllInstance;

double						STAR_SEPARATION;
double						JACK_LENGTH;
double						RL_ADJUST;
double						STAR_SEPARATION_APPROX;
double						JACK_LENGTH_APPROX;
double						RL_ADJUST_APPROX;
double						EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD;
double						EXTRA_LENGTH_AT_TOP_OF_STANDARD;

#define	ALLOW_INDIVIDUAL_COMPONENTS

bool f_bNoSentinelCheck = false;
//extern bool CheckForProductKey();

extern bool CheckForSentinelKey();

//bool acedLoadPartialMenu(const TCHAR* pszMenuFile);

//////////////////////////////////////////////////////////////////////////////////
bool TestController()
{
   CString	sMsg;

  if( gpController==NULL )
    return false;

  if( !gpController->IsComponentDetailsLoaded() )
  {
    sMsg = _T("Cannot proceed without valid ComponentDetails file.\n\nPlease restart AutoCAD to reload file.");
    MessageBox( NULL, sMsg, _T("STOP! Error"), MB_OK );
    return false;
  }

  if( !gpController->IsStockFileLoaded() )
  {
    sMsg = _T("Cannot proceed without valid Stock file.\n\nPlease restart AutoCAD to reload file.");
    MessageBox( NULL, sMsg, _T("STOP! Error"), MB_OK );
    return false;
  }

  // Commented by ~SJ~ since the security code has changed
  //if( !gpSecurity->QuerySecurity() ) return false;
  //return true;

  // Check for the Sentinel lock if the cheat code is not set
  return (f_bNoSentinelCheck ? true : CheckForSentinelKey());
}

//////////////////////////////////////////////////////////////////////////////////
void TrapMouse()
{
  ;//assert( gpController!=NULL );
  gpController->GetABTools()->SetMouseTrack(false);

  if (gbMouseDone == true) // already has the hook?
    return;

  if (acedRegisterFilterWinMsg(filterMouse) == false)
    acutPrintf(_T("Can't register Windows Msg hook - VH - mouse\n")); 
  else
    gbMouseDone = true;
}

//////////////////////////////////////////////////////////////////////////////////
void UnTrapMouse()
{
  if (gbMouseDone == true)
  {
    acedRemoveFilterWinMsg(filterMouse); 
    gbMouseDone = false;
  }
}

BOOL filterMouse(MSG *pMsg)
{
  if( g_pSplashScreen!=NULL )
  {
    bool bDelete = false;
    if( pMsg->message==WM_MOUSEMOVE )
    {
      bDelete = true;
    }
    else if( pMsg->message==WM_TIMER )
    {
      int			iSeconds;
      CTime		CurrentTime;
      CTimeSpan	LengthOfTime;
      CurrentTime = CurrentTime.GetCurrentTime();

      LengthOfTime = CurrentTime-m_StartTime;
      iSeconds = LengthOfTime.GetSeconds();
      if( iSeconds>3 )
      {
        bDelete = true;
      }
    }

    if(bDelete)
    {
      g_pSplashScreen->ShowWindow( SW_HIDE );
      g_pSplashScreen->EndDialog(1);
      delete g_pSplashScreen;
      g_pSplashScreen = NULL;
      //if( giAutobuildNumber<=0 )
        //UnTrapMouse();
      ;//assert( gpController!=NULL );
      gpController->RefreshScreen();
    }
  }

  //;//assert( gpController!=NULL );
  if( gpController!=NULL )
  {
  if( bCreatingLapboards )
    return gpController->GetABTools()->filterMouseForLapboard( pMsg );

  if( bCreatingTieTube )
    return gpController->GetABTools()->filterMouseForTieTube( pMsg );

  return gpController->GetABTools()->filterMouse( pMsg );
  }
  else
	  return false;
}
//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CKwikScaf2007App : public AcRxArxApp {

public:
	CKwikScaf2007App () : AcRxArxApp () {}

  virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) 
  {
	AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg (pkt) ;

	//****LICENSE***
	LPCSTR ZXml_Path = "C:\\Program Files\\Autodesk\\License.xml"; //"C:\\ProgramData\\Autodesk\\ApplicationPlugins\\KwikScaf.bundle\\Contents\Windows\en\\License.xml" ;//
	LPCSTR ZExe_Path = "C:\\Program Files\\Autodesk\\KwikscafApplication.exe";//"C:\\ProgramData\\Autodesk\\ApplicationPlugins\\KwikScaf.bundle\\Contents\Windows\en\\KwikscafApplication.exe";// ;
   // DWORD ftyp = GetFileAttributesA(ZdirName_in.c_str());
	DWORD ftyp_xml = GetFileAttributesA(ZXml_Path);
	DWORD ftyp_exe = GetFileAttributesA(ZExe_Path);
	if (ftyp_xml == INVALID_FILE_ATTRIBUTES || ftyp_exe == INVALID_FILE_ATTRIBUTES)
	{
		MessageBoxA(NULL, ("License.xml or KwikscafApplication.exe file Not Found!..Please Contact Administrator."), ("File Not Found Error!"),MB_OK);
		return AcRx::kRetError;  //something is wrong with your path!
	}

	HWND hwnd;
	/*ShellExecute(hwnd, NULL, _T("E:\2014\Kwikscaf License\Debug\\Kwikscaf License.exe"), NULL, NULL, SW_HIDE );
	CloseWindow(hwnd);*/
	HINSTANCE hInt =  ShellExecute(NULL, NULL, _T("C:\\Program Files\\Autodesk\\KwikscafApplication.exe"), NULL, NULL, SW_HIDE);
	CloseWindow(hwnd);
	Sleep(1000);
	int retval = ::_tsystem( _T("taskkill /F /T /IM KwikscafApplication.exe") );
	char* StringName;
	DWORD dwType = REG_SZ;

	HKEY hKey = 0;
	char value[1024];
	DWORD value_length = 1024;
	char buf[255] = {0};
	DWORD dwBufSize = sizeof(buf);

   CString strStatus, strMessage;
   CRegKey regKey;
   regKey.Open( HKEY_CURRENT_USER, _T("Software\\Kwikscaf\\Protection") );
   ULONG len = 255;
   ULONG len2 = 255;
   regKey.QueryStringValue( TEXT("status"), strStatus.GetBufferSetLength( len ),  &len );
   strStatus.ReleaseBuffer();
   
   if(!(strStatus.CompareNoCase(_T("Access")) == 0))
   {
	   regKey.QueryStringValue( TEXT("Message"), strMessage.GetBufferSetLength( len2 ),  &len2 );
       strMessage.ReleaseBuffer();
	   MessageBox(NULL,strMessage, _T("Security Alert!"), MB_OK);
	   return AcRx::kRetError;
   }
//****LICENSE END***    

   		
    AcMemFile::rxInit();
    EntityReactor::rxInit();
    Entity::rxInit();
    Component::rxInit();
    AsdkDbReactor::rxInit();
#ifdef	USE_LINE_CLASS
    Line::rxInit();
#endif	//#ifdef	USE_LINE_CLASS

    acrxBuildClassHierarchy();

    gbOpenFileDialogActive = false;
    //gpSecurity = NULL;
    gpController = NULL;
    gpScaffoldArchive = NULL;

    acrxDynamicLinker->unlockApplication(pkt);
    acrxRegisterAppMDIAware(pkt);

    gpAsdkAppDocGlobals = new AsdkAppDocGlobals(curDoc()); 
    gpAsdkAppDocGlobals->setGlobals(curDoc()); 

    gpMeccanoEditorReactor = new AsdkMeccanoEditorReactor();
    acedEditor->addReactor( gpMeccanoEditorReactor );

    // DO NOT CHANGE THE ORDER OF ASSIGNMENT IN THE BELOW CODE
    // ~SJ~, 11.01.2008
    STAR_SEPARATION			                = STAR_SEPARATION_KWIKSTAGE;
    JACK_LENGTH				                  = STAR_SEPARATION;
    RL_ADJUST				                    = STAR_SEPARATION;
    STAR_SEPARATION_APPROX	            = STAR_SEPARATION_APPROX_KWIKSTAGE;
    JACK_LENGTH_APPROX		              = STAR_SEPARATION_APPROX;
    RL_ADJUST_APPROX		                = STAR_SEPARATION_APPROX;
    EXTRA_LENGTH_AT_TOP_OF_STANDARD		  = EXTRA_LENGTH_KS_BOTTOM_OF_STANDARD;
    EXTRA_LENGTH_AT_BOTTOM_OF_STANDARD	= EXTRA_LENGTH_KS_BOTTOM_OF_STANDARD;

    bCreatingLapboards = false;
    bCreatingTieTube = false;

    //gpSecurity = new Security(acedGetAcadFrame());

    TrapMouse();

    m_StartTime = m_StartTime.GetCurrentTime();
    g_pSplashScreen = new HelpAboutDlg();
    g_pSplashScreen->SetModal(true);
#ifndef	_DEBUG
    // Commented by ~SJ~ since the new locks do not have serial numbers
    //g_pSplashScreen->SetHardwareLockID( gpSecurity->GetSerialNumber() );
#endif	//#ifndef	_DEBUG

    g_pSplashScreen->Create(IDD_HELP_ABOUT);
    g_pSplashScreen->ShowWindow( SW_SHOW );
    g_pSplashScreen->BeginModalState();
    g_pSplashScreen->SetFocus();

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) 
  {
    acedEditor->removeReactor( gpMeccanoEditorReactor );
    DELETE_PTR(gpMeccanoEditorReactor);
    if (gpAsdkAppDocGlobals != NULL) 
    { 
      gpAsdkAppDocGlobals->unload(); 
      DELETE_PTR(gpAsdkAppDocGlobals); 
    } 

    //DestroyController(); // this is done now in the mdi aware class AsdkPerDocData

    // Remove the AsdkObjectToNotify class from the ACRX
    // runtime class hierarchy. If this is done while the
    // database is still active, it should cause all objects
    // of class AsdkObjectToNotify to be turned into proxies.
    // 
    deleteAcRxClass(AcMemFile::desc());
    deleteAcRxClass(Component::desc());
    deleteAcRxClass(Entity::desc());
    deleteAcRxClass(EntityReactor::desc());
    deleteAcRxClass(AsdkDbReactor::desc());
#ifdef	USE_LINE_CLASS
    deleteAcRxClass(Line::desc());
#endif	//#ifdef	USE_LINE_CLASS

    //DELETE_PTR(gpSecurity);

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {	}

  // AutoBuild & Overrun
  static void KwikScaf2007_OverRun()            { if (TestController()) gpController->SetOverrun(!gpController->GetOverrun()); }
  static void KwikScaf2007_AutoBuild()          { if (TestController()) { TrapMouse(); giAutobuildNumber++; gpController->DoAutoBuild(); UnTrapMouse(); } }

  // Stage & Level
  static void KwikScaf2007_Stages()             { if (TestController()) gpController->SetStageForSelectedBays(); }
  static void KwikScaf2007_AddLevel()           { if (TestController()) gpController->AddLevel();                }
  static void KwikScaf2007_RegenMatrixLevels()  { if (TestController()) gpController->RegenMatrixLevels();       }
  static void KwikScaf2007_EnterLevels()        { if (TestController()) gpController->EnterLevels();             }

  // Level
  static void KwikScaf2007_ToggleLvlLength()    { if (TestController()) gpController->ToggleLvlLength();    }
  static void KwikScaf2007_ToggleFirstScale()   { if (TestController()) gpController->ToggleFirstScale();   }
  static void KwikScaf2007_ToggleScaleLines()   { if (TestController()) gpController->ToggleScaleLines();   }
  static void KwikScaf2007_ColourByStageLevel() { if (TestController()) gpController->ColourByStageLevel(); }

  // BOM
  static void KwikScaf2007_BOM()               { if (TestController()) gpController->BOM();             }
  static void KwikScaf2007_BOMS()              { if (TestController()) gpController->DoBOMSummary();    }
  static void KwikScaf2007_BOMExtra()          { if (TestController()) gpController->DisplayBOMExtra(); }

  // Views
  static void KwikScaf2007_RedrawSchematic()   { if (TestController()) gpController->RedrawSchematic(false);  }
  static void KwikScaf2007_UseActualComps()    { if (TestController()) gpController->UseActualComponents();   }
  static void KwikScaf2007_ClearActualComps()  { if (TestController()) gpController->ClearActualComponents(); }
  static void KwikScaf2007_CreateMatrix()      { if (TestController()) gpController->CreateMatrix();          }
  static void KwikScaf2007_DeleteMatrix()      { if (TestController()) gpController->DeleteMatrix();          }
  static void KwikScaf2007_Create3DView()      { if (TestController()) gpController->Create3DView();          }
  static void KwikScaf2007_Delete3DView()      { if (TestController()) gpController->Delete3DView();          }
  static void KwikScaf2007_Kut()               { if (TestController()) gpController->CreateCutThrough();      }
  static void KwikScaf2007_ResetKutSuggest()   { if (TestController()) gpController->ResetLabelCount();       }

  // Component & Stock Details
  static void KwikScaf2007_CCDFN()             { if (TestController()) gpController->GetCompDetails()->SetFilenameInRegistry(_T("")); }
  static void KwikScaf2007_CSDFN()             { if (TestController()) gpController->GetStockList()->ClearFilenameInRegistry(true);   }
  static void KwikScaf2007_CDDFN()             { if (TestController()) gpController->GetStockList()->ClearFilenameInRegistry(false);  }

  // Edit functions
  static void KwikScaf2007_EditBay()           { if (TestController()) gpController->EditBay();        }
  static void KwikScaf2007_EditRun()           { if (TestController()) gpController->EditRun();        }
  static void KwikScaf2007_ResizeBay()         { if (TestController()) gpController->ResizeBay();      }
  static void KwikScaf2007_InsertBay()         { if (TestController()) gpController->InsertBay();      }
  static void KwikScaf2007_StairBay()          { if (TestController()) gpController->StairBay();       }
  static void KwikScaf2007_LadderBay()         { if (TestController()) gpController->LadderBay();      }
  static void KwikScaf2007_ButtressBay()       { if (TestController()) gpController->ButtressBay();    }
  static void KwikScaf2007_ChangeRLs()         { if (TestController()) gpController->ChangeRLs();      }
  static void KwikScaf2007_EditComponents()    { if (TestController()) gpController->ComponentsEdit(); }
  static void KwikScaf2007_CopyComponents()    { if (TestController()) gpController->ComponentCopy();  }

  // Adding & Removing Components
  static void KwikScaf2007_AddBracing()             { if (TestController()) gpController->AddBracingToBay();         }
  static void KwikScaf2007_AddChainLink()           { if (TestController()) gpController->AddChainLinkToBay();       }
  static void KwikScaf2007_AddHandrail()            { if (TestController()) gpController->AddHandrailsToBay();       }
  static void KwikScaf2007_AddHopups()              { if (TestController()) gpController->AddHopupsToBay();          }
  static void KwikScaf2007_AddLapboard()            { if (TestController()) { bCreatingLapboards = true; TrapMouse(); gpController->GetABTools()->SetAccessEnabled(true); gpController->GetABTools()->CreateLapboard(); gpController->GetABTools()->SetAccessEnabled(false); UnTrapMouse(); bCreatingLapboards = false; }  }
  static void KwikScaf2007_AddTube()                { if (TestController()) { bCreatingTieTube   = true; TrapMouse(); gpController->GetABTools()->SetAccessEnabled(true); gpController->GetABTools()->CreateTieTube();  gpController->GetABTools()->SetAccessEnabled(false); UnTrapMouse(); bCreatingTieTube   = false; }  }
  static void KwikScaf2007_AddShadeCloth()          { if (TestController()) gpController->AddShadeClothToBay();      }
  static void KwikScaf2007_AddTie()                 { if (TestController()) gpController->AddTieToBay();             }
  static void KwikScaf2007_AddCornerStageBoard()    { if (TestController()) gpController->AddCornerStageBoard();     }
  static void KwikScaf2007_RemoveBracing()          { if (TestController()) gpController->DeleteBracingFromBays();   }
  static void KwikScaf2007_RemoveChainLink()        { if (TestController()) gpController->DeleteChainLinkFromBay();  }
  static void KwikScaf2007_RemoveHandrail()         { if (TestController()) gpController->DeleteHandrails();         }
  static void KwikScaf2007_RemoveHopups()           { if (TestController()) gpController->DeleteHopups();            }
  static void KwikScaf2007_RemoveShadeCloth()       { if (TestController()) gpController->DeleteShadeClothFromBay(); }
  static void KwikScaf2007_RemoveTies()             { if (TestController()) gpController->DeleteTiesFromBay();       }
  static void KwikScaf2007_RemoveCornerStageBoard() { if (TestController()) gpController->RemoveCornerStageBoard();  }
  static void KwikScaf2007_InsertComponent()        { if (TestController()) gpController->ComponentInsert();         }

  // Mills & KwikStage
  static void KwikScaf2007_UseMills()               { if (TestController()) gpController->SetSystem(S_MILLS);        }
  static void KwikScaf2007_UseKwikStage()           { if (TestController()) gpController->SetSystem(S_KWIKSTAGE);    }
  static void KwikScaf2007_ChangeSystem()           { if (TestController()) gpController->ChangeSystem();            }

  // Mills System
  static void KwikScaf2007_MillsSystemCnr()         { if (TestController()) gpController->MillsSystemCorner(); acedRetInt(gpController->IsMillsSystemCorner()? 1 : 0); }
  static void KwikScaf2007_ConvertToMillsCorner()   { if (TestController()) gpController->ConvertToMillsCorner();    }
  static void KwikScaf2007_UnconvertMillsCorner()   { if (TestController()) gpController->UnconvertMillsCorner();    }

  // Sydney Corner
  static void KwikScaf2007_SydneyCnr()              { if (TestController()) gpController->SydneyCorner(); acedRetInt(gpController->IsSydneyCorner()? 1 : 0); }
  static void KwikScaf2007_SydneyCnrSeparation()    { if (TestController()) gpController->SydneyCornerSeparation();  }

  // Hide Matrix
  static void KwikScaf2007_ShowEntireMatrix()       { if (TestController()) { gpController->ShowAllMatrixElements(); gpController->CreateMatrix(true); } }
  static void KwikScaf2007_HideMatrixSections()     { if (TestController()) { gpController->ExcludeMatrixElements(); gpController->CreateMatrix(true); } }

  // Miscellaneous Commands
  static void KwikScaf2007_JobDetail()              { if (TestController()) gpController->DisplayJobDetail();                   }
  static void KwikScaf2007_DoCleanUp()              { if (TestController()) gpController->CleanUp();                            }
  static void KwikScaf2007_SetArcLapSpan()          { if (TestController()) gpController->SetArcLapSpan();                      }
  static void KwikScaf2007_ClearCommitted()         { if (TestController()) gpController->ClearCommitted();                     }
  static void KwikScaf2007_EditStandards()          { if (TestController()) gpController->EditStandards();                      }
  static void KwikScaf2007_HCT()                    { if (TestController()) gpController->SelectHiddenComponentTypes();         }
  static void KwikScaf2007_ChangeColours()          { if (TestController()) gpController->SelectHiddenComponentTypes();         }
  static void KwikScaf2007_UseToeboardOnLapboard()  { if (TestController()) gpController->UseToeboardOnLapboard();              }
  static void KwikScaf2007_UsePutLogClipsInLadder() { if (TestController()) gpController->UsePutLogClipsInLadder();             }
  static void KwikScaf2007_WOLH()                   { if (TestController()) gpController->WallOffsetFromLowestHopup();          }
  static void KwikScaf2007_Use1500Soleboards()      { if (TestController()) gpController->Use1500Soleboards();                  }
  static void KwikScaf2007_SMSLOS()                 { if (TestController()) gpController->ChangeShowMatrixLabelsOnSchematic();  }
  static void KwikScaf2007_ShowStandardLengths()    { if (TestController()) gpController->ChangeShowStandardLengths();          }
  static void KwikScaf2007_ShowZLine()              { if (TestController()) gpController->DisplayZigZagLine();                  }
  static void KwikScaf2007_AllComponentsVisible()   { if (TestController()) gpController->SetAllComponentsVisible();            }
  static void KwikScaf2007_BaysInBOMS()             { if (TestController()) gpController->ShowBaysInBOMS();                     }
  static void KwikScaf2007_BaySizesInBOMS()         { if (TestController()) gpController->ShowBaySizesInBOMS();                 }
  static void KwikScaf2007_EditStock()              { if (TestController()) gpController->EditStock();                          }
  static void KwikScaf2007_CalcPerRoll()            { if (TestController()) gpController->CalcPerRoll();                        }

  // Hidden/Secret Commands
  static void KwikScaf2007_TC()                     { if (TestController()) gpController->TraceContents();                      }
  static void KwikScaf2007_DS()                     { if (TestController()) gpController->ChangeDeleteSchematic();              }
  static void KwikScaf2007_SubT4L()                 { if (TestController()) { gpController->ChangeUse0700TransomForLedger(); gpController->ChangeUse1200TransomForLedger(); } }
  static void KwikScaf2007_DisplaySalePrice()       { if (TestController()) gpController->DisplaySalePrice();                   }
  static void KwikScaf2007_ShowSavedHistory()       { if (TestController()) gpController->SavedHistoryShow();                   }
  static void KwikScaf2007_ClearSavedHistory()      { if (TestController()) gpController->SavedHistoryClear();                  }
  static void KwikScaf2007_CnvrtSystemOnLoad()      { if (TestController()) gpController->SetConvertToSystemOnNextOpen(true);   }
  static void KwikScaf2007_ShowStockList()          { if (TestController()) if (gpController->GetStockList() != NULL) gpController->GetStockList()->TraceContents(true);  } 
  static void KwikScaf2007_StoreStockList()         { if (TestController()) if (gpController->GetStockList() != NULL) gpController->GetStockList()->TraceContents(false); }
  static void KwikScaf2007_KwikRLs()                { if (TestController()) if (gpController->GetStockList() != NULL) gpController->SetRLs();         }
  static void KwikScaf2007_DrawKwikRLs()            { if (TestController()) if (gpController->GetStockList() != NULL) gpController->DrawKwikRLs();    }
  static void KwikScaf2007_FindRLs()                { if (TestController()) if (gpController->GetStockList() != NULL) gpController->ShowRLforPoint(); }
  static void KwikScaf2007_KwikRegen()              { if (TestController()) if (gpController->GetStockList() != NULL) gpController->UpdateAllVeiws(); }
  static void KwikScaf2007_TVC()                    
  { 
    if (TestController())
    {
      int iCount, i, j;
      Component *pComponent;
      acutPrintf(_T("\nThere are %i visual components:\n"), gpController->GetVisualComponents()->GetNumberOfComponents());
      for (i = CT_DECKING_PLANK; i <= CT_VISUAL_COMPONENT; i++)
      {
        iCount = 0;
        for (j = 0; j < gpController->GetVisualComponents()->GetNumberOfComponents(); j++)
        {
          pComponent=gpController->GetVisualComponents()->GetComponent(j);
          if (pComponent->GetType() == (ComponentTypeEnum)i)
          {
            iCount++;
          }
        }
        if (iCount > 0)
        {
          acutPrintf(_T("%i x %s\n"), iCount, GetComponentDescStr((ComponentTypeEnum)i));
        }
      }
    }
  }

  // About
  static void KwikScaf2007_AboutKwikscaf()          
  { 
    HelpAboutDlg Dialog;
    if (TestController())
    {
#ifndef	_DEBUG
      // Commented by ~SJ~ since the new locks do not have serial numbers
      //Dialog.SetHardwareLockID(gpSecurity->GetSerialNumber());
#endif	//#ifndef	_DEBUG
      Dialog.DoModal();
    }
    else
    {
      Dialog.SetShowHardwareKeyMsg(true);
      Dialog.DoModal();
    }
  }

  // Cheat code
  static void KwikScaf2007_DMKS() 
  { 
    // Get value of USERI2
    struct resbuf rbUserI2;
    acedGetVar(_T("USERI2"), &rbUserI2);

    // If the value is "1423" (our ADN code), set the flag
    if (rbUserI2.resval.rint == 1423) f_bNoSentinelCheck = true; 

    // Reset the USERI2 value to 0
    rbUserI2.resval.rint = 0;
    acedSetVar(_T("USERI2"), &rbUserI2);

    // Print this to confuse anyone entering this command
    acutPrintf(_T("Unknown command \"DMKS\".  Press F1 for help.\n")); 
  }
};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CKwikScaf2007App)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Command registrations

// AutoBuild & Overrun
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _AutoBuild,         AutoBuild,         ACRX_CMD_MODAL,       NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _OverRun,           OverRun,           ACRX_CMD_TRANSPARENT, NULL)

// Stage & Level
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _Stages,            Stages,            ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _AddLevel,          AddLevel,          ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _RegenMatrixLevels, RegenMatrixLevels, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _EnterLevels,       EnterLevels,       ACRX_CMD_TRANSPARENT, NULL)

// Level
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _ToggleLvlLength,    ToggleLvlLength,    ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _ToggleFirstScale,   ToggleFirstScale,   ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _ToggleScaleLines,   ToggleScaleLines,   ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _ColourByStageLevel, ColourByStageLevel, ACRX_CMD_TRANSPARENT, NULL)

// BOM	
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _BOM,               BOM,               ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _BOMS,              BOMS,              ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _BOMExtra,          BOMExtra,          ACRX_CMD_TRANSPARENT, NULL)

// Views
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _RedrawSchematic,   RedrawSchematic,   ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _UseActualComps,    UseActualComps,    ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _ClearActualComps,  ClearActualComps,  ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _CreateMatrix,      CreateMatrix,      ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _DeleteMatrix,      DeleteMatrix,      ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _Create3DView,      Create3DView,      ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _Delete3DView,      Delete3DView,      ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _Kut,               Kut,               ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _ResetKutSuggest,   ResetKutSuggest,   ACRX_CMD_TRANSPARENT, NULL)

// Component & Stock Details
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _CCDFN,             CCDFN,             ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _CSDFN,             CSDFN,             ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _CDDFN,             CDDFN,             ACRX_CMD_TRANSPARENT, NULL)

// Edit functions
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _EditBay,           EditBay,           ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _EditRun,           EditRun,           ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _ResizeBay,         ResizeBay,         ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _InsertBay,         InsertBay,         ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _StairBay,          StairBay,          ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _LadderBay,         LadderBay,         ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _ButtressBay,       ButtressBay,       ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _ChangeRLs,         ChangeRLs,         ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
#ifdef	ALLOW_INDIVIDUAL_COMPONENTS
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _EditComponents,    EditComponents,    ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _CopyComponents,    CopyComponents,    ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
#endif	//#ifdef	ALLOW_INDIVIDUAL_COMPONENTS

// Adding & Removing Components
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _AddBracing,             AddBracing,             ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _AddChainLink,           AddChainLink,           ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _AddHandrail,            AddHandrail,            ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _AddHopups,              AddHopups,              ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _AddLapboard,            AddLapboard,            ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _AddTube,                AddTube,                ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _AddShadeCloth,          AddShadeCloth,          ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _AddTie,                 AddTie,                 ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _AddCornerStageBoard,    AddCornerStageBoard,    ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _RemoveBracing,          RemoveBracing,          ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _RemoveChainLink,        RemoveChainLink,        ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _RemoveHandrail,         RemoveHandrail,         ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _RemoveHopups,           RemoveHopups,           ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _RemoveShadeCloth,       RemoveShadeCloth,       ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _RemoveTies,             RemoveTies,             ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _RemoveCornerStageBoard, RemoveCornerStageBoard, ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
#ifdef	ALLOW_INDIVIDUAL_COMPONENTS
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _InsertComponent,        InsertComponent,        ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
#endif	//#ifdef	ALLOW_INDIVIDUAL_COMPONENTS

// Mills & KwikStage
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _UseMills,               UseMills,               ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _UseKwikStage,           UseKwikStage,           ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _ChangeSystem,           ChangeSystem,           ACRX_CMD_TRANSPARENT, NULL)

// Mills System
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _MillsSystemCnr,         MillsSystemCnr,         ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _ConvertToMillsCorner,   ConvertToMillsCorner,   ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _UnconvertMillsCorner,   UnconvertMillsCorner,   ACRX_CMD_TRANSPARENT, NULL)

// Sydney Corners
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _SydneyCnr,              SydneyCnr,              ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _SydneyCnrSeparation,    SydneyCnrSeparation,    ACRX_CMD_TRANSPARENT, NULL)

// Hide Matrix
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _ShowEntireMatrix,       ShowEntireMatrix,       ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _HideMatrixSections,     HideMatrixSections,     ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)

// Miscellaneous Commands
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _JobDetail,              JobDetail,              ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _DoCleanUp,              DoCleanUp,              ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _SetArcLapSpan,          SetArcLapSpan,          ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _ClearCommitted,         ClearCommitted,         ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _EditStandards,          EditStandards,          ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _HCT,                    HCT,                    ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _ChangeColours,          ChangeColours,          ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _UseToeboardOnLapboard,  UseToeboardOnLapboard,  ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _UsePutLogClipsInLadder, UsePutLogClipsInLadder, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _WOLH,                   WOLH,                   ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _Use1500Soleboards,      Use1500Soleboards,      ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _SMSLOS,                 SMSLOS,                 ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _ShowStandardLengths,    ShowStandardLengths,    ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _ShowZLine,              ShowZLine,              ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _AllComponentsVisible,   AllComponentsVisible,   ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _BaysInBOMS,             BaysInBOMS,             ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _BaySizesInBOMS,         BaySizesInBOMS,         ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _EditStock,              EditStock,              ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _CalcPerRoll,            CalcPerRoll,            ACRX_CMD_TRANSPARENT, NULL)

// Hidden/Secret Commands
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _TC,                     TC,                     ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _TVC,                    TVC,                    ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _DS,                     DS,                     ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _SubT4L,                 SubT4L,                 ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _DisplaySalePrice,       DisplaySalePrice,       ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _ShowSavedHistory,       ShowSavedHistory,       ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _ClearSavedHistory,      ClearSavedHistory,      ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _CnvrtSystemOnLoad,      CnvrtSystemOnLoad,      ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _ShowStockList,          ShowStockList,          ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _StoreStockList,         StoreStockList,         ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _KwikRLs,                KwikRLs,                ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _DrawKwikRLs,            DrawKwikRLs,            ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _FindRLs,                FindRLs,                ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _KwikRegen,              KwikRegen,              ACRX_CMD_TRANSPARENT, NULL)

// About
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _AboutKwikscaf,          AboutKwikscaf,          ACRX_CMD_TRANSPARENT, NULL)

// Cheat code for developer (enter this command, so that it won't check for hardware lock)
ACED_ARXCOMMAND_ENTRY_AUTO(CKwikScaf2007App, KwikScaf2007, _DMKS, DMKS, ACRX_CMD_TRANSPARENT, NULL)
