#if !defined(AFX_SHOWCOMPONENTTYPEDIALOG_H__24C66EC5_10BD_11D4_9F78_0008C7999B1D__INCLUDED_)
#define AFX_SHOWCOMPONENTTYPEDIALOG_H__24C66EC5_10BD_11D4_9F78_0008C7999B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShowComponentTypeDialog.h : header file
//

#include "ComponentDefinitions.h"
#include "KwikscafDialog.h"
#include "KwikScafButton.h"

//forward declarations
class intArray;

/////////////////////////////////////////////////////////////////////////////
// ShowComponentTypeDialog dialog

class ShowComponentTypeDialog : public KwikscafDialog
{
// Construction
public:
	void SetComponentTypeHideArray( intArray *pArray );
	ShowComponentTypeDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ShowComponentTypeDialog)
	enum { IDD = IDD_SHOW_COMPONENT_TYPES };
	CKwikScafButton	m_btnColourPartialCommitted;
	CKwikScafButton	m_btnColourMatrixRLText;
	CKwikScafButton	m_btnColourMatrixRLSpan;
	CKwikScafButton	m_btnColourMatrixRLMinor;
	CKwikScafButton	m_btnColourMatrixRLMajor;
	CKwikScafButton	m_btnColourMatrixLevelText;
	CKwikScafButton	m_btnColourMatrixLevelLines;
	CKwikScafButton	m_btnColourCrosshairArc;
	CKwikScafButton	m_btnColourCrosshair;
	CKwikScafButton	m_btnColourCommitted;
	CKwikScafButton	m_btnColourBay2400;
	CKwikScafButton	m_btnColourBay1800;
	CKwikScafButton	m_btnColourBay0800;
	CKwikScafButton	m_btnColourBay1200;
	CKwikScafButton	m_btnColourLapboard;
	CKwikScafButton	m_btnColourLadderPutlog;
	CKwikScafButton	m_btnColourLadder;
	CKwikScafButton	m_btnColourTieTubes;
	CKwikScafButton	m_btnColourTransom;
	CKwikScafButton	m_btnColourToeBoard;
	CKwikScafButton	m_btnColourTieClampYoke;
	CKwikScafButton	m_btnColourTieClampMasonary;
	CKwikScafButton	m_btnColourTieClampColumn;
	CKwikScafButton	m_btnColourTieClamp90Deg;
	CKwikScafButton	m_btnColourTieBar;
	CKwikScafButton	m_btnColourStandardsOpenEnd;
	CKwikScafButton	m_btnColourStandards;
	CKwikScafButton	m_btnColourStair;
	CKwikScafButton	m_btnColourStageBoard;
	CKwikScafButton	m_btnColourSoleBoards;
	CKwikScafButton	m_btnColourMidRail;
	CKwikScafButton	m_btnColourShadeCloth;
	CKwikScafButton	m_btnColourRail;
	CKwikScafButton	m_btnColourMeshGuard;
	CKwikScafButton	m_btnColourLedger;
	CKwikScafButton	m_btnColourJacks;
	CKwikScafButton	m_btnColourHopupBrackets;
	CKwikScafButton	m_btnColourDeckingPlank;
	CKwikScafButton	m_btnColourCornerStageBoard;
	CKwikScafButton	m_btnColourChainLink;
	CKwikScafButton	m_btnColourBracing;
	CKwikScafButton	m_btnColourStairRail;
	CKwikScafButton	m_btnColourStairRailStopend;
	CKwikScafButton	m_btnColourStandardConnector;
	CKwikScafButton	m_btnColourToeBoardClip;
	CKwikScafButton	m_btnColourText;
	BOOL	m_bChainLink;
	BOOL	m_bBracing;
	BOOL	m_bCornerStageBoard;
	BOOL	m_bDeckingPlank;
	BOOL	m_bHopupBracket;
	BOOL	m_bJack;
	BOOL	m_bLadder;
	BOOL	m_bLadderPutlog;
	BOOL	m_bLapboard;
	BOOL	m_bLedger;
	BOOL	m_bMeshGuard;
	BOOL	m_bMidRail;
	BOOL	m_bRail;
	BOOL	m_bShadeCloth;
	BOOL	m_bSoleBoards;
	BOOL	m_bStageBoard;
	BOOL	m_bStair;
	BOOL	m_bStairRail;
	BOOL	m_bStairRailStopend;
	BOOL	m_bStandard;
	BOOL	m_bStandardConnector;
	BOOL	m_bStandardOpenend;
	BOOL	m_bText;
	BOOL	m_bTieBar;
	BOOL	m_bTieClamp90Deg;
	BOOL	m_bTieClampColumn;
	BOOL	m_bTieClampMasonary;
	BOOL	m_bTieClampYoke;
	BOOL	m_bTieTube;
	BOOL	m_bToeBoard;
	BOOL	m_bToeBoardClip;
	BOOL	m_bTransom;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ShowComponentTypeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	intArray * m_piaComponentsToHide;

	// Generated message map functions
	//{{AFX_MSG(ShowComponentTypeDialog)
	afx_msg void OnHideAll();
	afx_msg void OnShowAll();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnColourText();
	afx_msg void OnColourStairRail();
	afx_msg void OnColourStairRailStopend();
	afx_msg void OnColourStandardConnector();
	afx_msg void OnColourToeBoardClip();
	afx_msg void OnColourBracing();
	afx_msg void OnColourChainLink();
	afx_msg void OnColourCornerStageBoard();
	afx_msg void OnColourDeckingPlank();
	afx_msg void OnColourHopupBrackets();
	afx_msg void OnColourJacks();
	afx_msg void OnColourLedger();
	afx_msg void OnColourMeshGuard();
	afx_msg void OnColourMidRail();
	afx_msg void OnColourRail();
	afx_msg void OnColourShadeCloth();
	afx_msg void OnColourSoleBoards();
	afx_msg void OnColourStageBoard();
	afx_msg void OnColourStair();
	afx_msg void OnColourStandards();
	afx_msg void OnColourStandardsOpenEnd();
	afx_msg void OnColourTieBar();
	afx_msg void OnColourTieClamp90Deg();
	afx_msg void OnColourTieClampColumn();
	afx_msg void OnColourTieClampMasonary();
	afx_msg void OnColourTieClampYoke();
	afx_msg void OnColourToeBoard();
	afx_msg void OnColourTransom();
	afx_msg void OnColourLadder();
	afx_msg void OnColourLadderPutlog();
	afx_msg void OnColourLapboard();
	afx_msg void OnColourTieTubes();
	afx_msg void OnColourBay0800();
	afx_msg void OnColourBay1200();
	afx_msg void OnColourBay1800();
	afx_msg void OnColourBay2400();
	afx_msg void OnColourCommitted();
	afx_msg void OnColourCrosshair();
	afx_msg void OnColourCrosshairArc();
	afx_msg void OnColourMatrixLevelLines();
	afx_msg void OnColourMatrixLevelText();
	afx_msg void OnColourMatrixRLMajor();
	afx_msg void OnColourMatrixRLMinor();
	afx_msg void OnColourMatrixRLSpan();
	afx_msg void OnColourMatrixRLText();
	afx_msg void OnColourPartialCommitted();
	afx_msg void OnStageLevel();
	afx_msg void OnResetColour();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void FillColours();
	int	m_iColourTransom;
	int	m_iColourToeBoard;
	int	m_iColourTieClampYoke;
	int	m_iColourTieClampMasonary;
	int	m_iColourTieClampColumn;
	int	m_iColourTieClamp90Deg;
	int	m_iColourTieBar;
	int	m_iColourStandardsOpenEnd;
	int	m_iColourStandards;
	int	m_iColourStair;
	int	m_iColourStageBoard;
	int	m_iColourSoleBoards;
	int	m_iColourMidRail;
	int	m_iColourShadeCloth;
	int	m_iColourRail;
	int	m_iColourMeshGuard;
	int	m_iColourLedger;
	int	m_iColourJacks;
	int	m_iColourHopupBrackets;
	int	m_iColourDeckingPlank;
	int	m_iColourCornerStageBoard;
	int	m_iColourChainLink;
	int	m_iColourBracing;
	int	m_iColourStairRail;
	int	m_iColourStairRailStopend;
	int	m_iColourStandardConnector;
	int	m_iColourToeBoardClip;
	int	m_iColourText;
	int	m_iColourLadder;
	int	m_iColourLadderPutlog;
	int	m_iColourLapboard;
	int	m_iColourTieTubes;
	int	m_iColourPartialCommitted;
	int	m_iColourMatrixRLText;
	int	m_iColourMatrixRLSpan;
	int	m_iColourMatrixRLMinor;
	int	m_iColourMatrixRLMajor;
	int	m_iColourMatrixLevelText;
	int	m_iColourMatrixLevelLines;
	int	m_iColourCrosshairArc;
	int	m_iColourCrosshair;
	int	m_iColourCommitted;
	int	m_iColourBay2400;
	int	m_iColourBay1800;
	int	m_iColourBay0800;
	int	m_iColourBay1200;

private:
	void SetAll( BOOL bNewValue );


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOWCOMPONENTTYPEDIALOG_H__24C66EC5_10BD_11D4_9F78_0008C7999B1D__INCLUDED_)
