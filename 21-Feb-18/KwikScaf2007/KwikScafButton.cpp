// KwikScafButton.cpp : implementation file
//

#include "stdafx.h"
#include "meccano.h"
#include "KwikScafButton.h"
#include "ColourCode.h"
#include <dbcolor.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKwikScafButton

CKwikScafButton::CKwikScafButton()
{
	m_iColour = 0;
}

CKwikScafButton::~CKwikScafButton()
{
}


BEGIN_MESSAGE_MAP(CKwikScafButton, CButton)
	//{{AFX_MSG_MAP(CKwikScafButton)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKwikScafButton message handlers

void CKwikScafButton::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if( m_iColour==COLOUR_MULTISELECT )
	{
		CRect		Rect( 0, 0, 3, 15 );
		CBrush		Brush;
		COLORREF	cRef;

		cRef = COLOUR_RGB_RED;
		Brush.CreateSolidBrush( cRef );
		dc.FillRect( Rect, &Brush );

		Rect.SetRect( 3, 0, 6, 15 );
		cRef = COLOUR_RGB_BLUE;
		Brush.CreateSolidBrush( cRef );
		dc.FillRect( Rect, &Brush );

		Rect.SetRect( 6, 0, 9, 15 );
		cRef = COLOUR_RGB_GREEN;
		Brush.CreateSolidBrush( cRef );
		dc.FillRect( Rect, &Brush );

		Rect.SetRect( 9, 0, 12, 15 );
		cRef = COLOUR_RGB_BLACK;
		Brush.CreateSolidBrush( cRef );
		dc.FillRect( Rect, &Brush );

		Rect.SetRect( 12, 0, 15, 15 );
		cRef = COLOUR_RGB_YELLOW;
		Brush.CreateSolidBrush( cRef );
		dc.FillRect( Rect, &Brush );
	}
	else
	{
		CRect Rect( 0, 0, 15, 15 );
		COLORREF cRef, cWhite, cDkGrey;
		cRef = (COLORREF)m_iColour;

		cWhite	= COLOUR_RGB_WHITE;
		cDkGrey = COLOUR_RGB_GREY_DARK;
		CBrush Brush;
		Brush.CreateSolidBrush( cRef );
		dc.FillRect( Rect, &Brush );
		dc.Draw3dRect( Rect, cWhite, cDkGrey );
	}
}

void CKwikScafButton::SetAutoCADColour(int iColour)
{
	if( iColour<0 )
	{
		m_iColour = iColour;
	}
	else
	{
		int iRGBColour;
		AcCmEntityColor	clr;
		iRGBColour = clr.lookUpRGB( iColour );
		SetRGBColour( iRGBColour );
	}
}

void CKwikScafButton::SetRGBColour(int iColour)
{
	//Either Brush or FillRect are using BGR not RGB
	int iRed, iGreen, iBlue;
	iRed	= iColour&COLOUR_RGB_BLUE;
	iGreen	= iColour&COLOUR_RGB_GREEN;
	iBlue	= iColour&COLOUR_RGB_RED;
	iRed	= iRed>>16;
	iBlue	= iBlue<<16;
	m_iColour = iRed+iGreen+iBlue;
}

void CKwikScafButton::SetRGBColour(int iRed, int iGreen, int iBlue)
{
	int iClr;
	iClr = iBlue;
	iClr = iClr<<16;
	iClr+= iGreen;
	iClr = iClr<<16;
	iClr+= iRed;
	SetRGBColour( iClr );
}

int CKwikScafButton::GetAutoCADColour()
{
	if( m_iColour<0 )
	{
		return m_iColour;
	}
	else
	{
		int				iColour, iRed, iGreen, iBlue;
		AcCmEntityColor	clr;

		GetRGBColour( iRed, iGreen, iBlue );
		iColour = clr.lookUpACI( iRed, iGreen, iBlue );
		return iColour;
	}
}

void CKwikScafButton::GetRGBColour(int &iRed, int &iGreen, int &iBlue)
{
	iRed	= m_iColour&COLOUR_RGB_RED;
	iGreen	= m_iColour&COLOUR_RGB_GREEN;
	iBlue	= m_iColour&COLOUR_RGB_BLUE;
	iGreen	= iGreen>>8;
	iBlue	= iBlue>>16;
}

int CKwikScafButton::GetRGBColour()
{
	int iColour, iRed, iGreen, iBlue;
	GetRGBColour( iRed, iGreen, iBlue );
	iColour = iRed<<16;
	iColour+= iGreen<<8;
	iColour+= iBlue;
	return iColour;
}
