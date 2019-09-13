// Main.h: interface for the dll.
//
//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
//////////////////////////////////////////////////////////////////////

#if !defined(MAIN_H)
#define MAIN_H

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//								MAIN FILE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 4/12/00 4:32p $
//	Version			: $Revision: 80 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file is used as the initial entry point for the Dll
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
void initApp();
void unloadApp();
void DoAutoBuild();
void ResizeBay();
void TrapMouse();
void UnTrapMouse();
void DestroyController();
void BOM();
void BOMSummary();
void BOMExtraDisplay();
void JobDetail();
void Create3DView();
void Delete3DView();
void CreateMatrix();
void DeleteMatrix();
void CreateCutThrough();
void ClearCompDetialsFilename();
void ClearStockDetailsFilename();
void ClearDrawingDetailsFilename();
void Overrun();
void EditBay();
void EditRun();
void InsertBay();
void UseActual3DComps();
void DoCleanUp();
void StairBay();
void LadderBay();
void ButtressBay();
void SetStageForSelectedBays();
void RegenMatrixLevels();
void EnterLevels();
void AddLevel();
void UseMills();
void UseKwikstage();
void ChangeSystem();
void SetArcLapSpan();
void Redraw();
void ChangeRLs();
bool TestController();
void AddBracing();
void AddChainLink();
void AddHandrail();
void AddHopups();
void AddLapboard();
void AddShadeCloth();
void AddTie();
void AddTieTube();
void RemoveBracing();
void RemoveChainLink();
void RemoveHandrail();
void RemoveHopups();
void RemoveShadeCloth();
void RemoveTies();
void ToggleLvlLength();
void ToggleFirstScale();
void ToggleScaleLines();
void ClearCommitted();
void MillsSystemCorner();
void SydneyCorner();
void SydneyCornerSeparation();
void EditStandards();
void ShowEntireMatrix();
void HideMatrixSections();
void HideComponentTypes();
void ColourByStageLevel();
void AddCornerStageBoard();
void RemoveCornerStageBoard();
void DisplaySalePrice();
void WallOffsetFromLowestHopup();
void UseToeboardOnLapboard();
void UsePutLogClipsInLadder();
void ConvertToMillsCorner();
void UnconvertMillsCorner();
void AboutKwikscaf();
void TraceContents();
void DeleteSchematic();
void ResetCutThroughSuggest();
void Use1500Soleboards();
void SwapLedgerAndTransoms();
void ShowMatrixSectLabelsOnSch();
void ShowStandardLengths();
void DisplayZigZagLine();
void EditComponent();
void MoveComponent();
void RotateComponent();
void CopyComponent();
void InsertComponent();
void AllComponentsVisible( );
void ShowBaysInBOMS( );
void ShowBaySizesInBOMS( );
void ShowSavedHistory();
void ClearSavedHistory();
void CnvrtSystemOnLoad();
void ShowStockList();
void StoreStockList();
void SetRLs();
void DrawKwikRLs();
void FindRLs();
void KwikRegen();
void EditStock();
void CalcPerRoll();
void TraceVisualComponents();
void ClearActual3DComps();

//void RestricMouse( int iDirection );


BOOL filterMouse(MSG *pMsg);

///////////////////////////////////////////////////////////////////////////////
//forward declaration
class Controller;
class ScaffoldArchive;
class Security;


enum MouseRestrictionEnum
{
	MR_NONE,
	MR_VERTICAL,
	MR_HORIZONTAL,
	MR_ORTHO
};

///////////////////////////////////////////////////////////////////////////////
//global variables
extern Controller		*gpController;
extern ScaffoldArchive	*gpScaffoldArchive;
extern Security			*gpSecurity;

//////////////////////////////////////////////////////////////////////
// Commented by SJ, 26.07.2007, as part of the migration process
// extern HINSTANCE _hdllInstance;
//////////////////////////////////////////////////////////////////////

extern bool gbOpenFileDialogActive;

static bool	gbMouseDone		= false;

///////////////////////////////////////////////////////////////////////////////
//extern "C" AcRx::AppRetCode acrxEntryPoint(AcRx::AppMsgCode msg, void* p);


