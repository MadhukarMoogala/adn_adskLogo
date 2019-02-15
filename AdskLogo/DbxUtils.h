#ifndef DBX_UTILS_H
#define DBX_UTILS_H
#include "StdAfx.h"

//Utils:
struct DbxUtils
{
	static void rxErrorAlert(Acad::ErrorStatus msgId)
	{
		CString str;

		if (msgId != Acad::eOk) {
			str.Format(_T("ARX ERROR: %s"), acadErrorStatusText(msgId));
			acedAlert(str);
		}
	}
	static void	rxErrorMsg(Acad::ErrorStatus msgId)
	{
		if (msgId != Acad::eOk)
			acutPrintf(_T("\nARX ERROR: %s"), acadErrorStatusText(msgId));
	}
	static const TCHAR*	rxErrorStr(Acad::ErrorStatus msgId)
	{
		return acadErrorStatusText(msgId);
	}
	static void getEcsToWcsMatrix(const AcGePoint3d& origin,
		const AcGeVector3d& zAxis, AcGeMatrix3d& mat)
	{
		const double kArbBound = 0.015625;  //  1/64th

			// short circuit if in WCS already
		if (zAxis == AcGeVector3d::kZAxis) {
			mat.setToIdentity();
			return;
		}

		AcGeVector3d xAxis, yAxis;

		ASSERT(zAxis.isUnitLength());

		if ((fabs(zAxis.x) < kArbBound) && (fabs(zAxis.y) < kArbBound))
			xAxis = AcGeVector3d::kYAxis.crossProduct(zAxis);
		else
			xAxis = AcGeVector3d::kZAxis.crossProduct(zAxis);

		xAxis.normalize();
		yAxis = zAxis.crossProduct(xAxis);
		yAxis.normalize();

		mat.setCoordSystem(AcGePoint3d::kOrigin, xAxis, yAxis, zAxis);
	}
	static void getEcsXAxis(const AcGeVector3d& ecsZAxis, AcGeVector3d& xAxis) {
		AcGeMatrix3d arbMat;
		getEcsToWcsMatrix(AcGePoint3d::kOrigin, ecsZAxis, arbMat);

		AcGePoint3d origin;
		AcGeVector3d yAxis, zAxis;

		arbMat.getCoordSystem(origin, xAxis, yAxis, zAxis);    // return xAxis;
	}
	static void getUcsToWcsMatrix(AcGeMatrix3d& m, AcDbDatabase* db)
	{
		ASSERT(db != NULL);

		if (!acdbUcsMatrix(m, db)) {
			m.setToIdentity();
			ASSERT(0);
		}
	}
	static void	transformToWcs(AcDbEntity* ent, AcDbDatabase* db)
	{
		ASSERT(ent->isWriteEnabled());
		ASSERT(db != NULL);

		AcGeMatrix3d m;
		getUcsToWcsMatrix(m, db);
		Acad::ErrorStatus es = ent->transformBy(m);

		ASSERT(es == Acad::eOk);
	}
	static LPCTSTR objToClassStr(const AcRxObject* obj)
	{
		ASSERT(obj != NULL);

		AcRxClass* rxClass = obj->isA();
		if (rxClass == NULL) {
			ASSERT(0);
			acedAlert(_T("AcRxObject class has not called rxInit()!"));
			return _T("*Unknown*");
		}

		return obj->isA()->name();
	}
	static LPCTSTR objToHandleStr(const AcDbObject* obj, CString& str)
	{
		ASSERT(obj != NULL);

		AcDbHandle handle;

		obj->getAcDbHandle(handle);
		TCHAR tmpStr[256];
		handle.getIntoAsciiBuffer(tmpStr);
		str = tmpStr;
		return str;
	}
	static LPCTSTR objToClassAndHandleStr(const AcDbObjectId& objId, CString& str)
	{
		CString tmpStr;
		AcDbObject* tmpObj;

		if (objId.isNull()) {
			str = _T("AcDbObjectId::kNull");
		}
		else {
			// open up even if erased
			Acad::ErrorStatus es = acdbOpenObject(tmpObj, objId, AcDb::kForRead, true);
			if (es == Acad::eOk) {
				str.Format(_T("<%-20s %-4s>"),
					objToClassStr(tmpObj), objToHandleStr(tmpObj, tmpStr));
				tmpObj->close();
			}
			else
				str = rxErrorStr(es);
		}

		return str;
	}
	static LPCTSTR objToClassAndHandleStr(const AcDbObject* obj, CString& str)
	{
		CString tmpStr;

		str.Format(_T("<%-20s %-4s>"),
			objToClassStr(obj), objToHandleStr(obj, tmpStr));

		return str;
	}

