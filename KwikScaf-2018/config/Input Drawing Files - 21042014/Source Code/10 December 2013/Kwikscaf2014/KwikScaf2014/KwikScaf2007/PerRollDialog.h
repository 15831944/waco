#if !defined(AFX_PERROLLDIALOG_H__425BD138_AB97_11D4_9FF7_000102313BDD__INCLUDED_)
#define AFX_PERROLLDIALOG_H__425BD138_AB97_11D4_9FF7_000102313BDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PerRollDialog.h : header file
//

//Forward Declarations
class Controller;

enum CalculationTypeEnum
{
	CALC_PER_METRE,
	CALC_PER_ROLL
};


/////////////////////////////////////////////////////////////////////////////
// PerRollDialog dialog

class PerRollDialog : public CDialog
{
// Construction
public:
	bool m_bShadeCloth;
	void SetShadeCloth( bool bShadeCloth );
	void FillDialog();
	PerRollDialog( Controller *pController, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(PerRollDialog)
	enum { IDD = IDD_M2_OR_PER_ROLL_DLG };
	CStatic	m_editProductCode;
	CComboBox	m_comboRoundingDevisor;
	CSpinButtonCtrl	m_spinAreaSpinner;
	CEdit	m_editAreaPerRoll;
	CString	m_sAreaPerRoll;
	CString	m_sMainLabel;
	int		m_iCalculationType;
	CString	m_sProductCode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PerRollDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CStringArray		saRoundingTexts;
	Controller			*m_pController;
	CalculationTypeEnum	m_eCalculationType;

	// Generated message map functions
	//{{AFX_MSG(PerRollDialog)
	afx_msg void OnDeltaposAreaSpinner(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCalcPerMetre();
	afx_msg void OnCalcPerRoll();
	afx_msg void OnSelectPartNumber();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void CalcTypeChanged();
	void AdjustControls();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PERROLLDIALOG_H__425BD138_AB97_11D4_9FF7_000102313BDD__INCLUDED_)
