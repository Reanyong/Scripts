#ifndef OTHER_H
#define OTHER_H

#include "extensions.h"

void __stdcall CreateObject		(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

void __stdcall MsgBox			(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall DbgPrint			(int nargs, c_variable** pargs, c_engine* p_engine);

// Tag Value Control 20220715 YDK 수정
void __stdcall GetTagValue	(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall SetTagValue(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall Sleep(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

// Tag Value toggle 20230118 PHK 추가
void __stdcall ToggleDigitalPoint(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result); //ok
void __stdcall AcknowledgeAllAlarms(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall AcknowledgeAnAlarm(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall DisableAlarm(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall EnableAlarm(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall CloseDigitalPoint(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall OpenDigitalPoint(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

void __stdcall ClosePicture(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall OpenPicture(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall ReplacePicture(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall OffScan(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall OnScan(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall ToggleScan(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall ReadValue(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall WriteValue(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall RampValue(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
//void __stdcall LocateObject(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

void __stdcall Login(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
//void __stdcall PictureAlias(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

void __stdcall Execute(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall HmiDuplexTransfer(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall HmiNetworkTransfer(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
//문자열 파싱
void __stdcall sprintf(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

//20210402 YYYY-MM-DD HH:MM:SS.MIL 을 찍어보자
void __stdcall DbgPrintMil			(int nargs, c_variable** pargs, c_engine* p_engine);
bool _check_DbgPrintMil(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

void __stdcall DbgLog			(int nargs, c_variable** pargs, c_engine* p_engine);
void __stdcall DbgBreak			(int nargs, c_variable** pargs, c_engine* p_engine);
void __stdcall iif				(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall eval				(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall Stop				(int nargs, c_variable** pargs, c_engine* p_engine);
void __stdcall Assert			(int nargs, c_variable** pargs, c_engine* p_engine);
void __stdcall ChgEngActiveStandby(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall SetObjectVisible(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall SetObjectText(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
void __stdcall SetNodePrimaryType(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);
// check functions ------------------------------------------------------------

bool _check_CreateObject		(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

bool _check_MsgBox			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_DbgPrint		(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_DbgLog			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_DbgBreak		(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_iif				(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_eval			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_Stop			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_Assert			(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);


// Tag Value Control 20220715 YDK 수정

bool _check_GetTagValue	(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_SetTagValue(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_Sleep(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

// Tag Value toggle 20230118 PHK 추가
bool _check_ToggleDigitalPoint(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_AcknowledgeAllAlarms(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_AcknowledgeAnAlarm(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_DisableAlarm(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_EnableAlarm(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_CloseDigitalPoint(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_OpenDigitalPoint(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

bool _check_ClosePicture(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_OpenPicture(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_ReplacePicture(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_OffScan(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_OnScan(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_ToggleScan(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_ReadValue(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_WriteValue(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_RampValue(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
//bool _check_LocateObject(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

bool _check_Login(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
//bool _check_PictureAlias(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

bool _check_Execute(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_HmiDuplexTransfer(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_HmiNetworkTransfer(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
//문자열 파싱
bool _check_sprintf(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_ChgEngActiveStandby(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_SetObjectVisible(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_SetObjectText(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
bool _check_SetNodePrimaryType(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);

bool _check_SetObjectFont(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall SetObjectFont(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_SetObjectSize(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall SetObjectSize(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetHmiRedundancyStatusCur(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetHmiRedundancyStatusCur(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_SetObjectFontSize(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall SetObjectFontSize(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_SetObjectFontType(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall SetObjectFontType(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_SetObjectForegroundColor(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall SetObjectForegroundColor(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_SetObjectBackgroundColor(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall SetObjectBackgroundColor(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_SetObjectOutlineColor(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall SetObjectOutlineColor(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_SetObjectFillValueX(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall SetObjectFillValueX(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_SetObjectFillValueY(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall SetObjectFillValueY(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_SetObjectFillDirectionX(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall SetObjectFillDirectionX(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_SetObjectFillDirectionY(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall SetObjectFillDirectionY(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_SetObjectRotate(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall SetObjectRotate(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_SetObjectSizeX(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall SetObjectSizeX(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_SetObjectSizeY(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall SetObjectSizeY(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_SetObjectSize(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall SetObjectSize(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_SetObjectPosX(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall SetObjectPosX(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_SetObjectPosY(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall SetObjectPosY(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_SetObjectPos(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall SetObjectPos(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetObjectVisible(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetObjectVisible(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetObjectText(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetObjectText(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetObjectFont(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetObjectFont(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetObjectFontSize(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetObjectFontSize(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetObjectFontType(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetObjectFontType(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetObjectForegroundColor(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetObjectForegroundColor(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetObjectBackgroundColor(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetObjectBackgroundColor(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetObjectOutlineColor(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetObjectOutlineColor(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetObjectFillValueX(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetObjectFillValueX(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetObjectFillValueY(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetObjectFillValueY(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetObjectFillDirectionX(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetObjectFillDirectionX(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetObjectFillDirectionY(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetObjectFillDirectionY(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetObjectRotate(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetObjectRotate(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetObjectSizeX(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetObjectSizeX(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetObjectSizeY(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetObjectSizeY(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetObjectSize(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetObjectSize(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetObjectPosX(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetObjectPosX(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetObjectPosY(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetObjectPosY(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

bool _check_GetObjectPos(int n, VARENUM* p_types, c_string* p_msg, c_engine* p_engine);
void __stdcall GetObjectPos(int nargs, c_variable** pargs, c_engine* p_engine, c_variable& result);

#endif