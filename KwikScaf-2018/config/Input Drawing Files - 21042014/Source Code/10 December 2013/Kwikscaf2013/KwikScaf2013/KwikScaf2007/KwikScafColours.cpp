// KwikScafColours.cpp: implementation of the CKwikScafColours class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "meccano.h"
#include "KwikScafColours.h"
#include "ColourCode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int COLOUR_DECKING_PLANK		= 20;
int COLOUR_STAGE_BOARD			= COLOUR_DECKING_PLANK;
int COLOUR_LAPBOARD				= COLOUR_DECKING_PLANK;
int COLOUR_LEDGER				= 40;
int COLOUR_TRANSOM				= 50;
int COLOUR_MESH_GUARD			= 60;
int COLOUR_RAIL					= 70;
int COLOUR_MID_RAIL				= 80;
int COLOUR_TOE_BOARD			= 90;
int COLOUR_CHAIN_LINK			= COLOUR_WHITE;
int COLOUR_SHADE_CLOTH			= COLOUR_GREY;
int COLOUR_CHAIN_AND_SHADE		= COLOUR_YELLOW;
int COLOUR_TIE_TUBE				= 120;
int COLOUR_TIE_CLAMP_COLUMN		= 130;
int COLOUR_TIE_CLAMP_MASONARY	= 131;
int COLOUR_TIE_CLAMP_YOKE		= 132;
int COLOUR_TIE_CLAMP_90DEGREE	= 140;
int COLOUR_BRACING				= 150;
int COLOUR_STAIR				= 160;
int COLOUR_LADDER				= 170;
int COLOUR_STANDARD				= 180;
int COLOUR_JACK					= 190;
int COLOUR_SOLEBOARD			= 200;
int COLOUR_HOPUP_BRACKET		= 210;
int COLOUR_CORNER_STAGE_BOARD	= 220;
int COLOUR_TEXT					= 230;
int COLOUR_TIE_BAR				= 240;
int COLOUR_STANDARD_CONNECTOR	= 250;
int COLOUR_LADDER_PUTLOG		= 10;
int COLOUR_STAIR_RAIL			= 20;
int COLOUR_STAIR_RAIL_STOPEND	= 30;
int COLOUR_TOE_BOARD_CLIP		= 40;
int COLOUR_BOM_EXTRA			= 50;
int COLOUR_PUTLOG_CLIP			= 60;
int COLOUR_VISUAL_COMPONENT		= 70;


int COLOUR_CROSSHAIR				= COLOUR_YELLOW;
int COLOUR_CROSSHAIR_DEBUG			= COLOUR_RED;
int COLOUR_CROSSHAIR_ARC			= COLOUR_YELLOW;
int COLOUR_CROSSHAIR_3D_VIEW		= COLOUR_YELLOW;
int COLOUR_CROSSHAIR_MATRIX			= COLOUR_GREY;
int COLOUR_MATRIX_RL_TEXT			= COLOUR_YELLOW;
int COLOUR_MATRIX_RL_LINES_MINOR	= COLOUR_RED;
int COLOUR_MATRIX_RL_LINES_MAJOR	= COLOUR_BLUE;
int COLOUR_MATRIX_RL_LINES_SPAN		= COLOUR_YELLOW;
int COLOUR_MATRIX_LEVELS_TEXT		= COLOUR_GREEN;
int COLOUR_MATRIX_LEVELS_LINES		= COLOUR_DARK_GREY;

int COLOUR_0700		= COLOUR_CYAN;		//0.7m long = Cyan,
int COLOUR_1200		= COLOUR_YELLOW;	//1.2m long = Yellow,
int COLOUR_1800		= COLOUR_GREEN;		//1.8m long = Green,
int COLOUR_2400		= COLOUR_RED;		//2.4m long = Red.

int COLOUR_COMMITTED_FULLY		= COLOUR_WHITE;
int COLOUR_COMMITTED_PARTIALLY	= COLOUR_DARK_GREY;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CKwikScafColours::CKwikScafColours()
{

}

CKwikScafColours::~CKwikScafColours()
{

}
