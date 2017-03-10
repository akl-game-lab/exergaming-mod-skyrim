#include "ScaleformCallbacks.h"
#include <typeinfo>

GFxValue::~GFxValue()
{
	CleanManaged();
}

void GFxValue::AddManaged(void)
{
	if(IsManaged())
		CALL_MEMBER_FN(objectInterface, AddManaged_Internal)(this, content.obj);
}

void GFxValue::CleanManaged(void)
{
	if(IsManaged())
	{
		CALL_MEMBER_FN(objectInterface, ReleaseManaged_Internal)(this, content.obj);

		objectInterface = NULL;
		type = kType_Undefined;
	}
}

void GFxValue::SetUndefined(void)
{
	CleanManaged();

	type = kType_Undefined;
}

void GFxValue::SetNull(void)
{
	CleanManaged();

	type = kType_Null;
}

void GFxValue::SetBool(bool value)
{
	CleanManaged();

	type = kType_Bool;
	content.boolean = value;
}

void GFxValue::SetNumber(double value)
{
	CleanManaged();

	type = kType_Number;
	content.number = value;
}

void GFxValue::SetString(const char * value)
{
	CleanManaged();

	type = kType_String;
	content.string = value;
}

void GFxValue::SetWideString(const wchar_t * value)
{
	CleanManaged();

	type = kType_WideString;
	content.wideString = value;
}

bool GFxValue::GetBool(void)
{
	switch(GetType())
	{
		case kType_Bool: return content.boolean;
		case kType_Number: return content.number != 0;
		default: HALT("GFxValue::GetBool: bad type"); return false;
	}
}

const char * GFxValue::GetString(void)
{
	if(GetType() != kType_String)
		return NULL;

	if(IsManaged())
		return *content.managedString;
	else
		return content.string;
}

const wchar_t * GFxValue::GetWideString(void)
{
	if(GetType() != kType_WideString)
		return NULL;

	if(IsManaged())
		return *content.managedWideString;
	else
		return content.wideString;
}

double GFxValue::GetNumber(void)
{
	switch(GetType())
	{
		case kType_Number:	return content.number;
		case kType_Bool:	return content.boolean ? 1 : 0;
		default:			HALT("GFxValue::GetNumber: bad type"); return 0;
	}
}

bool GFxValue::HasMember(const char * name)
{
	return CALL_MEMBER_FN(objectInterface, HasMember)(content.obj, name, IsDisplayObject());
}

bool GFxValue::SetMember(const char * name, GFxValue * value)
{
	return CALL_MEMBER_FN(objectInterface, SetMember)(content.obj, name, value, IsDisplayObject());
}

bool GFxValue::GetMember(const char * name, GFxValue * value)
{
	return CALL_MEMBER_FN(objectInterface, GetMember)(content.obj, name, value, IsDisplayObject());
}

bool GFxValue::DeleteMember(const char * name)
{
	return CALL_MEMBER_FN(objectInterface, DeleteMember)(content.obj, name, IsDisplayObject());
}

bool GFxValue::Invoke(const char * name, GFxValue * result, GFxValue * args, UInt32 numArgs)
{
	return CALL_MEMBER_FN(objectInterface, Invoke)(content.obj, result, name, args, numArgs, IsDisplayObject());
}

bool GFxValue::PushBack(GFxValue * value)
{
	return CALL_MEMBER_FN(objectInterface, PushBack)(content.obj, value);
}

UInt32 GFxValue::GetArraySize()
{
	return CALL_MEMBER_FN(objectInterface, GetArraySize)(content.obj);
}

bool GFxValue::GetElement(UInt32 index, GFxValue * value)
{
	return CALL_MEMBER_FN(objectInterface, GetElement)(content.obj, index, value);
}

bool GFxValue::GetDisplayInfo(DisplayInfo * displayInfo)
{
	return CALL_MEMBER_FN(objectInterface, GetDisplayInfo)(content.obj, displayInfo);
}

bool GFxValue::SetDisplayInfo(DisplayInfo * displayInfo)
{
	return CALL_MEMBER_FN(objectInterface, SetDisplayInfo)(content.obj, displayInfo);
}


UInt32 g_GFxFunctionHandler_count = 0;

GFxFunctionHandler::GFxFunctionHandler()
{
	g_GFxFunctionHandler_count++;
}

GFxFunctionHandler::~GFxFunctionHandler()
{
	g_GFxFunctionHandler_count--;
}

FunctionHandlerCache g_functionHandlerCache;

const FxDelegateHandler::Callback PlaySoundCallback = (FxDelegateHandler::Callback)0x00899940;