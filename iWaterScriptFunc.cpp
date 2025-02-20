#include "pch.h"
#include "iWaterScriptFunc.h"

bool CheckValidTagName(const char* TagName)
{
	if (g_pTagCol == NULL)
		return false;
	if (TagName == NULL || strlen(TagName) == 0 || strcmp(TagName, "") == 0)
		return false;

	int nErr = 1;
	TAGTYPEID TagType = TAGTYPEID::ENUMEND;
	try
	{
		TagType = g_pTagCol->GetTagType((char*)TagName, &nErr);
		if (TagType != TAGTYPEID::ENUMEND && nErr > 0)
			return true;
	}
	catch (...)
	{

	}
    return false;
}

bool KWPutTagValue(const char* TagName, c_variable TagValue)
{
	if (g_pTagCol == NULL)
		return false;
	if (TagName == NULL || strlen(TagName) == 0 || strcmp(TagName, "") == 0)
		return false;
	int nErr = 1;
	TAGTYPEID TagType = TAGTYPEID::ENUMEND;
	char szNode[64] = { 0 };
	double dValue = INT_MIN;
	c_string strValue = "";
	try
	{
		if (g_pTagCol->GetTagNodeName((char*)TagName, szNode) != 1)
			return false;
		TagType = g_pTagCol->GetTagType(szNode,(char*)TagName, &nErr);
		if (TagType == TAGTYPEID::ENUMEND || nErr < 0)
			return false;

		if (strlen(szNode) == 0 || strcmp(szNode, "") == 0)
			return false;
		switch (TagType)
		{
		case TAGTYPEID::ANALOGINPUT:
		case TAGTYPEID::DIGITALINPUT:
			dValue = TagValue.as_double();
			if (dValue == INT_MIN)
				return false;
			if (g_pTagCol->PutTagValue(szNode, (char*)TagName, dValue) != 1)
				return false;
			break;
		case TAGTYPEID::ANALOGOUTPUT:
		case TAGTYPEID::DIGITALOUTPUT:
			dValue = TagValue.as_double();
			if (dValue == INT_MIN)
				return false;
			if (g_pTagCol->PutTagOutput(szNode, (char*)TagName, dValue) != 1)
				return false;
			break;
		case TAGTYPEID::TEXT:
			TagValue.as_string(strValue);
			if (strlen(strValue.get_buffer()) == 0 || strcmp(strValue.get_buffer(), "") == 0)
				return false;
			if (strlen(strValue.get_buffer()) >= TEXTDATALEN)
			{
				if (g_pTagCol->PutStrTagValue(szNode, (char*)TagName, (char*)strValue.get_buffer(), TEXTDATALEN - 1) != 1)
					return false;
			}
			else
			{
				if (g_pTagCol->PutStrTagValue(szNode, (char*)TagName, (char*)strValue.get_buffer(), strValue.get_length()) != 1)
					return false;
			}
			break;
		default:
			return false;
		}
		return true;
	}
	catch (...)
	{

	}
    return false;
}

bool KWGetTagValue(const char* TagName, c_variable& TagValue)
{
	if (g_pTagCol == NULL)
		return false;
	if (TagName == NULL || strlen(TagName) == 0 || strcmp(TagName, "") == 0)
		return false;
	int nErr = 1;
	TAGTYPEID TagType = TAGTYPEID::ENUMEND;
	char szNode[64] = { 0 };
	char szTagValue[TEXTDATALEN] = { 0 };
	double dValue = INT_MIN;
	c_string strValue = "";
	time_t tChg = 0;
	try
	{
		if (g_pTagCol->GetTagNodeName((char*)TagName, szNode) != 1)
			return false;
		TagType = g_pTagCol->GetTagType(szNode,(char*)TagName, &nErr);
		if (TagType == TAGTYPEID::ENUMEND || nErr < 0)
			return false;

		if (strlen(szNode) == 0 || strcmp(szNode, "") == 0)
			return false;
		switch (TagType)
		{
		case TAGTYPEID::ANALOGINPUT:
		case TAGTYPEID::DIGITALINPUT:
		case TAGTYPEID::ANALOGOUTPUT:
		case TAGTYPEID::DIGITALOUTPUT:
			if (g_pTagCol->GetTagValue((char*)TagName, dValue,&tChg) != 1)
				return false;
			TagValue = dValue;
			break;
		case TAGTYPEID::TEXT:
			if (g_pTagCol->GetStrTagValue((char*)TagName, szTagValue, &tChg) != 1)
				return false;
			TagValue = szTagValue;
			break;
		default:
			return false;
		}
		return true;
	}
	catch (...)
	{

	}
	return false;
}