	static LPCTSTR filerTypeToStr(AcDb::FilerType type, CString& str)
	{
		if (type == AcDb::kFileFiler)
			str = _T("File");
		else if (type == AcDb::kCopyFiler)
			str = _T("Copy");
		else if (type == AcDb::kUndoFiler)
			str = _T("Undo");
		else if (type == AcDb::kBagFiler)
			str = _T("Bag");
		else if (type == AcDb::kIdXlateFiler)
			str = _T("IdXlate");
		else if (type == AcDb::kPageFiler)
			str = _T("Page");
		else if (type == AcDb::kIdFiler)
			str = _T("Id");
		else if (type == AcDb::kDeepCloneFiler)
			str = _T("DeepClone");
		else if (type == AcDb::kPurgeFiler)
			str = _T("Purge");
		else if (type == AcDb::kWblockCloneFiler)
			str = _T("WblockClone");
		else {
			ASSERT(0);
			str.Format(_T("*Unknown* (%d)"), type);
		}

		return str;
	}

	static Acad::ErrorStatus enameToObjId(ads_name ent, AcDbObjectId& objId)
	{
		Acad::ErrorStatus es = acdbGetObjectId(objId, ent);
		ASSERT(es == Acad::eOk);
		if (es != Acad::eOk)
			rxErrorMsg(es);

		return es;
	}
	static LPCTSTR ptToStr(const AcGePoint3d& pt, CString& str, int unit, int prec)
	{
		TCHAR xstr[100], ystr[100], zstr[100];

		acdbRToS(pt.x, unit, prec, xstr);
		acdbRToS(pt.y, unit, prec, ystr);
		acdbRToS(pt.z, unit, prec, zstr);

		str.Format(_T("(%s, %s, %s)"), xstr, ystr, zstr);
		return str;
	}
	static LPCTSTR doubleToStr(const double realVal, CString& str, int unit = -1, int prec = -1)
	{
		TCHAR tmpStr[100];

		acdbRToS(realVal, unit, prec, tmpStr);
		str = tmpStr;
		return str;
	}
	static LPCTSTR angleToStr(const double angle, CString& str, int unit, int prec)
	{
		TCHAR tmpStr[100];

		acdbAngToS(angle, unit, prec, tmpStr);
		str = tmpStr;
		return str;
	}


	static LPCTSTR vectorToStr(const AcGeVector3d& vec, CString& str, int unit, int prec)
	{
		TCHAR xstr[100], ystr[100], zstr[100];

		acdbRToS(vec.x, unit, prec, xstr);
		acdbRToS(vec.y, unit, prec, ystr);
		acdbRToS(vec.z, unit, prec, zstr);

		str.Format(_T("(%s, %s, %s)"), xstr, ystr, zstr);
		return str;
	}
	static Acad::ErrorStatus verifyClassType(const AcDbObject* obj, AcRxClass* classType)
	{
		ASSERT(classType != NULL);

		if (obj->isAProxy()) {
			// we can't really know if this is correct or not, so just leave it
			return Acad::eOk;
		}
		else {
			if (obj->isKindOf(classType))
				return Acad::eOk;
		}

		return Acad::eWrongObjectType;
	}



	static AcDbBlockTableRecord*
		openCurrentSpaceBlock(AcDb::OpenMode mode, AcDbDatabase* db)
	{
		ASSERT(db != NULL);

		AcDbBlockTableRecord* blkRec;

		Acad::ErrorStatus es = acdbOpenObject(blkRec, db->currentSpaceId(), mode);
		if (es != Acad::eOk)
			return NULL;
		else
			return blkRec;
	}