#endif // !defined(MAIN_H)


/********************************************************************************
 *	History Records
 ********************************************************************************
 * $History: Main.h $
 * 
 * *****************  Version 80  *****************
 * User: Jsb          Date: 4/12/00    Time: 4:32p
 * Updated in $/Meccano/Stage 1/Code
 * Only really have marks ghost after insert bug left
 * 
 * *****************  Version 79  *****************
 * User: Jsb          Date: 27/11/00   Time: 4:06p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 78  *****************
 * User: Jsb          Date: 16/11/00   Time: 3:51p
 * Updated in $/Meccano/Stage 1/Code
 * About to release KwikScaf version 1.1k
 * 
 * *****************  Version 77  *****************
 * User: Jsb          Date: 31/10/00   Time: 4:43p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly go it all working, just need to debug
 * 
 * *****************  Version 76  *****************
 * User: Jsb          Date: 5/10/00    Time: 11:59a
 * Updated in $/Meccano/Stage 1/Code
 * About to build 1.5.1.4 (R1.1d) - This should have the copied matrix
 * move commands sorted out.
 * 
 * *****************  Version 75  *****************
 * User: Jsb          Date: 15/09/00   Time: 3:51p
 * Updated in $/Meccano/Stage 1/Code
 * Still working on the component move, copy and rotate commands
 * 
 * *****************  Version 74  *****************
 * User: Jsb          Date: 14/09/00   Time: 11:38a
 * Updated in $/Meccano/Stage 1/Code
 * I have checked in Release 1.0j and now I am getting going 'forwarding'
 * to the code I was working on this morning which was checked in at 9:40
 * this morning.  I need to check them in so that I can check them back
 * out again and make them writable
 * 
 * *****************  Version 72  *****************
 * User: Jsb          Date: 13/09/00   Time: 4:45p
 * Updated in $/Meccano/Stage 1/Code
 * Building 1.0i
 * 
 * *****************  Version 71  *****************
 * User: Jsb          Date: 8/09/00    Time: 4:35p
 * Updated in $/Meccano/Stage 1/Code
 * Fixed Milo's Bug
 * 
 * *****************  Version 70  *****************
 * User: Jsb          Date: 8/09/00    Time: 12:00p
 * Updated in $/Meccano/Stage 1/Code
 * about to build R1.0g
 * 
 * *****************  Version 69  *****************
 * User: Jsb          Date: 4/09/00    Time: 4:11p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 68  *****************
 * User: Jsb          Date: 31/08/00   Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 67  *****************
 * User: Jsb          Date: 29/08/00   Time: 4:50p
 * Updated in $/Meccano/Stage 1/Code
 * Stage and Level cutoffs are now correct!
 * 
 * *****************  Version 66  *****************
 * User: Jsb          Date: 9/08/00    Time: 10:05a
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8x
 * 
 * *****************  Version 65  *****************
 * User: Jsb          Date: 3/08/00    Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8u
 * 
 * *****************  Version 64  *****************
 * User: Jsb          Date: 2/08/00    Time: 3:35p
 * Updated in $/Meccano/Stage 1/Code
 * About to create RC8t
 * 
 * *****************  Version 63  *****************
 * User: Jsb          Date: 1/08/00    Time: 1:54p
 * Updated in $/Meccano/Stage 1/Code
 * Just created RC8r
 * 
 * *****************  Version 62  *****************
 * User: Jsb          Date: 31/07/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * Labels for the cutthrough finished, 1.5m soleboards finished, save BOMS
 * to csv is completed
 * 
 * *****************  Version 61  *****************
 * User: Jsb          Date: 28/07/00   Time: 4:54p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 60  *****************
 * User: Jsb          Date: 26/07/00   Time: 5:14p
 * Updated in $/Meccano/Stage 1/Code
 * Cutthrough section now working, no labels though
 * 
 * *****************  Version 59  *****************
 * User: Jsb          Date: 21/07/00   Time: 4:21p
 * Updated in $/Meccano/Stage 1/Code
 * Nearly completed the SetDecks to Levels bug
 * 
 * *****************  Version 58  *****************
 * User: Jsb          Date: 20/07/00   Time: 5:04p
 * Updated in $/Meccano/Stage 1/Code
 * Halfway through the positioning of the components
 * 
 * *****************  Version 57  *****************
 * User: Jsb          Date: 3/07/00    Time: 4:22p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 56  *****************
 * User: Jsb          Date: 20/06/00   Time: 5:15p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 55  *****************
 * User: Jsb          Date: 8/06/00    Time: 11:02a
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC8
 * 
 * *****************  Version 53  *****************
 * User: Jsb          Date: 6/06/00    Time: 5:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 52  *****************
 * User: Jsb          Date: 19/05/00   Time: 5:05p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:01p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 51  *****************
 * User: Jsb          Date: 16/05/00   Time: 4:28p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 50  *****************
 * User: Jsb          Date: 12/05/00   Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 49  *****************
 * User: Jsb          Date: 5/05/00    Time: 4:25p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 48  *****************
 * User: Jsb          Date: 19/04/00   Time: 4:52p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 47  *****************
 * User: Jsb          Date: 14/04/00   Time: 5:19p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 46  *****************
 * User: Jsb          Date: 14/04/00   Time: 9:57a
 * Updated in $/Meccano/Stage 1/Code
 * Colour By Stage & level
 * 
 * *****************  Version 45  *****************
 * User: Jsb          Date: 13/04/00   Time: 4:40p
 * Updated in $/Meccano/Stage 1/Code
 * Almost ready for RC5
 * 
 * *****************  Version 44  *****************
 * User: Jsb          Date: 6/04/00    Time: 4:47p
 * Updated in $/Meccano/Stage 1/Code
 * Release Candidate 1.4.4.5 (RC1.4.4fe)
 * 
 * *****************  Version 43  *****************
 * User: Jsb          Date: 15/03/00   Time: 4:18p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 42  *****************
 * User: Jsb          Date: 17/02/00   Time: 2:21p
 * Updated in $/Meccano/Stage 1/Code
 * about to build RC3 (1.4.03)
 * 
 * *****************  Version 41  *****************
 * User: Jsb          Date: 16/02/00   Time: 4:07p
 * Updated in $/Meccano/Stage 1/Code
 * Big Day, just fixed Marks save bug, sydney corners operational
 * 
 * *****************  Version 40  *****************
 * User: Jsb          Date: 9/02/00    Time: 4:57p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 39  *****************
 * User: Jsb          Date: 8/02/00    Time: 3:27p
 * Updated in $/Meccano/Stage 1/Code
 * Building 1.4.00 Release Candidate 1
 * 
 * *****************  Version 38  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 37  *****************
 * User: Jsb          Date: 13/01/00   Time: 4:13p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 36  *****************
 * User: Jsb          Date: 16/12/99   Time: 4:50p
 * Updated in $/Meccano/Stage 1/Code
 * Finished adding function:
 * AddShadeCloth
 * AddHopup
 * AddChainLink
 * AddHandrail
 * plus delete functions for the same!
 * 
 * *****************  Version 35  *****************
 * User: Jsb          Date: 16/12/99   Time: 2:34p
 * Updated in $/Meccano/Stage 1/Code
 * about to add, add and remove handrails and hopups
 * 
 * *****************  Version 34  *****************
 * User: Jsb          Date: 15/12/99   Time: 4:12p
 * Updated in $/Meccano/Stage 1/Code
 * Adjusting RL's nearly finished, just need a dialogbox or other UI to do
 * it!
 * 
 * *****************  Version 33  *****************
 * User: Jsb          Date: 7/12/99    Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * Still fixing problems with the split run function
 * 
 * *****************  Version 32  *****************
 * User: Jsb          Date: 6/12/99    Time: 9:15a
 * Updated in $/Meccano/Stage 1/Code
 * This is the updated code from home
 * 
 * *****************  Version 29  *****************
 * User: Jsb          Date: 18/11/99   Time: 8:12a
 * Updated in $/Meccano/Stage 1/Code
 * Code from the 12-15/11/99
 * 
 * *****************  Version 28  *****************
 * User: Jsb          Date: 11/11/99   Time: 2:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 26  *****************
 * User: Jsb          Date: 2/11/99    Time: 10:57a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 25  *****************
 * User: Jsb          Date: 1/11/99    Time: 1:54p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 24  *****************
 * User: Dar          Date: 26/10/99   Time: 12:54p
 * Updated in $/Meccano/Stage 1/Code
 * fixed bug 301 - requires testing
 * 
 * *****************  Version 23  *****************
 * User: Dar          Date: 19/10/99   Time: 10:59a
 * Updated in $/Meccano/Stage 1/Code
 * added stock.csv file for BOM summary report so hire/sale pricing and
 * weight can be calculated.
 * 
 * *****************  Version 22  *****************
 * User: Jsb          Date: 13/10/99   Time: 2:58p
 * Updated in $/Meccano/Stage 1/Code
 * 1) Standards Fit - Fine fit is now operational, Course and Same require
 * work!
 * 
 * *****************  Version 21  *****************
 * User: Jsb          Date: 12/10/99   Time: 9:32a
 * Updated in $/Meccano/Stage 1/Code
 * 1) Level Assignment now operational - but have to call "RegenLevels"
 * 2) Fixed the Schematic View Serialize error.
 * 
 * *****************  Version 20  *****************
 * User: Jsb          Date: 8/10/99    Time: 3:41p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 19  *****************
 * User: Dar          Date: 8/10/99    Time: 10:23a
 * Updated in $/Meccano/Stage 1/Code
 * added BOMS command
 * 
 * *****************  Version 18  *****************
 * User: Dar          Date: 1/10/99    Time: 2:21p
 * Updated in $/Meccano/Stage 1/Code
 * added security plus more dialog work
 * 
 * *****************  Version 17  *****************
 * User: Jsb          Date: 1/10/99    Time: 12:24p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 16  *****************
 * User: Dar          Date: 9/21/99    Time: 10:19a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 15  *****************
 * User: Jsb          Date: 21/09/99   Time: 9:02a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 14  *****************
 * User: Jsb          Date: 17/09/99   Time: 3:42p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 13  *****************
 * User: Jsb          Date: 17/09/99   Time: 12:14p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 12  *****************
 * User: Jsb          Date: 8/09/99    Time: 2:15p
 * Updated in $/Meccano/Stage 1/Code
 * Matrix nearly working
 * 
 * *****************  Version 11  *****************
 * User: Jsb          Date: 9/03/99    Time: 11:01a
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 10  *****************
 * User: Dar          Date: 9/01/99    Time: 5:10p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 9  *****************
 * User: Jsb          Date: 8/26/99    Time: 3:40p
 * Updated in $/Meccano/Stage 1/Code
 * Hopupbrackets, rails, midrails, toeboards, etc are all now working
 * 
 * *****************  Version 8  *****************
 * User: Jsb          Date: 8/24/99    Time: 5:23p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 7  *****************
 * User: Jsb          Date: 8/20/99    Time: 1:08p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 6  *****************
 * User: Jsb          Date: 8/09/99    Time: 3:34p
 * Updated in $/Meccano/Stage 1/Code
 * 3D View getting better
 * 
 * *****************  Version 5  *****************
 * User: Jsb          Date: 2/08/99    Time: 17:05
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 4  *****************
 * User: Jsb          Date: 29/07/99   Time: 15:56
 * Updated in $/Meccano/Stage 1/Code
 * I will be away friday
 * 
 * *****************  Version 3  *****************
 * User: Jsb          Date: 29/07/99   Time: 10:05
 * Updated in $/Meccano/Stage 1/Code
 * I have now added a test routine to the controller which calls the
 * "LiftList.Test" function
 * I have added operator functions to the Run class
 * 
 * *****************  Version 2  *****************
 * User: Jsb          Date: 15/07/99   Time: 11:43
 * Updated in $/Meccano/Stage 1/Code
 * MFC should now be working via stdafx.h
 * 
 *
 *******************************************************************************/