bool KWSetTagField(const char* TagName, TAG_FIELD field, c_variable FieldValue)
{
	if (g_pTagCol == NULL)
		return false;
	if (TagName == NULL || strlen(TagName) == 0 || strcmp(TagName, "") == 0)
		return false;
	int nErr = 1;
	TAGTYPEID TagType = TAGTYPEID::ENUMEND;
	char szTagNodeName[64] = { 0 };
	CString strLocalNodeName = "";
	bool bLocal = false;
	LPTAG_ANALOG pAi = NULL;
	LPTAG_ANALOG_OUTPUT pAo = NULL;
	LPTAG_DIGITAL pDi = NULL;
	LPTAG_DIGITAL_OUTPUT pDo = NULL;
	LPTAG_BASE pTag = NULL;
	int nAiCount = 0;
	int nAoCount = 0;
	int nDiCount = 0;
	int nDoCount = 0;
	int nFieldValue = INT_MIN;
	try
	{
		if (g_pTagCol->GetTagNodeName((char*)TagName, szTagNodeName) != 1)
			return false;
		TagType = g_pTagCol->GetTagType(szTagNodeName,(char*)TagName, &nErr);
		if (TagType == TAGTYPEID::ENUMEND || nErr < 0)
			return false;

		if (strlen(szTagNodeName) == 0 || strcmp(szTagNodeName, "") == 0)
			return false;
		strLocalNodeName = g_pTagCol->GetLocalGrpNodeName();
		if (strlen(strLocalNodeName) == 0 || strcmp(strLocalNodeName, "") == 0)
			return false;
		if (_stricmp(strLocalNodeName, szTagNodeName) == 0)
			bLocal = true;
		switch (TagType)
		{
		case TAGTYPEID::ANALOGINPUT:
			pAi = g_pTagCol->GetAiTag(szTagNodeName, (char*)TagName);
			if (pAi == NULL)
				return false;
			nAiCount++;
			pTag = &pAi->tagInfo;
			break;
		case TAGTYPEID::DIGITALINPUT:
			pDi = g_pTagCol->GetDiTag(szTagNodeName, (char*)TagName);
			if (pDi == NULL)
				return false;
			pTag = &pDi->tagInfo;
			nDiCount++;
			break;
		case TAGTYPEID::ANALOGOUTPUT:
			pAo = g_pTagCol->GetAoTag(szTagNodeName, (char*)TagName);
			if (pAo == NULL)
				return false;
			pTag = &pAo->tagInfo;
			nAoCount++;
			break;
		case TAGTYPEID::DIGITALOUTPUT:
			pDo = g_pTagCol->GetDoTag(szTagNodeName, (char*)TagName);
			if (pDo == NULL)
				return false;
			pTag = &pDo->tagInfo;
			nDoCount++;
			break;
		default:
			return false;
		}
		if (pTag == NULL)
			return false;
		nFieldValue = FieldValue.as_integer();
		if (nFieldValue == INT_MIN)
			return false;
		switch (field)
		{
		case TAG_FIELD::ALARM:
			if (nFieldValue == 1)
			{
				if ((pTag->useFlag & USEALARM) != USEALARM)
					pTag->useFlag = pTag->useFlag | USEALARM;
			}
			else if (nFieldValue == 0)
			{
				if ((pTag->useFlag & USEALARM) == USEALARM)
					pTag->useFlag = pTag->useFlag ^ USEALARM;
			}
			else
				return false;
			break;
		case TAG_FIELD::SCAN:
			if (nFieldValue == 1)
			{
				if ((pTag->useFlag & USESCAN) != USESCAN)
					pTag->useFlag = pTag->useFlag | USESCAN;
			}
			else if (nFieldValue == 0)
			{
				if ((pTag->useFlag & USESCAN) == USESCAN)
					pTag->useFlag = pTag->useFlag ^ USESCAN;
			}
			else
				return false;
			break;
		default:
			return false;
		}
		if (bLocal)
			g_pTagCol->ModNodeTag(g_szLocalModNode, pAi, nAiCount, pDi, nDiCount, pAo, nAoCount, pDo, nDoCount);
		else
			g_pTagCol->ModNodeTag(szTagNodeName, pAi, nAiCount, pDi, nDiCount, pAo, nAoCount, pDo, nDoCount);
		return true;
	}
	catch (...)
	{

	}
	return false;
}