	static Acad::ErrorStatus
		addToCurrentSpace(AcDbEntity* newEnt)
	{
		return addToCurrentSpace(newEnt, acdbHostApplicationServices()->workingDatabase());
	}

	/****************************************************************************
	**
	**   addToCurrentSpace
	**      add a new entity to the current space block record.
	**
	**  **jma
	**
	*************************************/

	static Acad::ErrorStatus
		addToCurrentSpace(AcDbEntity* newEnt, AcDbDatabase* db)
	{
		ASSERT(newEnt != NULL);
		ASSERT(db != NULL);

		AcDbBlockTableRecord* blkRec = openCurrentSpaceBlock(AcDb::kForWrite, db);

		ASSERT(blkRec != NULL);

		if (blkRec == NULL)
			return Acad::eInvalidInput;

		// append new entity to current space block
		Acad::ErrorStatus es = blkRec->appendAcDbEntity(newEnt);
		if (es != Acad::eOk) {
			acutPrintf(_T("\nERROR: could not add entity to current space (%s)"), rxErrorStr(es));
		}

		blkRec->close();
		return es;
	}

	/****************************************************************************
	**
	**   addToCurrentSpaceAndClose
	**      add a new entity to the current space block record.
	**
	**  NOTE:  this function accepts responsibility for the new entity.
	**  If appending to the blkRec fails, it will delete it and if successful
	**  it will close it.
	**
	**  **jma
	**
	*************************************/

	static Acad::ErrorStatus
		addToCurrentSpaceAndClose(AcDbEntity* newEnt, AcDbDatabase* db)
	{
		Acad::ErrorStatus es;

		es = addToCurrentSpace(newEnt, db);
		if (es == Acad::eOk)
			newEnt->close();
		else
			delete newEnt;

		return es;
	}



	static Acad::ErrorStatus
		addToCurrentSpaceAndClose(AcDbEntity* newEnt)
	{
		return addToCurrentSpaceAndClose(newEnt, acdbHostApplicationServices()->workingDatabase());
	}


	static Acad::ErrorStatus verifyEntityReference(const AcDbEntity* ent, const AcDbObjectId& idToValidate,
		AcRxClass* classType, bool allowNull, bool allowErased)
	{
		if (ent == NULL) {
			ASSERT(0);
			return Acad::eInvalidInput;
		}

		// first check for NULL
		if (idToValidate == AcDbObjectId::kNull) {
			if (allowNull)
				return Acad::eOk;
			else
				return Acad::eNullObjectId;
		}

		// now open it up and see what it is
		Acad::ErrorStatus es;
		AcDbEntity* entToValidate;
		es = acdbOpenAcDbEntity(entToValidate, idToValidate, AcDb::kForRead);
		if (es == Acad::eOk) {
			// make sure all references are from the same database
			// which should always be the case!
			if (ent->database() != entToValidate->database()) {
				entToValidate->close();
				return Acad::eWrongDatabase;
			}
			// make sure that references are from the same block
			// This can fail from a simple deepClone() to make a
			// blockTableRecord.  If the user didn't pick all the
			// referenced objects in the anchor at the time of
			// making the block, they will still point to the
			// entities in modelSpace... which will eventually crash
			// the whole thing.
			if (ent->blockId() != entToValidate->blockId()) {
				entToValidate->close();
				return Acad::eNotInBlock;
			}

			// see if they want to validate based on object type
			if (classType != NULL) {
				es = verifyClassType(entToValidate, classType);
				if (es != Acad::eOk) {
					entToValidate->close();
					return es;
				}
			}

			entToValidate->close();
			return Acad::eOk;        // passed our test    
		}
		else if (es == Acad::eWasErased) {
			if (allowErased)
				return Acad::eOk;    // passed under relaxed constraints
			else
				return Acad::eWasErased;    // failed
		}
		else {
			return es;        // failed
		}
	}

	//Dictionary Utils


