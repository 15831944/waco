//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                              MaterialTypeDefinitions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//	Last Changed By : $Author: Jsb $
//	Last Revision	: $Date: 19/05/00 12:03p $
//  FileName        : /PROJECTS/MECCANO/STAGE 1/CODE/MATERIALTYPEDEFINITIONS.H $
//	Version			: $Revision: 4 $
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Description
//
///////////////////////////////////////////////////////////////////////////////

#ifndef	__MATERIAL_TYPE_DEFINITIONS_H__
#define	__MATERIAL_TYPE_DEFINITIONS_H__


/////////////////////////////////////////////////////////////////////
//Material Type
enum MaterialTypeEnum
{
	MT_STEEL,
	MT_TIMBER,
	MT_OTHER
};

/////////////////////////////////////////////////////////////////////
// GetComponentMaterialStr						INLINE FUNCTION
//
// This function gets the descriptive string for the component material enum arguement.
// The return value is a CString.
inline CString GetComponentMaterialStr( MaterialTypeEnum eMaterialType )
{
	switch( eMaterialType )
	{
		case( MT_STEEL )	: return "Steel";					break;
		case( MT_TIMBER )	: return "Timber";					break;
		case( MT_OTHER )	: return "Other Material";			break;
		default				:
			;//assert( false );
			return "Unknown Material Type";	break;
	};
	;//assert( false );
	return "";
}

#endif	//#ifndef	__MATERIAL_TYPE_DEFINITIONS_H__


/*******************************************************************************
*	History Records
********************************************************************************
* $History: MaterialTypeDefinitions.h $
 * 
 * *****************  Version 4  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:03p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 1  *****************
 * User: Jsb          Date: 19/05/00   Time: 12:01p
 * Created in $/Meccano/Stage 1/Code/Code
 * 
 * *****************  Version 3  *****************
 * User: Jsb          Date: 17/01/00   Time: 4:01p
 * Updated in $/Meccano/Stage 1/Code
 * 
 * *****************  Version 2  *****************
 * User: Jsb          Date: 8/09/99    Time: 2:15p
 * Updated in $/Meccano/Stage 1/Code
 * Matrix nearly working
*
*******************************************************************************/
