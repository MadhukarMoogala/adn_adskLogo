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
//- StdAfx.h : include file for standard system include files,
//-      or project specific include files that are used frequently,
//-      but are changed infrequently
//-----------------------------------------------------------------------------
#pragma once
#define ADSKLOGO_MODULE
#ifndef _ALLOW_RTCc_IN_STL
#define _ALLOW_RTCc_IN_STL
#endif
#pragma pack (push, 8)
#pragma warning(disable: 4786 4996)
//#pragma warning(disable: 4098)

//-----------------------------------------------------------------------------
#define STRICT

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN			//- Exclude rarely-used stuff from Windows headers
#endif

//- Modify the following defines if you have to target a platform prior to the ones specified below.
//- Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER							//- Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0601			//- Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif


//- ObjectARX and OMF headers needs this
#include <map>

//-----------------------------------------------------------------------------
#include <afxwin.h>				//- MFC core and standard components
#include <afxext.h>				//- MFC extensions
#include <afxcmn.h>				//- MFC support for Windows Common Controls

//-----------------------------------------------------------------------------
//- Include ObjectDBX/ObjectARX headers
//- Uncomment one of the following lines to bring a given library in your project.
//#define _BREP_SUPPORT_					//- Support for the BRep API
//#define _HLR_SUPPORT_						//- Support for the Hidden Line Removal API
//#define _AMODELER_SUPPORT_				//- Support for the AModeler API
//#define _ASE_SUPPORT_							//- Support for the ASI/ASE API
//#define _RENDER_SUPPORT_					//- Support for the AutoCAD Render API
//#define _ARX_CUSTOM_DRAG_N_DROP_	//- Support for the ObjectARX Drag'n Drop API
//#define _INC_LEAGACY_HEADERS_			//- Include legacy headers in this project
#include "arxHeaders.h"
#include <Ac64BitHelpers.h>



//-----------------------------------------------------------------------------
#include "DocData.h" //- Your document specific data class holder

//- Declare it as an extern here so that it becomes available in all modules
extern AcApDataManager<CDocData> DocVars ;



/****************************************************************************
**
**    CLASS SdStrObjIdNode:
**
**    **jma
**
*************************************/

class SdStrObjIdNode {

	friend class SdStrObjIdList;
	friend class SdStrObjIdListIterator;

private:
	// constructors/destructors
	SdStrObjIdNode(LPCTSTR name, const AcDbObjectId& objId);

	// data members
	CString            m_name;
	AcDbObjectId    m_objid;

	// outlawed functions
	SdStrObjIdNode(const SdStrObjIdNode& copy);
	SdStrObjIdNode&  operator=(const SdStrObjIdNode& copy);
};

/****************************************************************************
**
**    CLASS SdStrObjIdList:
**
**    **jma
**
*************************************/

class SdStrObjIdList {

	friend class SdStrObjIdListIterator;    // allow direct access to underlying list

public:
	// constructors/destructors
	SdStrObjIdList(BOOL caseSensitive = TRUE);
	virtual    ~SdStrObjIdList();

	// operations
	int Count() const {
		// 64Bit: I don't think we'll ever need THAT many (2 billion) 
		// objects, especially in samples (this class isn't used any-
		// where else). Keep it as it is for now until I hear shouting.
		return AcIntPtrToInt(m_list.GetCount());
	}
	virtual POSITION    AddHead(LPCTSTR str, const AcDbObjectId& objId);
	virtual POSITION    AddTail(LPCTSTR str, const AcDbObjectId& objId);
	virtual POSITION    AddAlpha(LPCTSTR str, const AcDbObjectId& objId);
	virtual BOOL        GetAtIndex(int index, CString& str, AcDbObjectId& objId) const;
	virtual BOOL        GetIndexFor(const CString& str, int& index) const;
	virtual BOOL        GetIndexFor(const AcDbObjectId& objId, int& index) const;
	virtual BOOL        GetMatchFor(const AcDbObjectId& objId, CString& str) const;
	virtual BOOL        GetMatchFor(const CString& str, AcDbObjectId& objId) const;
	virtual BOOL        Rename(const CString& oldName, const CString& newName);
	virtual void        RemoveAt(int index);
	virtual void        RemoveAll();

private:
	// data members
	CPtrList    m_list;
	BOOL        m_caseSensitive;


	// outlawed functions
	SdStrObjIdList(const SdStrObjIdList&);
	SdStrObjIdList&    operator=(const SdStrObjIdList&);
};

/****************************************************************************
**
**    CLASS SdStrObjIdListIterator:
**
**    **jma
**
*************************************/

class SdStrObjIdListIterator {

public:
	// constructors/destructor
	SdStrObjIdListIterator(const SdStrObjIdList& list);
	virtual         ~SdStrObjIdListIterator() {};

	virtual void    toFirst() { m_pos = m_list.GetHeadPosition(); }
	virtual void    toLast() { m_pos = m_list.GetTailPosition(); }
	virtual void    previous() { m_list.GetPrev(m_pos); }
	virtual void    next() { m_list.GetNext(m_pos); }
	virtual void    item(CString& name, AcDbObjectId& objId) const;
	virtual BOOL    done() const { return(m_pos == NULL); }

private:
	// data members
	POSITION                m_pos;
	const CPtrList&         m_list;

	// outlawed functions    (for now)
	SdStrObjIdListIterator(const SdStrObjIdListIterator&);
	SdStrObjIdListIterator&    operator=(const SdStrObjIdListIterator&);
};





#pragma pack (pop)