	static Acad::ErrorStatus collectDictionaryEntryNames(const AcDbObjectId& dictId, SdStrObjIdList& list, AcRxClass* classType)
	{
		Acad::ErrorStatus ret = Acad::eInvalidInput;

		AcDbDictionary* dict;
		ret = acdbOpenObject(dict, dictId, AcDb::kForRead);
		if (ret == Acad::eOk) {
			Acad::ErrorStatus es;
			AcDbObject* obj;
			AcDbDictionaryIterator* dictIter = dict->newIterator();
			ASSERT(dictIter != NULL);
			if (dictIter != NULL) {
				for (; !dictIter->done(); dictIter->next()) {
					es = dictIter->getObject(obj, AcDb::kForRead);
					if (es == Acad::eOk) {
						if (classType == NULL)
							list.AddAlpha(dictIter->name(), dictIter->objectId());
						else if (obj->isKindOf(classType))
							list.AddAlpha(dictIter->name(), dictIter->objectId());
						obj->close();
					}
				}
				delete dictIter;
				ret = Acad::eOk;
			}
			else
				ret = Acad::eInvalidInput;

			dict->close();
		}
		return ret;
	}
	/****************************************************************************
	**
	**	getRegularDictionaryId
	**		get the id of a dictionary considered to be "regular" (one which is
	**	underneath the RootNamedObjectDictionary.
	**
	**	**jma
	**
	*************************************/

	static AcDbObjectId	getRegularDictionaryId(LPCTSTR dictName, bool createIfNotFound, AcDbDatabase* db)
	{
		ASSERT(db != NULL);

		AcDbObjectId dictId = AcDbObjectId::kNull;

		// get the root dict for this database
		AcDbDictionary* rootDict;
		Acad::ErrorStatus es = db->getNamedObjectsDictionary(rootDict, AcDb::kForRead);
		if (es != Acad::eOk) {
			rxErrorMsg(es);
			return AcDbObjectId::kNull;
		}

		// try to first open for read and see if it is already there
		AcDbDictionary* dict;
		dict = openDictionaryForRead(dictName, rootDict);
		if (dict) {
			dictId = dict->objectId();
			dict->close();
		}
		else {    // the dictionary has not yet been created
			if (createIfNotFound) {
				es = rootDict->upgradeOpen();
				if ((es == Acad::eOk) || (es == Acad::eWasOpenForWrite)) {
					dict = openDictionaryForWrite(dictName, true, rootDict);
					if (dict) {
						dictId = dict->objectId();
						dict->close();
					}
				}
			}
		}
		rootDict->close();    // don't need it anymore!
		return dictId;
	}

	/****************************************************************************
	**
	**  getSubDictionaryId
	**
	**  **jma
	**
	*************************************/

	static AcDbObjectId
		getSubDictionaryId(LPCTSTR dictName, bool createIfNotFound,
			AcDbDictionary* parentDict)
	{
		ASSERT(parentDict != NULL);

		AcDbObjectId dictId;

		// try to first see if it is already there
		Acad::ErrorStatus es = parentDict->getAt(dictName, dictId);
		if (es == Acad::eOk) {
			return dictId;
		}
		else if (es == Acad::eKeyNotFound) {
			if (createIfNotFound) {
				// upgrade parentDict to write if not already
				if (parentDict->isWriteEnabled() == Adesk::kFalse) {
					es = parentDict->upgradeOpen();
					if ((es != Acad::eOk) && (es != Acad::eWasOpenForWrite)) {
						rxErrorAlert(es);
						ASSERT(0);
						return AcDbObjectId::kNull;
					}
				}
				// create this new dict
				AcDbDictionary* newDict = new AcDbDictionary;
				es = parentDict->setAt(dictName, newDict, dictId);
				if (es == Acad::eOk) {
					newDict->close();
					return dictId;
				}
				else {
					delete newDict;
					return AcDbObjectId::kNull;
				}
			}
			else
				return AcDbObjectId::kNull;
		}
		else {
			rxErrorAlert(es);
			ASSERT(0);
			return AcDbObjectId::kNull;
		}
	}

	/****************************************************************************
	**
	**	openDictionaryForRead
	**		use this when you expect the dictionary to be there and you
	**	are just retrieving values out of it. This is a wrapper for finding
	**	this dictionary underneath the root dictionary of the current drawing.
	**
	**	**jma
	**
	*************************************/