bool KWGetTagField(const char* TagName, TAG_FIELD field, c_variable& FieldValue)
{
	if (g_pTagCol == NULL)
		return false;
	if (TagName == NULL || strlen(TagName) == 0 || strcmp(TagName, "") == 0)
		return false;
	int nErr = 1;
	TAGTYPEID TagType = TAGTYPEID::ENUMEND;
	char szTagNodeName[64] = { 0 };
	CString strLocalNodeName = "";
	bool bLocal = false;
	LPTAG_ANALOG pAi = NULL;
	LPTAG_ANALOG_OUTPUT pAo = NULL;
	LPTAG_DIGITAL pDi = NULL;
	LPTAG_DIGITAL_OUTPUT pDo = NULL;
	LPTAG_BASE pTag = NULL;
	int nAiCount = 0;
	int nAoCount = 0;
	int nDiCount = 0;
	int nDoCount = 0;
	int nFieldValue = INT_MIN;
	try
	{
		if (g_pTagCol->GetTagNodeName((char*)TagName, szTagNodeName) != 1)
			return false;
		TagType = g_pTagCol->GetTagType(szTagNodeName,(char*)TagName, &nErr);
		if (TagType == TAGTYPEID::ENUMEND || nErr < 0)
			return false;

		if (strlen(szTagNodeName) == 0 || strcmp(szTagNodeName, "") == 0)
			return false;
		strLocalNodeName = g_pTagCol->GetLocalGrpNodeName();
		if (strlen(strLocalNodeName) == 0 || strcmp(strLocalNodeName, "") == 0)
			return false;
		if (_stricmp(strLocalNodeName, szTagNodeName) == 0)
			bLocal = true;
		switch (TagType)
		{
		case TAGTYPEID::ANALOGINPUT:
			pAi = g_pTagCol->GetAiTag(szTagNodeName, (char*)TagName);
			if (pAi == NULL)
				return false;
			nAiCount++;
			pTag = &pAi->tagInfo;
			break;
		case TAGTYPEID::DIGITALINPUT:
			pDi = g_pTagCol->GetDiTag(szTagNodeName, (char*)TagName);
			if (pDi == NULL)
				return false;
			pTag = &pDi->tagInfo;
			nDiCount++;
			break;
		case TAGTYPEID::ANALOGOUTPUT:
			pAo = g_pTagCol->GetAoTag(szTagNodeName, (char*)TagName);
			if (pAo == NULL)
				return false;
			pTag = &pAo->tagInfo;
			nAoCount++;
			break;
		case TAGTYPEID::DIGITALOUTPUT:
			pDo = g_pTagCol->GetDoTag(szTagNodeName, (char*)TagName);
			if (pDo == NULL)
				return false;
			pTag = &pDo->tagInfo;
			nDoCount++;
			break;
		default:
			return false;
		}
		if (pTag == NULL)
			return false;
		switch (field)
		{
		case TAG_FIELD::ALARM:
			FieldValue = (pTag->useFlag & USEALARM) == USEALARM ? 1 : 0;
			break;
		case TAG_FIELD::SCAN:
			FieldValue = (pTag->useFlag & USESCAN) == USESCAN ? 1 : 0;
			break;
		default:
			return false;
		}
		return true;
	}
	catch (...)
	{

	}
	return false;
}

bool KWSendMsgToViewer(COPYDATASTRUCT cds)
{
	HWND hViewer = NULL;
	try
	{
		if (g_pTagCol->GetViewer(hViewer) != 1)
			return false;
		if (hViewer == NULL)
			return false;
		if (!IsWindow(hViewer))
			return false;
		::SendMessageTimeout(hViewer, WM_COPYDATA, NULL, (LPARAM)&cds, SMTO_NORMAL, 1000, NULL);
		return true;
	}
	catch (...)
	{

	}
	return false;
}
