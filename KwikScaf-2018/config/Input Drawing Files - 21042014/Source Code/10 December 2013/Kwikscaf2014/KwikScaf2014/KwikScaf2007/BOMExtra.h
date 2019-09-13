//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
#if !defined(AFX_BOMEXTRA_H__E0E97FE3_6CA5_11D3_9EB1_00508B043A6C__INCLUDED_)
#define AFX_BOMEXTRA_H__E0E97FE3_6CA5_11D3_9EB1_00508B043A6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BOMExtra.h : header file
//
#include "meccano.h"
#include "MeccanoDefinitions.h"

class ComponentDetailsArray;
class ComponentList;

class BOMExtraElement
{
public:
	SystemEnum m_eSystem;
	CString GetBOMPNumDescript();
	CString GetBOMDescript();
	CString GetBOMQuantityDescript();
	CString GetBOMPartNumberDescription();
	CString GetCompDetailsPartNumberDescription();
	CString GetBOMDescription();
	CString GetCompDetailsDescription();

	BOMExtraElement &operator=( const BOMExtraElement &Original);
	bool Compare(BOMExtraElement *pBOMExtraElement);
	BOMExtraElement();
	BOMExtraElement(	ComponentTypeEnum	eComponentType,
						MaterialTypeEnum	eMaterialType,
						double				dLength,
						CString				sName,
						CString				sStage,
						int					iLevel,
						int					iQuantity,
						SystemEnum			eSystem );

	ComponentTypeEnum	m_eComponentType;
	MaterialTypeEnum	m_eMaterialType;
	double				m_dLength;
	CString				m_sName;
	CString				m_sStage;
	int					m_iLevel;
	CString				m_sPartNumber;
	int					m_iQuantity;
	double				m_dArea;
	
protected:

};


#if defined(_DEBUG) 
  #define _DEBUG_WAS_DEFINED
   #undef _DEBUG 
#endif  

#include <afxtempl.h>

#ifdef _DEBUG_WAS_DEFINED
  #define _DEBUG    
  #undef _DEBUG_WAS_DEFINED 
#endif
class BOMExtraArray : public CArray<BOMExtraElement*, BOMExtraElement*>
{
	
public:
	void RemoveAll();
	BOMExtraArray &operator=( const BOMExtraArray &Original );
	void TracePartNos();
	void Sort( bool bIncremental=true );
};

/////////////////////////////////////////////////////////////////////////////
// BOMExtra dialog

#include "KwikscafDialog.h"
	
class BOMExtra : public KwikscafDialog
{
// Construction
public:
	void DestroyBOMDetailsArray();
	int GetLevelAsInt();
	virtual  ~BOMExtra();
	BOMExtra(CWnd* pParent = NULL, ComponentDetailsArray *pComponentDetailsList = NULL, ComponentList *pBOMExtraCompList = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(BOMExtra)
	enum { IDD = IDD_BOM_EXTRA_DIALOG };
	CButton	m_btnFilter;
	CListCtrl	m_ExtraComponentsListControl;
	CComboBox	m_cmboLevel;
	CComboBox	m_cmboStage;
	CEdit	m_ctrlQuantity;
	CButton	m_BOMRemoveButtonCtrl;
	CButton	m_BOMAddButtonCtrl;
	CListBox	m_ComponentListBoxCtrl;
	CString	m_sStage;
	CString	m_sQuantity;
	int		m_iLevel;
	CString	m_sFilterString;
	CString	m_sMatchingElements;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BOMExtra)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bInitialised;
	void ConvertDialogListToBOMCompList();
	void ConvertBOMCompListToDialogList();
	void AddBOMSelection(BOMExtraElement *pBOMExtraElement, bool bInitializeList = false);
	bool m_bSortStageLevel;
	bool IsComponentDetailInList();
	void AddBOMExtraList();
	void AddComponentDetailsList(bool bApplyingFilter);

	ComponentDetailsArray	*m_pComponentDetailsList;
	ComponentList			*m_pBOMExtraCompList;

	BOMExtraArray m_BOMDetailsArray;
	BOMExtraArray m_BOMExtraArray;

	// Generated message map functions
	//{{AFX_MSG(BOMExtra)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnRemoveButton();
	afx_msg void OnChangeQuantityEdit();
	virtual void OnOK();
	afx_msg void OnEditchangeStageCombo();
	afx_msg void OnDeltaposQuantitySpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditupdateStageCombo();
	afx_msg void OnKillfocusStageCombo();
	afx_msg void OnRemoveallDone();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnDblclkComponentListbox();
	afx_msg void OnFilter();
	afx_msg void OnDblclkExtraComponentsListcontrol(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusFilterString();
	afx_msg void OnSelchangeLevelCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool IsInBOMList( LPCTSTR strPartNumber );
	void DestroyBOMExtraArray();
	CString GetStageAsString();
	void FillStageList();
	void FillLevelList();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BOMEXTRA_H__E0E97FE3_6CA5_11D3_9EB1_00508B043A6C__INCLUDED_)