	static AcDbDictionary* openDictionaryForRead(LPCTSTR dictName, AcDbDatabase* db)
	{
		ASSERT(db != NULL);

		// get the root dict for this database
		AcDbDictionary* dict = NULL;
		AcDbDictionary* rootDict;
		Acad::ErrorStatus es = db->getNamedObjectsDictionary(rootDict, AcDb::kForRead);
		if (es == Acad::eOk) {
			// now get the sub-dict
			dict = openDictionaryForRead(dictName, rootDict);
			rootDict->close();
		}
		else {
			rxErrorMsg(es);
		}

		return dict;
	}

	/****************************************************************************
	**
	**	openDictionaryForRead
	**		use this when you expect the dictionary to be there and you
	**	are just retrieving values out of it. The parentDict is the
	**	dictionary that contians the one you are looking for
	**
	**	**jma
	**
	*************************************/

	static AcDbDictionary*
		openDictionaryForRead(LPCTSTR dictName, const AcDbObjectId& parentDictId)
	{
		ASSERT(dictName != NULL);
		if (parentDictId == AcDbObjectId::kNull)
			return NULL;

		// figure out what parent directory they want
		AcDbDictionary* parentDict;
		Acad::ErrorStatus es = acdbOpenObject(parentDict, parentDictId, AcDb::kForRead);
		if (es != Acad::eOk) {
			rxErrorMsg(es);
			return NULL;
		}

		AcDbDictionary* dict = openDictionaryForRead(dictName, parentDict);
		parentDict->close();
		return dict;
	}

	/****************************************************************************
	**
	**	openDictionaryForRead
	**		use this when you expect the dictionary to be there and you
	**	are just retrieving values out of it. The parentDict is the
	**	dictionary that contians the one you are looking for
	**
	**	**jma
	**
	*************************************/

	static AcDbDictionary*
		openDictionaryForRead(LPCTSTR dictName, AcDbDictionary* parentDict)
	{
		ASSERT(dictName != NULL);
		ASSERT(parentDict != NULL);

		// get the dictionary that we are looking for
		AcDbObject* obj;
		Acad::ErrorStatus es = parentDict->getAt(dictName, obj, AcDb::kForRead);

		if (es == Acad::eOk)
			return AcDbDictionary::cast(obj);
		else
			return NULL;
	}

	/****************************************************************************
	**
	**	openDictionaryForWrite
	**		wrapper for opening a dictionary stored off of the root dictionary
	**	of the current directory.
	**
	**	**jma
	**
	*************************************/

	static AcDbDictionary*
		openDictionaryForWrite(LPCTSTR dictName, bool createIfNotFound, AcDbDatabase* db)
	{
		ASSERT(db != NULL);

		// get the root dict for this database
		AcDbDictionary* dict = NULL;
		AcDbDictionary* rootDict;
		Acad::ErrorStatus es = db->getNamedObjectsDictionary(rootDict, AcDb::kForWrite);
		if (es == Acad::eOk) {
			// now get the sub-dict
			dict = openDictionaryForWrite(dictName, createIfNotFound, rootDict);
			rootDict->close();
		}
		else {
			rxErrorMsg(es);
		}

		return dict;
	}

	/****************************************************************************
	**
	**	openDictionaryForWrite
	**
	**	**jma
	**
	*************************************/

	static AcDbDictionary* openDictionaryForWrite(LPCTSTR dictName, bool createIfNotFound, const AcDbObjectId& parentDictId)
	{
		ASSERT(dictName != NULL);
		if (parentDictId == AcDbObjectId::kNull)
			return NULL;

		// figure out what parent directory they want
		AcDbDictionary* parentDict;
		Acad::ErrorStatus es = acdbOpenObject(parentDict, parentDictId, AcDb::kForWrite);
		if (es != Acad::eOk) {
			rxErrorMsg(es);
			return NULL;
		}

		AcDbDictionary* dict = openDictionaryForWrite(dictName, createIfNotFound, parentDict);
		parentDict->close();
		return dict;
	}

	/****************************************************************************
	**
	**	openDictionaryForWrite
	**
	**	**jma
	**
	*************************************/

