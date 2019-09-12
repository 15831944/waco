// StockListElement.h: interface for the StockListElement class.
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

#if !defined(AFX_STOCKLISTELEMENT_H__8AE11507_84F7_11D3_9EC0_00508B043A6C__INCLUDED_)
#define AFX_STOCKLISTELEMENT_H__8AE11507_84F7_11D3_9EC0_00508B043A6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const	double	SLT_INVALID		= -1.00;
const	double	SLT_NOT_FOUND	= -2.00;
const	double	SLT_UNDEFINED	= -3.00;

enum	PathEnum
{
	P_STOCK,	//stock.csv
	P_DRAWING,	//DrawingDetails.csv
	P_ENTITY,	//Path for entity files
};

enum ViewTypeEnum
{
	VT_3D,
	VT_2D,
	VT_2DEO
};


//forward declaration
class	StockListArray;
class	Entity;

class StockListElement  
{
public:
	void DeleteEntity( ViewTypeEnum eVT );
	void ClearActualComponents();
	StockListElement &operator =(const StockListElement &Original);

	bool Import( CStdioFile &file );
	bool Export( CStdioFile &file );
	void SetSSCEdited( bool bEdited );
	bool IsEditedSSC();

	StockListElement();
	virtual ~StockListElement();

	void Serialize(CArchive &ar);
	void TraceContents( CString *pText=NULL );

	void SetEntity( Entity *pEntity, ViewTypeEnum eVT );
	bool LoadEntity( CString &sPath, ViewTypeEnum eVT );
	Entity * GetEntity( ViewTypeEnum eVT );
	Entity * GetClonedEntity( ViewTypeEnum eVT );

	bool UseSalePrice();
	bool GetUserCreatedElement();
	CString GetPartNumber();
	CString GetDescription();
	double	GetHire();
	double  GetSale();
	double  GetWeight();
	double	GetLength();
	double	GetWidth();
	double	GetHeight();
	CString GetFilename2DEO();
	CString GetFilename2D();
	CString	GetFilename3D();

	void SetUseSalePrice( bool bUseSale );
	void SetUserCreatedElement(bool bUse);
	void SetPartNumber(		CString sPartNumber );
	void SetDescription(	CString sDesc );
	void SetHire(	double	dHire );
	void SetSale(	double  dSale );
	void SetWeight(	double  dWeight );
	void SetLength(	double	dLength );
	void SetWidth(	double	dWidth );
	void SetHeight(	double	dHeight );
	void SetFilename2DEO(	CString sFilename );
	void SetFilename2D(		CString sFilename );
	void SetFilename3D(		CString	sFilename );

protected:
	bool	m_bSSCEdited;
	bool	m_bUserCreatedElement;
	bool	m_bUseSalePrice;
	CString m_sPartNumber;
	CString m_sDescription;
	double	m_dHire;
	double  m_dSale;
	double  m_dWeight;
	double	m_dX;
	double	m_dY;
	double	m_dZ;
	CString m_sFilename2DEO;
	CString m_sFilename2D;
	CString	m_sFilename3D;

	Entity *m_pEntity3D;
	Entity *m_pEntity2D;
	Entity *m_pEntity2DEO;

private:
	AcDbObjectId addBlockWithAttributes(AcDbObjectId blockId, AcGePoint3d basePoint);
	CString	GetEntPathInRegistry(PathEnum eP);
	void	SetEntPathInRegistry(LPCTSTR strFileName, PathEnum eP);
};

#include <afxtempl.h>

class StockListArray: public CArray<StockListElement*, StockListElement*>
{
public:
	void ClearActualComponents();
	StockListElement * GetUserEditedSSC(int iIndex);
	int GetNumberOfEditedSSC();
	StockListArray();
	virtual ~StockListArray();

	int m_iVSLMajor, m_iVSLMinor, m_iVSLBuild;
	int m_iVVCMajor, m_iVVCMinor, m_iVVCBuild;

	//user created elements
	StockListElement *GetUserCreatedElement( int iIndex );
	int  GetNumberOfUserCreatedElements();

	void Serialize(CArchive &ar);

	bool GetUseSalePrice( CString sPartNumber, bool &bUseSale );
	int  EditStockElement();
	void TraceContents( bool bShowList );
	void RemoveAll();
	void ClearFilenameInRegistry( bool bStockFile );

	bool LoadList();
	StockListElement *GetMatchingComponent(CString sPartNumber, bool bShowWarning=true );

	bool GetDescription(CString sPartNumber, CString &sDesc );
	bool GetHire(CString sPartNumber, double &dHire );
	bool GetSale(CString sPartNumber, double &dSale);
	bool GetWeight(CString sPartNumber, double &dWeight);
	bool GetLength(CString sPartNumber, double &dLength);
	bool GetWidth(CString sPartNumber, double &dWidth);
	bool GetHeight(CString sPartNumber, double &dHeight);
	bool GetFilename3D(CString sPartNumber, CString &sFilename );
	bool GetFilename2D(CString sPartNumber, CString &sFilename );
	bool GetFilename2DEO(CString sPartNumber, CString &sFilename );

protected:
	CString m_sLastPartNumber;
	// Changed LPCTSTR to CString, ~BSK, 21.02.2018
	void	SetFilenameInRegistry(CString strFileName, bool bStockFile);
	CString  GetFilenameInRegistry(bool bStockFile);
	bool	GetFile( CStdioFile &File, bool bStockFile );
	double GetNextDoubleAndMoveOn(CString &sLine);
};

#endif // !defined(AFX_STOCKLISTELEMENT_H__8AE11507_84F7_11D3_9EC0_00508B043A6C__INCLUDED_)
