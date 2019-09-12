//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
#if !defined(AFX_BOMSUMMARYDIALOG_H__CA3E3FA4_7ADF_11D3_9EB8_00508B043A6C__INCLUDED_)
#define AFX_BOMSUMMARYDIALOG_H__CA3E3FA4_7ADF_11D3_9EB8_00508B043A6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BOMSummaryDialog.h : header file
//
#include "BOMExtra.h"
#include "meccano.h"
#include "MeccanoDefinitions.h"

//forward declarations
class SummaryInfo;
class ComponentDetailsArray;
class StockListArray;
class Component;
class SummaryElement;
class Bay;

//#include <afxtempl.h>

#if defined(_DEBUG) 
  #define _DEBUG_WAS_DEFINED
   #undef _DEBUG 
#endif  

#include <afxtempl.h>

#ifdef _DEBUG_WAS_DEFINED
  #define _DEBUG    
  #undef _DEBUG_WAS_DEFINED 
#endif

#include "doubleArray.h"
#include "KwikscafDialog.h"

/////////////////////////////////////////////////////////////////////////////
// BOMSummaryDialog dialog
typedef	CArray<Component*, Component*>  CompArray;

//forward declaration
//class	BOMExtraArray;

enum BOMShowType
{
	BOMSHOW_HIRE_ONLY,
	BOMSHOW_SALE_ONLY,
	BOMSHOW_SALE_AND_HIRE
};


class BOMSummaryDialog : public KwikscafDialog
{
// Construction
public:
	CStringArray m_saCSVData;
	void ShowCommitButtions();
	void ResetStageAndLevelToDefault();
	void CheckStandardsAndTransoms();
	void DisplayJobDetails( int &iCurrentListCtrlIndex );
	virtual  ~BOMSummaryDialog();
	BOMSummaryDialog(CWnd* pParent = NULL, CListBox *pSelectedListBox = NULL, SummaryInfo *pSummaryInfo = NULL, bool bAmalgamate=false);   // standard constructor
 
// Dialog Data
	//{{AFX_DATA(BOMSummaryDialog)
	enum { IDD = IDD_BOM_SUMMARY_DIALOG };
	CButton	m_btnCommit;
	CButton	m_btnUncommit;
	CButton	m_BOMExtraButton;
	CListBox	m_StockTestListBoxCtrl;
	CListCtrl	m_BOMSummaryNewListCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BOMSummaryDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool					m_bShowCaret;
	doubleArray				m_daQuantityTotals;
	doubleArray				m_daWeightTotals;
	doubleArray				m_daHireTotals;
	doubleArray				m_daSaleTotals;
	doubleArray				m_daSaleQuantityTotals;
	doubleArray				m_daSaleWeightTotals;
	int						 iCurrentListCtrlIndex;
	CListBox				*m_pSelectedListBox;
	SummaryInfo				*m_pSummaryInfo;
	ComponentDetailsArray	*m_pComponentDetailsArray;
	StockListArray			*m_pStockListArray;
	CArray<Component*, Component*>	m_CompsChanged;

	void LoadList();
	void MakeListOfComponents(intArray iaSIIndex, bool bShowGrandTotal=false );
	// Generated message map functions
	//{{AFX_MSG(BOMSummaryDialog)
	afx_msg void OnPrintButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnBomExtra();
	afx_msg void OnCommit();
	afx_msg void OnClose();
	afx_msg void OnUncommit();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSaveReport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool SeperateOutSaleOnlyItems( BOMExtraArray *pOriginal, BOMExtraArray *pSaleOnly );
	void DispalyGrandTotals( int &iListIndex );
	void DisplayList( BOMExtraArray *pBOMArray, int &iListIndex, BOMShowType show );
	bool GetFile(CStdioFile &File);
	bool m_bAmalgamate;
	void ReStageMatchingComp(Component *pComponent, SummaryElement *pSummaryElement,
					CompArray *pCompList, ComponentTypeEnum eType, SideOfBayEnum eSide,
					ComponentTypeEnum eTypeMatch, SideOfBayEnum eSideMatch, int iRise,
					Bay *pBayNeighbor, SideOfBayEnum eSideOwned );
	void ReStageStandards(Component *pComponent, SummaryElement *pSummaryElement,
					CompArray *pCompList, Bay *pBayNeighbor, SideOfBayEnum eSideStandard1,
					SideOfBayEnum eSideRail1, SideOfBayEnum eSideRail2,
					SideOfBayEnum eSideLedger );
	void AddBOMComponentToList( BOMExtraArray &BOMDetailsArray, CString sPartNumber, double dLength, ComponentTypeEnum eType, MaterialTypeEnum eMT, SystemEnum eSystem, int iQnty=1, double dArea=0.00 );
	int GetCurrentNumberOfDecks(Bay *bay,int dL,int dU,double &rise);
	int BOMSummaryDialog::GetCurrentNumberOfDecks(Bay *pBay);
public:
	afx_msg void OnLvnItemchangedBomSummaryListctrl(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BOMSUMMARYDIALOG_H__CA3E3FA4_7ADF_11D3_9EB8_00508B043A6C__INCLUDED_)
