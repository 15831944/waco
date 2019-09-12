//	© Waco Kwikform Limited
//	ACN 002 835 36
//	P.O. Box 15 Rydalmere NSW 2116
//
//	All rights reserved. No part of this work covered by copyright
//	may be reproduced or copied in anyform or by any means (graphic,
//	electronic or mechanical, including photocopying, recording,
//	recording taping or information retrieval system) without the
//	written permission of Waco Kwikform Limited.
#if !defined SERIALIZATION
#define SERIALIZATION

//********************************************************************************
//********************************************************************************
//									DEFINITIONS HEADER
//********************************************************************************
//********************************************************************************



// SERIALIZATION DEFINES
// MACROS BELOW For serialization conversion of bool type

//#define DEBUG_SERIALIZATION_TO_FILE


#if defined DEBUG_SERIALIZATION_TO_FILE
// IF DEBUG MODE

// global variable
//CString  debugCString;
//CArchive *debugr;

//#define OUTBUG(sformat, svariable) debugCString.Format(sformat, svariable); debugr << debugCString << "\n";

#else
// IF NOT DEBUG MODE

//#define OUTBUG(sformat, svariable) ;

// requies BOOL BTemp to be declared locally
#define STORE_bool_IN_AR(boolVar) BTemp = (boolVar)? TRUE : FALSE; ar << BTemp;		
// requies BOOL BTemp to be declared locally
#define LOAD_bool_IN_AR(boolVar) ar >> BTemp; boolVar = (BTemp) ? true : false;		

#endif DEBUG_SERIALIZATION_TO_FILE



#endif MECCANODEFINITIONS
