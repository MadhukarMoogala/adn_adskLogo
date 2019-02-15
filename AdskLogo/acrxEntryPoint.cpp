// (C) Copyright 2002-2012 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "DbxUtils.h"
#include "dbxLogoStyle.h"
#include "DbxLogo.h"
#include "eoktest.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("ADSK")




//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CAdskLogoApp : public AcRxArxApp {

public:
	CAdskLogoApp () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}
		
	static void ADSKMyGroupADSKLogo () {
		AcDbObjectId styleId;
		if (!getLogoStyle(styleId))
			return;
		CString prompt;
		prompt.Format(_T("\n%s: "), _T("Location"));
		int result;
		ads_point adsPt;
		result = acedGetPoint(NULL, prompt, adsPt);
		if (RTNORM != result) return;
		AcGePoint3d locationPt = asPnt3d(adsPt);

		TCHAR defStr[512];
				// set up prompt
		acdbRToS(1.00, -1, -1, defStr);
		double distanceValue;
		prompt.Format(_T("\n%s<%s>: "), _T("\nScale"), defStr);
		result = acedGetDist(asDblArray(locationPt), prompt, &distanceValue);	
		if (RTNORM != result) return;
		ads_name ent;
		AcDbObjectId tmpId;
		AcDbEntity* tmpEnt;
		Acad::ErrorStatus es;
		prompt.Format(_T("\n%s: "), _T("Select an arbitrary entity to reference (or RETURN for NONE)"));
		result = acedEntSel(prompt, ent, adsPt);
		if (RTNORM != result) return;

		if (result == RTNORM) {
			DbxUtils::enameToObjId(ent, tmpId);
			es = acdbOpenAcDbEntity(tmpEnt, tmpId, AcDb::kForRead);
			if (!eOkVerify(es)) return;
			ArxDbgDbAdeskLogo* logo = new ArxDbgDbAdeskLogo;
			logo->setLocation(locationPt);
			if (styleId != AcDbObjectId::kNull)
				logo->setLogoStyleId(styleId);

			logo->setScale(distanceValue);

			if (tmpId.isNull() == false)
				logo->setArbitraryReferenceId(tmpId);

			DbxUtils::transformToWcs(logo, acdbHostApplicationServices()->workingDatabase());
			DbxUtils::addToCurrentSpaceAndClose(logo);
		}
	
	}
	static bool getLogoStyle(AcDbObjectId& styleId)
	{
		// try to get the standard style, which will hardwire it if it doesn't
		// yet exist
		AcDbObjectId standardStyleId =
			ArxDbgDbAdeskLogoStyle::getStandardStyle(acdbHostApplicationServices()->workingDatabase(), true);

		SdStrObjIdList logoStyles;
		AcDbObjectId logoStyleDictId = DbxUtils::getRegularDictionaryId(
			ArxDbgDbAdeskLogoStyle::dictName(), true, acdbHostApplicationServices()->workingDatabase());

		DbxUtils::collectDictionaryEntryNames(logoStyleDictId, logoStyles, ArxDbgDbAdeskLogoStyle::desc());

		if (logoStyles.Count() == 0) {
			acedAlert(_T("There are no Adesk Logo Styles to choose from."));
			return false;
		}

		/*ArxDbgUiDlgGenericDefId dbox(acedGetAcadDwgView(),
			logoStyles, styleId, false);
		dbox.setDboxTitle(_T("Autodesk Logo Styles"));

		if (dbox.DoModal() == IDOK) {
			styleId = dbox.getSelectedId();
			return true;
		}*/

		CString str;
		AcDbObjectId id;
		if (logoStyles.GetAtIndex(0, str, id)) {
			styleId = id;
			return true;
		}

		return false;
	}
	
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CAdskLogoApp)

ACED_ARXCOMMAND_ENTRY_AUTO(CAdskLogoApp, ADSKMyGroup, ADSKLogo, ADSKLogo, ACRX_CMD_MODAL, NULL)