	static AcDbDictionary*
		openDictionaryForWrite(LPCTSTR dictName, bool createIfNotFound,
			AcDbDictionary* parentDict)
	{
		ASSERT(dictName != NULL);
		ASSERT(parentDict != NULL);
		ASSERT(parentDict->isWriteEnabled());    // make sure parent is open for write

		AcDbDictionary* dict = NULL;
		AcDbObject* obj;
		Acad::ErrorStatus es;

		// get the dictionary that we are looking for
		es = parentDict->getAt(dictName, obj, AcDb::kForWrite);
		if (es == Acad::eOk) {
			dict = AcDbDictionary::cast(obj);
		}
		else if (es == Acad::eKeyNotFound) {
			if (createIfNotFound) {
				dict = new AcDbDictionary;
				AcDbObjectId dictId;
				es = parentDict->setAt(dictName, dict, dictId);
				if (es != Acad::eOk) {
					acutPrintf(_T("\nERROR: could not create dictionary \"%s\" (%s)"), dictName, rxErrorStr(es));
					delete dict;
					dict = NULL;
				}
			}
		}
		else {
			rxErrorMsg(es);
		}

		return dict;
	}

	/****************************************************************************
	**
	**	lookUpDictEntryName
	**		given an entry Id in a dictionary, look up its parent dictionary
	**	and then find out what the key value is for this entry in the dictionary.
	**
	**	**jma
	**
	*************************************/

	static bool lookUpDictEntryName(const AcDbObjectId& objId, CString& entryName)
	{
		bool retCode = FALSE;
		AcDbObject* obj;

		Acad::ErrorStatus es = acdbOpenAcDbObject(obj, objId, AcDb::kForRead);
		if (es == Acad::eOk) {
			AcDbDictionary* dict;
			es = acdbOpenObject(dict, obj->ownerId(), AcDb::kForRead);
			if (es == Acad::eOk) {
				TCHAR* name = NULL;
				if (dict->nameAt(objId, name) == Acad::eOk) {
					entryName = name;
					retCode = true;
					acutDelString(name);
				}
				dict->close();
			}
			obj->close();
		}
		return retCode;
	}

	/****************************************************************************
	**
	**	openExtDictForRead
	**
	**	**jma
	**
	*************************************/

	static AcDbDictionary*
		openExtDictForRead(const AcDbObject* obj)
	{
		ASSERT(obj != NULL);

		// Get extension dicitonary object Id
		Acad::ErrorStatus es;
		AcDbObjectId extDictId = obj->extensionDictionary();
		if (extDictId == AcDbObjectId::kNull)
			return NULL;

		// Open the dictionary
		AcDbDictionary* extDict;
		es = acdbOpenObject(extDict, extDictId, AcDb::kForRead);
		if (es != Acad::eOk) {
			ASSERT(0);
			return NULL;
		}

		return extDict;
	}

	/****************************************************************************
	**
	**	openExtDictForWrite
	**		open an object's extension dictionary
	**
	**	**jma
	**
	*************************************/

	static AcDbDictionary* openExtDictForWrite(AcDbObject* obj, bool createIfNotFound)
	{
		ASSERT(obj != NULL);
		ASSERT(obj->isWriteEnabled());

		// Get extension dicitonary object Id, if it doesn't
		// exist yet, make it if asked for
		Acad::ErrorStatus es;
		AcDbObjectId extDictId = obj->extensionDictionary();
		if (extDictId == AcDbObjectId::kNull) {
			if (createIfNotFound) {
				es = obj->createExtensionDictionary();
				ASSERT(es == Acad::eOk);
				if (es != Acad::eOk) {
					rxErrorMsg(es);
					return NULL;
				}
				if ((extDictId = obj->extensionDictionary()) == AcDbObjectId::kNull) {
					acutPrintf(_T("\nERROR: Could not create an extension dictionary."));
					return NULL;
				}
			}
			else
				return NULL;
		}
		// Open the dictionary
		AcDbDictionary* extDict;
		es = acdbOpenObject(extDict, extDictId, AcDb::kForWrite);
		if (es != Acad::eOk) {
			ASSERT(0);
			return NULL;
		}

		return extDict;
	}

};
#endif

