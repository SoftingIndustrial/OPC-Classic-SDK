#include <jni.h>
#include <stdio.h>
#include <windows.h>

#include <OTCommon.h>
#include <OTClient.h>
#include <string.h>
#include <tchar.h> //UNICODE
#include <atlconv.h> //conversion functions A2T, T2A
#include "Main.h"

//global data
static JavaVM* jvm = NULL;

//Cached JNI class, method, field IDS (Symbolic lookups are relatively EXPENSIVE)
bool g_bCacheClsMethodIds = false;

jclass g_clsDoubleArray = NULL;
jclass g_clsFloatArray = NULL;
jclass g_clsLongArray = NULL;
jclass g_clsDateArray = NULL;
jclass g_clsBooleanArray = NULL;
jclass g_clsIntegerArray = NULL;
jclass g_clsShortArray = NULL;
jclass g_clsByteArray = NULL;
jclass g_clsBoolean = NULL;
jmethodID g_midBooleanCtor = NULL;
jmethodID g_midBooleanBoolValue = NULL;
jclass g_clsByte = NULL;
jmethodID g_midByteCtor = NULL;
jmethodID g_midByteByteValue = NULL;
jclass g_clsShort = NULL;
jmethodID g_midShortCtor = NULL;
jmethodID g_midShortShortValue = NULL;
jclass g_clsInteger = NULL;
jmethodID g_midIntegerCtor = NULL;
jmethodID g_midIntegerIntValue = NULL;
jclass g_clsDate = NULL;
jclass g_clsFloat = NULL;
jmethodID g_midFloatCtor = NULL;
jmethodID g_midFloatFloatValue = NULL;
jclass g_clsDouble = NULL;
jmethodID g_midDoubleCtor = NULL;
jmethodID g_midDoubleDoubleValue = NULL;
jclass g_clsLong = NULL;
jmethodID g_midLongCtor = NULL;
jmethodID g_midLongLongValue = NULL;
jclass g_clsArrayList = NULL;
jmethodID g_midArrayListGet = NULL;
jmethodID g_midArrayListAdd = NULL;
jclass g_clsValueQT = NULL;
jmethodID g_midValueQTCtor = NULL;
jmethodID g_midValueQTGetQuality = NULL;
jclass g_clsEnumQuality = NULL;
jmethodID g_midEnumQualityGetEnum = NULL;
jmethodID g_midEnumQualityToShort = NULL;
jmethodID g_midValueQTGetData = NULL;
jmethodID g_midValueQTGetTimestamp = NULL;
jclass g_clsJNIFILETIME = NULL;
jmethodID g_midJNIFILETIMECtor = NULL;
jmethodID g_midJNIFILETIMECtor2 = NULL;
jmethodID g_midJNIFILETIMEToDate = NULL;
jclass g_clsJNIOTCExecutionOptions = NULL;
jclass g_clsJNIOTCallbackFunctions = NULL;
jclass g_clsCallbackRC = NULL;
jmethodID g_midCallbackRC = NULL;
jclass g_clsCallbackWC = NULL;
jmethodID g_midCallbackWC = NULL;
jclass g_clsCallbackDC = NULL;
jmethodID g_midCallbackDC = NULL;
jclass g_clsJNIOTObjectData = NULL;
jmethodID g_midJNIOTObjectDataCtor = NULL;
jclass g_clsJNIOTCObjectContext = NULL;
jmethodID g_midJNIOTCObjectContextCtor = NULL;
jclass g_clsJNIOTCObjectDataArray = NULL;
jclass g_clsJNIOTValueDataArray = NULL;
jclass g_clsString = NULL;
jmethodID g_midStringCtor = NULL;
jclass g_clsStringArray = NULL;
jclass g_clsJNIOTValueData = NULL;
jmethodID g_midJNIOTValueDataCtor = NULL;
jclass g_clsValueData = NULL;
jmethodID g_midValueDataGetVartype = NULL;
jclass g_clsCharset = NULL;
jmethodID g_midCharsetForName = NULL;

//end global data

/*
* function for caching JNI class, method, field ID's
*/
void cacheClsMethodIds(JNIEnv* env)
{
	//DebugBreak();
	if (!g_bCacheClsMethodIds)
	{
		//Byte[]
		jclass clsByteArray = env->FindClass("[Ljava/lang/Byte;");
		g_clsByteArray = (jclass)env->NewGlobalRef((jobject)clsByteArray);
		//Short[]
		jclass clsShortArray = env->FindClass("[Ljava/lang/Short;");
		g_clsShortArray = (jclass)env->NewGlobalRef((jobject)clsShortArray);
		//Integer[]
		jclass clsIntegerArray = env->FindClass("[Ljava/lang/Integer;");
		g_clsIntegerArray = (jclass)env->NewGlobalRef((jobject)clsIntegerArray);
		//Boolean[]
		jclass clsBooleanArray = env->FindClass("[Ljava/lang/Boolean;");
		g_clsBooleanArray = (jclass)env->NewGlobalRef((jobject)clsBooleanArray);
		//Date[]
		jclass clsDateArray = env->FindClass("[Ljava/util/Date;");
		g_clsDateArray = (jclass)env->NewGlobalRef((jobject)clsDateArray);
		//Long[]
		jclass clsLongArray = env->FindClass("[Ljava/lang/Long;");
		g_clsLongArray = (jclass)env->NewGlobalRef((jobject)clsLongArray);
		//Float[]
		jclass clsFloatArray = env->FindClass("[Ljava/lang/Float;");
		g_clsFloatArray = (jclass)env->NewGlobalRef((jobject)clsFloatArray);
		//Double[]
		jclass clsDoubleArray = env->FindClass("[Ljava/lang/Double;");
		g_clsDoubleArray = (jclass)env->NewGlobalRef((jobject)clsDoubleArray);
		//Boolean
		jclass clsBoolean = env->FindClass("Ljava/lang/Boolean;");
		g_clsBoolean = (jclass)env->NewGlobalRef((jobject)clsBoolean);
		jmethodID midBooleanCtor = env->GetMethodID(clsBoolean, "<init>", "(Z)V");
		g_midBooleanCtor = (jmethodID)env->NewGlobalRef((jobject)midBooleanCtor);
		jmethodID midBooleanBoolValue = env->GetMethodID(clsBoolean, "booleanValue", "()Z");
		g_midBooleanBoolValue = (jmethodID)env->NewGlobalRef((jobject)midBooleanBoolValue);
		//Byte
		jclass clsByte = env->FindClass("Ljava/lang/Byte;");
		g_clsByte = (jclass)env->NewGlobalRef((jobject)clsByte);
		jmethodID midByteCtor = env->GetMethodID(clsByte, "<init>", "(B)V");
		g_midByteCtor = (jmethodID)env->NewGlobalRef((jobject)midByteCtor);
		jmethodID midByteByteValue = env->GetMethodID(clsByte, "byteValue", "()B");
		g_midByteByteValue = (jmethodID)env->NewGlobalRef((jobject)midByteByteValue);
		//Short
		jclass clsShort = env->FindClass("Ljava/lang/Short;");
		g_clsShort = (jclass)env->NewGlobalRef((jobject)clsShort);
		jmethodID midShortCtor = env->GetMethodID(clsShort, "<init>", "(S)V");
		g_midShortCtor = (jmethodID)env->NewGlobalRef((jobject)midShortCtor);
		jmethodID midShortShortValue = env->GetMethodID(clsShort, "shortValue", "()S");
		g_midShortShortValue = (jmethodID)env->NewGlobalRef((jobject)midShortShortValue);
		//Integer
		jclass clsInteger = env->FindClass("Ljava/lang/Integer;");
		g_clsInteger = (jclass)env->NewGlobalRef((jobject)clsInteger);
		jmethodID midIntegerCtor = env->GetMethodID(clsInteger, "<init>", "(I)V");
		g_midIntegerCtor = (jmethodID)env->NewGlobalRef((jobject)midIntegerCtor);
		jmethodID midIntegerIntValue = env->GetMethodID(clsInteger, "intValue", "()I");
		g_midIntegerIntValue = (jmethodID)env->NewGlobalRef((jobject)midIntegerIntValue);
		//Date
		jclass clsDate = env->FindClass("Ljava/util/Date;");
		g_clsDate = (jclass)env->NewGlobalRef((jobject)clsDate);
		//Long
		jclass clsLong = env->FindClass("Ljava/lang/Long;");
		g_clsLong = (jclass)env->NewGlobalRef((jobject)clsLong);
		jmethodID midLongCtor = env->GetMethodID(clsLong, "<init>", "(J)V");
		g_midLongCtor = (jmethodID)env->NewGlobalRef((jobject)midLongCtor);
		jmethodID midLongLongValue = env->GetMethodID(clsLong, "longValue", "()J");
		g_midLongLongValue = (jmethodID)env->NewGlobalRef((jobject)midLongLongValue);
		//Float
		jclass clsFloat = env->FindClass("Ljava/lang/Float;");
		g_clsFloat = (jclass)env->NewGlobalRef((jobject)clsFloat);
		jmethodID midFloatCtor = env->GetMethodID(clsFloat, "<init>", "(F)V");
		g_midFloatCtor = (jmethodID)env->NewGlobalRef((jobject)midFloatCtor);
		jmethodID midFloatFloatValue = env->GetMethodID(clsFloat, "floatValue", "()F");
		g_midFloatFloatValue = (jmethodID)env->NewGlobalRef((jobject)midFloatFloatValue);
		//Double
		jclass clsDouble = env->FindClass("Ljava/lang/Double;");
		g_clsDouble = (jclass)env->NewGlobalRef((jobject)clsDouble);
		jmethodID midDoubleCtor = env->GetMethodID(clsDouble, "<init>", "(D)V");
		g_midDoubleCtor = (jmethodID)env->NewGlobalRef((jobject)midDoubleCtor);
		jmethodID midDoubleDoubleValue = env->GetMethodID(clsDouble, "doubleValue", "()D");
		g_midDoubleDoubleValue = (jmethodID)env->NewGlobalRef((jobject)midDoubleDoubleValue);
		//ArrayList
		jclass clsArrayList = env->FindClass("Ljava/util/ArrayList;");
		g_clsArrayList = (jclass)env->NewGlobalRef((jobject)clsArrayList);
		jmethodID midArrayListGet = env->GetMethodID(clsArrayList, "get", "(I)Ljava/lang/Object;");
		g_midArrayListGet = (jmethodID)env->NewGlobalRef((jobject)midArrayListGet);
		jmethodID midArrayListAdd = env->GetMethodID(clsArrayList, "add", "(Ljava/lang/Object;)Z");
		g_midArrayListAdd = (jmethodID)env->NewGlobalRef((jobject)midArrayListAdd);
		//ValueQT
		jclass clsValueQT = env->FindClass("Lcom/softing/opc/client/api/ValueQT;");
		g_clsValueQT = (jclass)env->NewGlobalRef((jobject)clsValueQT);
		jmethodID midValueQTCtor = env->GetMethodID(clsValueQT, "<init>", "(Ljava/lang/Object;Lcom/softing/opc/client/enums/EnumQuality;Ljava/util/Date;)V");
		g_midValueQTCtor = (jmethodID)env->NewGlobalRef((jobject)midValueQTCtor);
		jmethodID midValueQTGetQuality = env->GetMethodID(clsValueQT, "getQuality", "()Lcom/softing/opc/client/enums/EnumQuality;");
		g_midValueQTGetQuality = (jmethodID)env->NewGlobalRef((jobject)midValueQTGetQuality);
		jmethodID midValueQTGetData = env->GetMethodID(clsValueQT, "getData", "()Ljava/lang/Object;");
		g_midValueQTGetData = (jmethodID)env->NewGlobalRef((jobject)midValueQTGetData);
		jmethodID midValueQTGetTimestamp = env->GetMethodID(clsValueQT, "getTimeStamp", "()Ljava/util/Date;");
		g_midValueQTGetTimestamp = (jmethodID)env->NewGlobalRef((jobject)midValueQTGetTimestamp);
		//EnumQuality
		jclass clsEnumQuality = env->FindClass("Lcom/softing/opc/client/enums/EnumQuality;");
		g_clsEnumQuality = (jclass)env->NewGlobalRef(clsEnumQuality);
		jmethodID midEnumQualityGetEnum = env->GetStaticMethodID(clsEnumQuality, "getEnum", "(S)Lcom/softing/opc/client/enums/EnumQuality;");
		g_midEnumQualityGetEnum = (jmethodID)env->NewGlobalRef((jobject)midEnumQualityGetEnum);
		jmethodID midEnumQualityToShort = env->GetMethodID(clsEnumQuality, "toShort", "()S");
		g_midEnumQualityToShort = (jmethodID)env->NewGlobalRef((jobject)midEnumQualityToShort);
		//FILETIME
		jclass clsJNIFILETIME = env->FindClass("Lcom/softing/opc/client/impl/jni/JNIFILETIME;");
		g_clsJNIFILETIME = (jclass)env->NewGlobalRef(clsJNIFILETIME);
		jmethodID midJNIFILETIMECtor = env->GetMethodID(clsJNIFILETIME, "<init>", "(Ljava/util/Date;)V");
		g_midJNIFILETIMECtor = (jmethodID)env->NewGlobalRef((jobject)midJNIFILETIMECtor);
		jmethodID midJNIFILETIMECtor2 = env->GetMethodID(clsJNIFILETIME, "<init>", "(II)V");
		g_midJNIFILETIMECtor2 = (jmethodID)env->NewGlobalRef((jobject)midJNIFILETIMECtor2);
		jmethodID midJNIFILETIMEToDate = env->GetMethodID(clsJNIFILETIME, "toDate", "()Ljava/util/Date;");
		g_midJNIFILETIMEToDate = (jmethodID)env->NewGlobalRef((jobject)midJNIFILETIMEToDate);
		//JNIOTCExecutionOptions
		jclass clsJNIOTCExecutionOptions = env->FindClass("Lcom/softing/opc/client/impl/jni/JNIOTCExecutionOptions;");
		g_clsJNIOTCExecutionOptions = (jclass)env->NewGlobalRef((jobject)clsJNIOTCExecutionOptions);
		//JNIOTCallbackFunctions
		jclass clsJNIOTCallbackFunctions = env->FindClass("Lcom/softing/opc/client/impl/jni/JNIOTCallbackFunctions;");
		g_clsJNIOTCallbackFunctions = (jclass)env->NewGlobalRef((jobject)clsJNIOTCallbackFunctions);
		//JNIOTObjectData
		jclass clsJNIOTObjectData = env->FindClass("Lcom/softing/opc/client/impl/jni/JNIOTObjectData;");
		g_clsJNIOTObjectData = (jclass)env->NewGlobalRef((jobject)clsJNIOTObjectData);
		jmethodID midJNIOTObjectDataCtor = env->GetMethodID(clsJNIOTObjectData, "<init>", "(JJ)V");
		g_midJNIOTObjectDataCtor = (jmethodID)env->NewGlobalRef((jobject)midJNIOTObjectDataCtor);
		//JNIOTCObjectContext
		jclass clsJNIOTCObjectContext = env->FindClass("Lcom/softing/opc/client/impl/jni/JNIOTCObjectContext;");
		g_clsJNIOTCObjectContext = (jclass)env->NewGlobalRef((jobject)clsJNIOTCObjectContext);
		jmethodID midJNIOTCObjectContextCtor = env->GetMethodID(clsJNIOTCObjectContext, "<init>", "(Lcom/softing/opc/client/impl/jni/JNIOTObjectData;JLcom/softing/opc/client/impl/jni/JNIOTObjectData;Lcom/softing/opc/client/impl/jni/JNIOTObjectData;)V");
		g_midJNIOTCObjectContextCtor = (jmethodID)env->NewGlobalRef((jobject)midJNIOTCObjectContextCtor);
		//JNIOTCObjectData[]
		jclass clsJNIOTCObjectDataArray = env->FindClass("[Lcom/softing/opc/client/impl/jni/JNIOTObjectData;");
		g_clsJNIOTCObjectDataArray = (jclass)env->NewGlobalRef((jobject)clsJNIOTCObjectDataArray);
		//JNIOTValueData[]
		jclass clsJNIOTValueDataArray = env->FindClass("[Lcom/softing/opc/client/impl/jni/JNIOTValueData;");
		g_clsJNIOTValueDataArray = (jclass)env->NewGlobalRef((jobject)clsJNIOTValueDataArray);
		//String
		jclass clsString = env->FindClass("Ljava/lang/String;");
		g_clsString = (jclass)env->NewGlobalRef((jobject)clsString);
		jmethodID midStringCtor = env->GetMethodID(clsString, "<init>", "([BLjava/nio/charset/Charset;)V");
		g_midStringCtor = (jmethodID)env->NewGlobalRef((jobject)midStringCtor);
		//String[]
		jclass clsStringArray = env->FindClass("[Ljava/lang/String;");
		g_clsStringArray = (jclass)env->NewGlobalRef((jobject)clsStringArray);
		//JNIOTValueData
		jclass clsJNIOTValueData = env->FindClass("Lcom/softing/opc/client/impl/jni/JNIOTValueData;");
		g_clsJNIOTValueData = (jclass)env->NewGlobalRef((jobject)clsJNIOTValueData);
		jmethodID midJNIOTValueDataCtor = env->GetMethodID(clsJNIOTValueData, "<init>", "(Ljava/lang/Object;SLcom/softing/opc/client/impl/jni/JNIFILETIME;)V");
		g_midJNIOTValueDataCtor = (jmethodID)env->NewGlobalRef((jobject)midJNIOTValueDataCtor);
		//ValueData
		jclass clsValueData = env->FindClass("Lcom/softing/opc/client/api/ValueData;");
		g_clsValueData = (jclass)env->NewGlobalRef((jobject)clsValueData);
		jmethodID midValueDataGetVartype = env->GetStaticMethodID(clsValueData, "getVartype", "(Ljava/lang/Object;)S");
		g_midValueDataGetVartype = (jmethodID)env->NewGlobalRef((jobject)midValueDataGetVartype);
		//Charset
		jclass clsCharset = env->FindClass("Ljava/nio/charset/Charset;");
		g_clsCharset = (jclass)env->NewGlobalRef((jobject)clsCharset);
		jmethodID midCharsetForName = env->GetStaticMethodID(clsCharset, "forName", "(Ljava/lang/String;)Ljava/nio/charset/Charset;");
		g_midCharsetForName = (jmethodID)env->NewGlobalRef((jobject)midCharsetForName);
		g_bCacheClsMethodIds = true;
	}
}

/*
* converts a C++ Variant to a JNI Object (jobject)
*/
jobject getJObject(JNIEnv* env, OTVariant* variant, bool* bIsArray)
{
	//DebugBreak();
	jobject result = NULL;

	if (variant->vt == VT_UI1 || variant->vt == VT_I1)
	{
		byte b = variant->bVal;
		result = env->NewObject(g_clsByte, g_midByteCtor, b);
	}
	else if (variant->vt == VT_UI2 || variant->vt == VT_I2)
	{
		short s = variant->iVal;
		result = env->NewObject(g_clsShort, g_midShortCtor, s);
	}
	else if (variant->vt == VT_BOOL)
	{
		short s = variant->boolVal;
		jboolean jb = (s == VARIANT_TRUE) ? true : false;
		result = env->NewObject(g_clsBoolean, g_midBooleanCtor, jb);
	}
	else if (variant->vt == VT_BSTR)
	{
		byte* pbstr = variant->pbVal;
		int length = *((int*)(pbstr - 4));
		jbyteArray bytes = env->NewByteArray(length);
		env->SetByteArrayRegion(bytes, 0, length, (jbyte*)pbstr);
		//jobject charset = env->CallStaticObjectMethod(g_clsCharset, g_midCharsetForName, env->NewStringUTF("UTF-16LE"));// non unicode
		const TCHAR* strenc = _T("UTF-16LE"); //unicode
		jobject charset = env->CallStaticObjectMethod(g_clsCharset, g_midCharsetForName, env->NewString(strenc, wcslen(strenc)));//unicode
		result = env->NewObject(g_clsString, g_midStringCtor, bytes, charset);
		env->DeleteLocalRef(bytes);
		env->DeleteLocalRef(charset); //
	}
	else if (variant->vt == VT_I4 || variant->vt == VT_UI4  || variant->vt == VT_INT || variant->vt == VT_UINT)
	{
		jint i = variant->intVal;
		result = env->NewObject(g_clsInteger, g_midIntegerCtor, i);
	}
	else if (variant->vt == VT_DATE)
	{
		DATE dt = variant->date; //this is a timeval structure
		SYSTEMTIME sysT;
		FILETIME filetime;
		VariantTimeToSystemTime(dt, &sysT);
		SystemTimeToFileTime(&sysT, &filetime);
		int low = filetime.dwLowDateTime;
		int high = filetime.dwHighDateTime;
		jobject ft = env->NewObject(g_clsJNIFILETIME, g_midJNIFILETIMECtor2, low, high);
		result = env->CallObjectMethod(ft, g_midJNIFILETIMEToDate);
		env->DeleteLocalRef(ft); //
	}
	else if (variant->vt == VT_R4)
	{
		float f = variant->fltVal;
		result = env->NewObject(g_clsFloat, g_midFloatCtor, f);
	}
	else if (variant->vt == VT_R8)
	{
		double d = variant->dblVal;
		result = env->NewObject(g_clsDouble, g_midDoubleCtor, d);
	}
	else if (variant->vt == VT_I8 || variant->vt == VT_UI8)
	{
		jlong l = variant->lVal;
		result = env->NewObject(g_clsLong, g_midLongCtor, l);
	}
	else if ((variant->vt & VT_ARRAY) == VT_ARRAY)
	{
		*bIsArray = true;
		void* safearray;
		SafeArrayAccessData(variant->parray, (void**)&safearray);
		int dim = SafeArrayGetDim(variant->parray);
		long uBound;
		long lBound;
		SafeArrayGetLBound(variant->parray, dim, &lBound);
		SafeArrayGetUBound(variant->parray, dim, &uBound);
		int size = uBound - lBound + 1;

		if (variant->vt == (VT_ARRAY | VT_I1) || variant->vt == (VT_ARRAY | VT_UI1))
		{
			jobjectArray joarray = env->NewObjectArray(size, g_clsByte, NULL);

			for (long j = lBound; j <= uBound; j++)
			{
				byte val;
				SafeArrayGetElement(variant->parray, &j, &val);
				jobject jo = env->NewObject(g_clsByte, g_midByteCtor, val);
				env->SetObjectArrayElement(joarray, j, jo);
				//env->DeleteLocalRef(jo); //
			}

			result = (jobject)joarray;
		}
		else if (variant->vt == (VT_ARRAY | VT_I2) || variant->vt == (VT_ARRAY | VT_UI2))
		{
			jobjectArray joarray = env->NewObjectArray(size, g_clsShort, NULL);

			for (long j = lBound; j <= uBound; j++)
			{
				short val;
				SafeArrayGetElement(variant->parray, &j, &val);
				jobject jo = env->NewObject(g_clsShort, g_midShortCtor, val);
				env->SetObjectArrayElement(joarray, j, jo);
				//env->DeleteLocalRef(jo); //
			}

			result = (jobject)joarray;
		}
		else if (variant->vt == (VT_ARRAY | VT_BSTR))
		{
			jobjectArray joarray = env->NewObjectArray(size, g_clsString, NULL);

			for (long j = lBound; j <= uBound; j++)
			{
				BSTR val;
				SafeArrayGetElement(variant->parray, &j, (void*)&val);
				byte* pbstr = (byte*)val;
				int length = *((int*)(pbstr - 4));
				jbyteArray bytes = env->NewByteArray(length);
				env->SetByteArrayRegion(bytes, 0, length, (jbyte*)pbstr);
				//jobject charset = env->CallStaticObjectMethod(g_clsCharset, g_midCharsetForName, env->NewStringUTF("UTF-16LE")); //NON UNICODE
				const TCHAR* strenc = _T("UTF-16LE");
				jobject charset = env->CallStaticObjectMethod(g_clsCharset, g_midCharsetForName, env->NewString(strenc, wcslen(strenc))); //NON UNICODE
				jobject jo = env->NewObject(g_clsString, g_midStringCtor, bytes, charset);
				env->DeleteLocalRef(bytes);
				env->DeleteLocalRef(charset); //
				SysFreeString(val);
				//jobject jo = env->NewStringUTF(OTSysBstrToString(val));
				env->SetObjectArrayElement(joarray, j, jo);
				//env->DeleteLocalRef(jo); //
			}

			result = (jobject)joarray;
		}
		else if (variant->vt == (VT_ARRAY | VT_BOOL))
		{
			jobjectArray joarray = env->NewObjectArray(size, g_clsBoolean, NULL);

			for (long j = lBound; j <= uBound; j++)
			{
				short val;
				SafeArrayGetElement(variant->parray, &j, &val);
				jboolean jb = (val == VARIANT_TRUE) ? true : false;
				jobject jo = env->NewObject(g_clsBoolean, g_midBooleanCtor, jb);
				env->SetObjectArrayElement(joarray, j, jo);
				//env->DeleteLocalRef(jo); //
			}

			result = (jobject)joarray;
		}
		else if (variant->vt == (VT_ARRAY | VT_I4) || variant->vt == (VT_ARRAY | VT_UI4) || variant->vt == (VT_ARRAY | VT_INT) || variant->vt == (VT_ARRAY | VT_UINT))
		{
			jobjectArray joarray = env->NewObjectArray(size, g_clsInteger, NULL);

			for (long j = lBound; j <= uBound; j++)
			{
				int val;
				SafeArrayGetElement(variant->parray, &j, &val);
				jobject jo = env->NewObject(g_clsInteger, g_midIntegerCtor, val);
				env->SetObjectArrayElement(joarray, j, jo);
				//env->DeleteLocalRef(jo); //
			}

			result = (jobject)joarray;
		}
		else if (variant->vt == (VT_ARRAY | VT_DATE))
		{
			jobjectArray joarray = env->NewObjectArray(size, g_clsDate, NULL);

			for (long j = lBound; j <= uBound; j++)
			{
				/*
				DATE dt = variant->date; //this is a timeval structure
				SYSTEMTIME sysT;
				FILETIME filetime;
				VariantTimeToSystemTime(dt, &sysT);
				SystemTimeToFileTime(&sysT, &filetime);

				int low = filetime.dwLowDateTime;
				int high = filetime.dwHighDateTime;

				jobject ft = env->NewObject(g_clsFILETIME, g_midFILETIMECtor2, low, high);
				result = env->CallObjectMethod(ft, g_midFILETIMEToDate);
				*/
				/*long val;
				SafeArrayGetElement(variant->parray, &j, &val);
				jobject ft = env->NewObject(g_clsFILETIME, g_midFILETIMECtor3, val);
				jobject jdt = env->CallObjectMethod(ft, g_midFILETIMEToDate);

				env->SetObjectArrayElement(joarray, j, jdt);
				*/
				VARIANT v;
				SafeArrayGetElement(variant->parray, &j, &(v.date));
				DATE dt = v.date;
				SYSTEMTIME sysT;
				FILETIME filetime;
				VariantTimeToSystemTime(dt, &sysT);
				SystemTimeToFileTime(&sysT, &filetime);
				int low = filetime.dwLowDateTime;
				int high = filetime.dwHighDateTime;
				jobject ft = env->NewObject(g_clsJNIFILETIME, g_midJNIFILETIMECtor2, low, high);
				jobject jdt = env->CallObjectMethod(ft, g_midJNIFILETIMEToDate);
				env->SetObjectArrayElement(joarray, j, jdt);
				env->DeleteLocalRef(ft); //
				//env->DeleteLocalRef(jdt); //
			}

			result = (jobject)joarray;
		}
		else if (variant->vt == (VT_ARRAY | VT_I8) || variant->vt == (VT_ARRAY | VT_UI8))
		{
			jobjectArray joarray = env->NewObjectArray(size, g_clsLong, NULL);

			for (long j = lBound; j <= uBound; j++)
			{
				//long val;
				LARGE_INTEGER li;
				//SafeArrayGetElement(variant->parray, &j, &val);
				SafeArrayGetElement(variant->parray, &j, &li.QuadPart);
				//jobject jo = env->NewObject(g_clsLong, g_midLongCtor, val);
				jobject jo = env->NewObject(g_clsLong, g_midLongCtor, li.QuadPart);
				env->SetObjectArrayElement(joarray, j, jo);
				//env->DeleteLocalRef(jo); //
			}

			result = (jobject)joarray;
		}
		else if (variant->vt == (VT_ARRAY | VT_R4))
		{
			jobjectArray joarray = env->NewObjectArray(size, g_clsFloat, NULL);

			for (long j = lBound; j <= uBound; j++)
			{
				float val;
				SafeArrayGetElement(variant->parray, &j, &val);
				jobject jo = env->NewObject(g_clsFloat, g_midFloatCtor, val);
				env->SetObjectArrayElement(joarray, j, jo);
				//env->DeleteLocalRef(jo); //
			}

			result = (jobject)joarray;
		}
		else if (variant->vt == (VT_ARRAY | VT_R8))
		{
			jobjectArray joarray = env->NewObjectArray(size, g_clsDouble, NULL);

			for (long j = lBound; j <= uBound; j++)
			{
				double val;
				SafeArrayGetElement(variant->parray, &j, &val);
				jobject jo = env->NewObject(g_clsDouble, g_midDoubleCtor, val);
				env->SetObjectArrayElement(joarray, j, jo);
				//env->DeleteLocalRef(jo); //
			}

			result = (jobject)joarray;
		}

		SafeArrayUnaccessData(variant->parray);
	}

	//etc
	return result;
}

void OTCAPI_CALL OnReadWriteComplete(bool bRead, unsigned long executionContextOTB, OTCObjectContext objectContextOTB, long resultOTB, unsigned long itemCountOTB, OTCObjectData* pItemDataOTB, OTChar * *pItemIDsOTB, OTChar * *pItemPathsOTB, OTCValueData* pValuesOTB, long* pResultsOTB)
{
	//DebugBreak();
	JNIEnv* env;
	int res = (*jvm).AttachCurrentThread((void**) &env, NULL);
	//cache class, field, method IDs
	cacheClsMethodIds(env);
	//building call parameters
	jlong executionContext = (long)executionContextOTB;
	jlong result = (long)resultOTB;
	jlong itemCount = (long)itemCountOTB;
	//objectContext
	jobject objectData = env->NewObject(g_clsJNIOTObjectData, g_midJNIOTObjectDataCtor, (jlong)(objectContextOTB.m_objectData.m_objectHandle), (jlong)(objectContextOTB.m_objectData.m_userData));

	if (objectData == NULL)
	{
		goto detach;
	}

	jobject sessionData = env->NewObject(g_clsJNIOTObjectData, g_midJNIOTObjectDataCtor, (jlong)(objectContextOTB.m_sessionData.m_objectHandle), (jlong)(objectContextOTB.m_sessionData.m_userData));

	if (sessionData == NULL)
	{
		goto detach;
	}

	jobject subscriptionData = env->NewObject(g_clsJNIOTObjectData, g_midJNIOTObjectDataCtor, (jlong)(objectContextOTB.m_subscriptionData.m_objectHandle), (jlong)(objectContextOTB.m_subscriptionData.m_userData));

	if (subscriptionData == NULL)
	{
		goto detach;
	}

	jobject objectContext = env->NewObject(g_clsJNIOTCObjectContext, g_midJNIOTCObjectContextCtor, objectData, (jlong)(objectContextOTB.m_objectType), sessionData,  subscriptionData);

	if (objectContext == NULL)
	{
		goto detach;
	}

	//pItemData, pItemIds, pItemPaths
	jobjectArray pItemData = NULL;

	if (pItemDataOTB != NULL)
	{
		pItemData = env->NewObjectArray(itemCountOTB, g_clsJNIOTObjectData, NULL);

		if (pItemData == NULL)
		{
			goto detach;
		}
	}

	jobjectArray pItemIds = env->NewObjectArray(itemCountOTB, g_clsString, NULL);

	if (pItemIds == NULL)
	{
		goto detach;
	}

	jobjectArray pItemPaths = env->NewObjectArray(itemCountOTB, g_clsString, NULL);

	if (pItemPaths == NULL)
	{
		goto detach;
	}

	jobjectArray pValues = NULL;

	if (!bRead && pValuesOTB != NULL
		|| bRead)
	{
		pValues = env->NewObjectArray(itemCountOTB, g_clsValueQT, NULL);

		if (pValues == NULL)
		{
			goto detach;
		}
	}

	jlongArray pResults = env->NewLongArray(itemCountOTB);

	if (pResults == NULL)
	{
		goto detach;
	}

	for (jint i = 0 ; i < itemCountOTB; i++)
	{
		//pItemData
		if (pItemData != NULL)
		{
			jobject objectData = env->NewObject(g_clsJNIOTObjectData, g_midJNIOTObjectDataCtor, (jlong)(pItemDataOTB[i].m_objectHandle), (jlong)(pItemDataOTB[i].m_userData));

			if (objectData == NULL)
			{
				goto detach;
			}

			env->SetObjectArrayElement(pItemData, i, objectData);
		}

		//itemId
		if (pItemIDsOTB != NULL && pItemIDsOTB[i] != NULL)
		{
			//jstring itemId = env->NewStringUTF(pItemIDsOTB[i]); //NON UNICODE
			jstring itemId = env->NewString(pItemIDsOTB[i], wcslen(pItemIDsOTB[i])); //UNICODE

			if (itemId == NULL)
			{
				goto detach;
			}

			env->SetObjectArrayElement(pItemIds, i, itemId);
		}
		else
		{
			const TCHAR* empty = _T("");
			//jstring itemId = env->NewStringUTF("");//NON UNICODE
			jstring itemId = env->NewString(empty, wcslen(empty));//UNICODE

			if (itemId == NULL)
			{
				goto detach;
			}

			env->SetObjectArrayElement(pItemIds, i, itemId);
		}

		//itemPath
		if (pItemPathsOTB != NULL && pItemPathsOTB[i] != NULL)
		{
			//jstring itemPath = env->NewStringUTF(pItemPathsOTB[i]); //NON UNICODE
			jstring itemPath = env->NewString(pItemPathsOTB[i], wcslen(pItemPathsOTB[i])); //UNICODE

			if (itemPath == NULL)
			{
				goto detach;
			}

			env->SetObjectArrayElement(pItemPaths, i, itemPath);
		}
		else
		{
			const TCHAR* empty = _T(""); //unicode
			//jstring itemPath = env->NewStringUTF(""); //NON UNICODE
			jstring itemPath = env->NewString(empty, wcslen(empty)); //UNICODE

			if (itemPath == NULL)
			{
				goto detach;
			}

			env->SetObjectArrayElement(pItemPaths, i, itemPath);
		}

		//valueData
		if (pValues != NULL)
		{
			bool bIsArray = false;
			/////getJObject()
			jobject vjobject = getJObject(env, pValuesOTB[i].m_pValue, &bIsArray);
			/////getJObject()
			OTVariantClear(pValuesOTB[i].m_pValue);
			jobject ft = env->NewObject(g_clsJNIFILETIME, g_midJNIFILETIMECtor2, (jint)pValuesOTB[i].m_timestamp.dwLowDateTime, (jint)pValuesOTB[i].m_timestamp.dwHighDateTime);

			if (ft == NULL)
			{
				goto detach;
			}

			jobject dt = env->CallObjectMethod(ft, g_midJNIFILETIMEToDate);

			if (dt == NULL)
			{
				goto detach;
			}

			jobject vjquality = env->CallStaticObjectMethod(g_clsEnumQuality, g_midEnumQualityGetEnum, (jshort)pValuesOTB[i].m_quality);

			if (vjquality == NULL)
			{
				goto detach;
			}

			jobject value = NULL;

			if (!bIsArray)
			{
				value = env->NewObject(g_clsValueQT, g_midValueQTCtor, vjobject , vjquality, dt);
			}
			else
			{
				value = env->NewObject(g_clsValueQT, g_midValueQTCtor, (jobjectArray)vjobject , vjquality, dt);
			}

			if (value == NULL)
			{
				goto detach;
			}

			env->SetObjectArrayElement(pValues, i, value);
		}
	}

	//pResults
	jlong* tmp = new jlong[itemCountOTB];

	for (jint k = 0 ; k < itemCountOTB; k++)
	{
		tmp[k] = pResultsOTB[k];
	}

	env->SetLongArrayRegion(pResults, 0, itemCountOTB, tmp);
	delete[] tmp;

	//... and the JAVA callback call
	if (bRead)
	{
		env->CallStaticVoidMethod(g_clsCallbackRC, g_midCallbackRC, executionContext, objectContext, result, itemCount, pItemData, pItemIds, pItemPaths, pValues, pResults);
	}
	else
	{
		env->CallStaticVoidMethod(g_clsCallbackWC, g_midCallbackWC, executionContext, objectContext, result, itemCount, pItemData, pItemIds, pItemPaths, pValues, pResults);
	}

	env->DeleteLocalRef(objectContext);
	env->DeleteLocalRef(pItemData);
	env->DeleteLocalRef(pItemIds);
	env->DeleteLocalRef(pItemPaths);
	env->DeleteLocalRef(pValues);
	env->DeleteLocalRef(pResults);
detach:
	(*jvm).DetachCurrentThread();
}

void OTCAPI_CALL JOTBOnReadComplete(IN unsigned long executionContextOTB, IN OTCObjectContext objectContextOTB, IN long resultOTB, IN unsigned long itemCountOTB, IN OTCObjectData* pItemDataOTB, IN OTChar * *pItemIDsOTB, IN OTChar * *pItemPathsOTB, IN OTCValueData* pValuesOTB, IN long* pResultsOTB)
{
	OnReadWriteComplete(true, executionContextOTB, objectContextOTB, resultOTB, itemCountOTB, pItemDataOTB, pItemIDsOTB, pItemPathsOTB, pValuesOTB, pResultsOTB);
}

void OTCAPI_CALL JOTBOnWriteComplete(IN unsigned long executionContextOTB, IN OTCObjectContext objectContextOTB, IN long resultOTB, IN unsigned long itemCountOTB, IN OTCObjectData* pItemDataOTB, IN OTChar * *pItemIDsOTB, IN OTChar * *pItemPathsOTB, IN OTCValueData* pValuesOTB, IN long* pResultsOTB)
{
	OnReadWriteComplete(false, executionContextOTB, objectContextOTB, resultOTB, itemCountOTB, pItemDataOTB, pItemIDsOTB, pItemPathsOTB, pValuesOTB, pResultsOTB);
}

void OTCAPI_CALL JOTBOnDataChange(IN unsigned long executionContextOTB, IN OTCObjectContext objectContextOTB, IN unsigned long countOTB, IN OTCObjectData* pItemDataOTB, IN OTCValueData* pValueDataOTB, IN long* pResultsOTB)
{
	//DebugBreak();
	JNIEnv* env;
	(*jvm).AttachCurrentThread((void**) &env, NULL);
	//cache class, field, method IDs;
	cacheClsMethodIds(env);
	//objectContext
	jobject objectData = env->NewObject(g_clsJNIOTObjectData, g_midJNIOTObjectDataCtor, (jlong)(objectContextOTB.m_objectData.m_objectHandle), (jlong)(objectContextOTB.m_objectData.m_userData));

	if (objectData == NULL)
	{
		goto detach;
	}

	jobject sessionData = env->NewObject(g_clsJNIOTObjectData, g_midJNIOTObjectDataCtor, (jlong)(objectContextOTB.m_sessionData.m_objectHandle), (jlong)(objectContextOTB.m_sessionData.m_userData));

	if (sessionData == NULL)
	{
		goto detach;
	}

	jobject subscriptionData = env->NewObject(g_clsJNIOTObjectData, g_midJNIOTObjectDataCtor, (jlong)(objectContextOTB.m_subscriptionData.m_objectHandle), (jlong)(objectContextOTB.m_subscriptionData.m_userData));

	if (subscriptionData == NULL)
	{
		goto detach;
	}

	jobject objectContext = env->NewObject(g_clsJNIOTCObjectContext, g_midJNIOTCObjectContextCtor, objectData, (jlong)(objectContextOTB.m_objectType), sessionData,  subscriptionData);

	if (objectContext == NULL)
	{
		goto detach;
	}

	//pItemData
	jobjectArray pItemData = env->NewObjectArray(countOTB, g_clsJNIOTObjectData, NULL);

	if (pItemData == NULL)
	{
		goto detach;
	}

	jobjectArray pValueData = env->NewObjectArray(countOTB, g_clsValueQT, NULL);

	if (pValueData == NULL)
	{
		goto detach;
	}

	jlongArray pResults = env->NewLongArray(countOTB);

	if (pResults == NULL)
	{
		goto detach;
	}

	for (jint i = 0 ; i < countOTB; i++)
	{
		//pItemData
		jobject objectData = env->NewObject(g_clsJNIOTObjectData, g_midJNIOTObjectDataCtor, (jlong)(pItemDataOTB[i].m_objectHandle), (jlong)(pItemDataOTB[i].m_userData));

		if (objectData == NULL)
		{
			goto detach;
		}

		env->SetObjectArrayElement(pItemData, i, objectData);
		//valueData
		jobject ft = env->NewObject(g_clsJNIFILETIME, g_midJNIFILETIMECtor2, (jint)pValueDataOTB[i].m_timestamp.dwLowDateTime, (jint)pValueDataOTB[i].m_timestamp.dwHighDateTime);

		if (ft == NULL)
		{
			goto detach;
		}

		jobject dt = env->CallObjectMethod(ft, g_midJNIFILETIMEToDate);

		if (dt == NULL)
		{
			goto detach;
		}

		jobject vjquality = env->CallStaticObjectMethod(g_clsEnumQuality, g_midEnumQualityGetEnum, (jshort)pValueDataOTB[i].m_quality);

		if (vjquality == NULL)
		{
			goto detach;
		}

		bool bIsArray = false;
		jobject vjobject = getJObject(env, pValueDataOTB[i].m_pValue, &bIsArray);
		jobject value = NULL;

		if (!bIsArray)
		{
			value = env->NewObject(g_clsValueQT, g_midValueQTCtor, vjobject, vjquality, dt);
		}
		else
		{
			value = env->NewObject(g_clsValueQT, g_midValueQTCtor, (jobjectArray)vjobject, vjquality, dt);
		}

		if (value == NULL)
		{
			goto detach;
		}

		env->SetObjectArrayElement(pValueData, i, value);
	}

	//pResults
	jlong* tmp = new jlong[countOTB];

	for (jint k = 0 ; k < countOTB; k++)
	{
		tmp[k] = pResultsOTB[k];
	}

	env->SetLongArrayRegion(pResults, 0, countOTB, tmp);
	delete[] tmp;
	jlong count = (jlong)countOTB;
	//the callback to Java
	env->CallStaticVoidMethod(g_clsCallbackDC, g_midCallbackDC, objectContext, count, pItemData, pValueData, pResults);
detach:
	(*jvm).DetachCurrentThread();
}

void ReleaseString(JNIEnv* env, jstring jstr, const jchar* str)
{
	if (jstr != NULL && env->GetStringLength(jstr))
	{
		//env->ReleaseStringUTFChars(jstr, str); //NON UNICODE
		env->ReleaseStringChars(jstr, str); //UNICODE
	}
}

JNIEXPORT jlong JNICALL Java_com_softing_opc_client_impl_jni_JNIOTClient_OTCAdvise(JNIEnv* env, jobject obj, jobject callbackFunctions)
{
	const jchar* strRC = NULL; //unicode; for ANSI use char*
	const jchar* strRM = NULL; //unicode
	const jchar* strWC = NULL; //unicode
	const jchar* strWM = NULL; //unicode
	const jchar* strDC = NULL; //unicode
	const jchar* strDM = NULL; //unicode
	jstring jstringRC = NULL;
	jstring jstringRM = NULL;
	jstring jstringWC = NULL;
	jstring jstringWM = NULL;
	jstring jstringDC = NULL;
	jstring jstringDM = NULL;
	//DebugBreak;
	//cache class, field, method IDs
	cacheClsMethodIds(env);

	//delete old attached global class, method IDS (if Advise is called twice - low probability)
	if (g_clsCallbackRC != NULL)
	{
		env->DeleteGlobalRef((jobject)g_clsCallbackRC);
		g_clsCallbackRC = NULL;
	}

	if (g_midCallbackRC != NULL)
	{
		env->DeleteGlobalRef((jobject)g_midCallbackRC);
		g_midCallbackRC = NULL;
	}

	if (g_clsCallbackWC != NULL)
	{
		env->DeleteGlobalRef((jobject)g_clsCallbackWC);
		g_clsCallbackWC = NULL;
	}

	if (g_midCallbackWC != NULL)
	{
		env->DeleteGlobalRef((jobject)g_midCallbackWC);
		g_midCallbackWC = NULL;
	}

	if (g_clsCallbackDC != NULL)
	{
		env->DeleteGlobalRef((jobject)g_clsCallbackDC);
		g_clsCallbackDC = NULL;
	}

	if (g_midCallbackDC != NULL)
	{
		env->DeleteGlobalRef((jobject)g_midCallbackDC);
		g_midCallbackDC = NULL;
	}

	jfieldID fidonReadCompleteCallbackClass = env->GetFieldID(g_clsJNIOTCallbackFunctions, "onReadCompleteCallbackClass", "Ljava/lang/String;");
	jfieldID fidonReadCompleteCallbackMethod = env->GetFieldID(g_clsJNIOTCallbackFunctions, "onReadCompleteCallbackMethod", "Ljava/lang/String;");
	jfieldID fidonWriteCompleteCallbackClass = env->GetFieldID(g_clsJNIOTCallbackFunctions, "onWriteCompleteCallbackClass", "Ljava/lang/String;");
	jfieldID fidonWriteCompleteCallbackMethod = env->GetFieldID(g_clsJNIOTCallbackFunctions, "onWriteCompleteCallbackMethod", "Ljava/lang/String;");
	jfieldID fidonDataChangeCallbackClass = env->GetFieldID(g_clsJNIOTCallbackFunctions, "onDataChangeCallbackClass", "Ljava/lang/String;");
	jfieldID fidonDataChangeCallbackMethod = env->GetFieldID(g_clsJNIOTCallbackFunctions, "onDataChangeCallbackMethod", "Ljava/lang/String;");
	jstringRC = (jstring)env->GetObjectField(callbackFunctions, fidonReadCompleteCallbackClass);
	jstringRM = (jstring)env->GetObjectField(callbackFunctions, fidonReadCompleteCallbackMethod);
	jstringWC = (jstring)env->GetObjectField(callbackFunctions, fidonWriteCompleteCallbackClass);
	jstringWM = (jstring)env->GetObjectField(callbackFunctions, fidonWriteCompleteCallbackMethod);
	jstringDC = (jstring)env->GetObjectField(callbackFunctions, fidonDataChangeCallbackClass);
	jstringDM = (jstring)env->GetObjectField(callbackFunctions, fidonDataChangeCallbackMethod);
	jboolean isCopy = JNI_FALSE;
	//strRC = (jstringRC != NULL && env->GetStringLength(jstringRC) > 0) ? env->GetStringUTFChars(jstringRC, NULL): NULL; //NON UNICODE
	strRC = (jstringRC != NULL && env->GetStringLength(jstringRC) > 0) ? env->GetStringChars(jstringRC, &isCopy) : NULL; //UNICODE
	//strRM = (jstringRM != NULL && env->GetStringLength(jstringRM) > 0) ? env->GetStringUTFChars(jstringRM, NULL): NULL; //NON UNICODE
	strRM = (jstringRM != NULL && env->GetStringLength(jstringRM) > 0) ? env->GetStringChars(jstringRM, &isCopy) : NULL; //UNICODE
	//strWC = (jstringWC != NULL && env->GetStringLength(jstringWC) > 0) ? env->GetStringUTFChars(jstringWC, NULL): NULL; //NON UNICODE
	strWC = (jstringWC != NULL && env->GetStringLength(jstringWC) > 0) ? env->GetStringChars(jstringWC, &isCopy) : NULL; //UNICODE
	//strWM = (jstringWM != NULL && env->GetStringLength(jstringWM) > 0) ? env->GetStringUTFChars(jstringWM, NULL): NULL; //NON UNICODE
	strWM = (jstringWM != NULL && env->GetStringLength(jstringWM) > 0) ? env->GetStringChars(jstringWM, &isCopy) : NULL; //UNICODE
	//strDC = (jstringDC != NULL && env->GetStringLength(jstringDC) > 0) ? env->GetStringUTFChars(jstringDC, NULL): NULL; //NON UNICODE
	strDC = (jstringDC != NULL && env->GetStringLength(jstringDC) > 0) ? env->GetStringChars(jstringDC, &isCopy) : NULL; //UNICODE
	//strDM = (jstringDM != NULL && env->GetStringLength(jstringDM) > 0) ? env->GetStringUTFChars(jstringDM, NULL): NULL; //NON UNICODE
	strDM = (jstringDM != NULL && env->GetStringLength(jstringDM) > 0) ? env->GetStringChars(jstringDM, &isCopy) : NULL; //UNICODE
	(*env).GetJavaVM(&jvm);  //jv is used later to geta valid JNIEnv reference
	USES_CONVERSION;

	if (strRC != NULL && strRM != NULL)
	{
		jclass clsCallbackRC = env->FindClass(T2A(strRC)); //T2A
		g_clsCallbackRC = (jclass)env->NewGlobalRef((jobject)clsCallbackRC);
		jmethodID midCallbackRC = env->GetStaticMethodID(clsCallbackRC, T2A(strRM), "(JLcom/softing/opc/client/impl/jni/JNIOTCObjectContext;JJ[Lcom/softing/opc/client/impl/jni/JNIOTObjectData;[Ljava/lang/String;[Ljava/lang/String;[Lcom/softing/opc/client/api/ValueQT;[J)V");
		g_midCallbackRC = (jmethodID)env->NewGlobalRef((jobject)midCallbackRC);
	}

	if (strWC != NULL && strWM != NULL)
	{
		jclass clsCallbackWC = env->FindClass(T2A(strWC));
		g_clsCallbackWC = (jclass)env->NewGlobalRef((jobject)clsCallbackWC);
		jmethodID midCallbackWC = env->GetStaticMethodID(clsCallbackWC, T2A(strWM), "(JLcom/softing/opc/client/impl/jni/JNIOTCObjectContext;JJ[Lcom/softing/opc/client/impl/jni/JNIOTObjectData;[Ljava/lang/String;[Ljava/lang/String;[Lcom/softing/opc/client/api/ValueQT;[J)V");
		g_midCallbackWC = (jmethodID)env->NewGlobalRef((jobject)midCallbackWC);
	}

	if (strDC != NULL && strDM != NULL)
	{
		jclass clsCallbackDC = env->FindClass(T2A(strDC));
		g_clsCallbackDC = (jclass)env->NewGlobalRef((jobject)clsCallbackDC);
		jmethodID midCallbackDC = env->GetStaticMethodID(clsCallbackDC, T2A(strDM), "(Lcom/softing/opc/client/impl/jni/JNIOTCObjectContext;J[Lcom/softing/opc/client/impl/jni/JNIOTObjectData;[Lcom/softing/opc/client/api/ValueQT;[J)V");
		g_midCallbackDC = (jmethodID)env->NewGlobalRef((jobject)midCallbackDC);
	}

	OTCCallbackFunctions g_callbacks =
	{
		NULL,
		NULL,
		(strDC != NULL && strDM != NULL) ?& JOTBOnDataChange : NULL,
		(strRC != NULL && strRM != NULL) ?& JOTBOnReadComplete : NULL,
		(strWC != NULL && strWM != NULL) ?& JOTBOnWriteComplete : NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	};
	//freeing global structures for callbacks management
	ReleaseString(env, jstringRC, strRC);
	ReleaseString(env, jstringRM, strRM);
	ReleaseString(env, jstringWC, strWC);
	ReleaseString(env, jstringWM, strWM);
	ReleaseString(env, jstringDC, strDC);
	ReleaseString(env, jstringDM, strDM);
	return OTCAdvise(&g_callbacks);
	//return 0;
}

/*
* Sets a variant from a jobject.
*/
void setVariant(JNIEnv* env, OTVariant* pVariant, jobject obj)
{
	//short vartype = env->CallStaticShortMethod(g_clsValueData, g_midValueDataGetVartype, obj);
	if (env->IsInstanceOf(obj, g_clsByte))
	{
		pVariant->bVal = env->CallByteMethod(obj, g_midByteByteValue);
		pVariant->vt = VT_I1;
	}
	else if (env->IsInstanceOf(obj, g_clsShort))
	{
		pVariant->iVal = env->CallShortMethod(obj, g_midShortShortValue);
		pVariant->vt = VT_I2;
	}
	else if (env->IsInstanceOf(obj, g_clsInteger))
	{
		pVariant->intVal = env->CallIntMethod(obj, g_midIntegerIntValue);
		pVariant->vt = VT_I4;
	}
	else if (env->IsInstanceOf(obj, g_clsString))
	{
		pVariant->vt = VT_BSTR;
		//const char *str = env->GetStringUTFChars((jstring)obj, NULL); //NON UNICODE
		jboolean isCopy = JNI_FALSE; //UNICODE
		const jchar* str = env->GetStringChars((jstring)obj, &isCopy); //unicode
		pVariant->bstrVal = OTSysAllocString((OTChar*)str);
		//env->ReleaseStringUTFChars((jstring)obj, str); //NON UNICODE
		env->ReleaseStringChars((jstring)obj, str);
	}
	else if (env->IsInstanceOf(obj, g_clsBoolean))
	{
		pVariant->boolVal = (jboolean)env->CallBooleanMethod(obj, g_midBooleanBoolValue) ? VARIANT_TRUE : VARIANT_FALSE;
		pVariant->vt = VT_BOOL;
	}
	else if (env->IsInstanceOf(obj, g_clsDate))
	{
		jfieldID fidJNIFILETIMELow = env->GetFieldID(g_clsJNIFILETIME, "dwLowDateTime", "I");
		jfieldID fidJNIFILETIMEHigh = env->GetFieldID(g_clsJNIFILETIME, "dwHighDateTime", "I");
		jobject ft = env->NewObject(g_clsJNIFILETIME, g_midJNIFILETIMECtor, obj); //returns a FILETIME object
		jint high = env->GetIntField(ft, fidJNIFILETIMEHigh);
		jint low =  env->GetIntField(ft, fidJNIFILETIMELow);
		FILETIME filetime;
		filetime.dwHighDateTime = high;
		filetime.dwLowDateTime = low;
		SYSTEMTIME sysT;
		FileTimeToSystemTime(&filetime, &sysT);
		DATE dt;
		SystemTimeToVariantTime(&sysT, &dt);
		pVariant->date = dt;
		pVariant->vt = VT_DATE;
	}
	else if (env->IsInstanceOf(obj, g_clsFloat))
	{
		pVariant->fltVal = env->CallFloatMethod(obj, g_midFloatFloatValue);
		pVariant->vt = VT_R4;
	}
	else if (env->IsInstanceOf(obj, g_clsDouble))
	{
		pVariant->dblVal = env->CallDoubleMethod(obj, g_midDoubleDoubleValue);
		pVariant->vt = VT_R8;
	}
	else if (env->IsInstanceOf(obj, g_clsLong))
	{
		pVariant->lVal = env->CallLongMethod(obj, g_midLongLongValue);
		pVariant->vt = VT_I8;
	}
	else if (env->IsInstanceOf(obj, g_clsByteArray))
	{
		int size = env->GetArrayLength((jbyteArray)obj);
		SAFEARRAYBOUND dim;
		dim.lLbound = 0;
		dim.cElements = size;
		pVariant->vt = VT_I1 | VT_ARRAY;
		pVariant->parray = SafeArrayCreate(pVariant->vt & ~VT_ARRAY, 1, &dim);
		byte buff;

		for (long j = 0; j < size; j++)
		{
			jobject jByteObject = env->GetObjectArrayElement((jobjectArray)obj, j);
			buff = env->CallByteMethod(jByteObject, g_midByteByteValue);
			SafeArrayPutElement(pVariant->parray, &j, &buff);
		}
	}
	else if (env->IsInstanceOf(obj, g_clsShortArray))
	{
		int size = env->GetArrayLength((jshortArray)obj);
		SAFEARRAYBOUND dim;
		dim.lLbound = 0;
		dim.cElements = size;
		pVariant->vt = VT_I2 | VT_ARRAY;
		pVariant->parray = SafeArrayCreate(pVariant->vt & ~VT_ARRAY, 1, &dim);
		short buff;

		//env->GetShortArrayRegion((jshortArray)obj, 0, size, buff);
		for (long j = 0; j < size; j++)
		{
			jobject jShortObject = env->GetObjectArrayElement((jobjectArray)obj, j);
			buff = env->CallShortMethod(jShortObject, g_midShortShortValue);
			SafeArrayPutElement(pVariant->parray, &j, &buff);
		}
	}
	else if (env->IsInstanceOf(obj, g_clsIntegerArray))
	{
		int size = env->GetArrayLength((jintArray)obj);
		SAFEARRAYBOUND dim;
		dim.lLbound = 0;
		dim.cElements = size;
		pVariant->vt = VT_I4 | VT_ARRAY;
		pVariant->parray = SafeArrayCreate(pVariant->vt & ~VT_ARRAY, 1, &dim);
		int buff;

		for (long j = 0; j < size; j++)
		{
			jobject jIntegerObject = env->GetObjectArrayElement((jobjectArray)obj, j);
			buff = env->CallIntMethod(jIntegerObject, g_midIntegerIntValue);
			SafeArrayPutElement(pVariant->parray, &j, &buff);
		}
	}
	else if (env->IsInstanceOf(obj, g_clsBooleanArray))
	{
		int size = env->GetArrayLength((jbooleanArray)obj);
		SAFEARRAYBOUND dim;
		dim.lLbound = 0;
		dim.cElements = size;
		pVariant->vt = VT_BOOL | VT_ARRAY;
		pVariant->parray = SafeArrayCreate(pVariant->vt & ~VT_ARRAY, 1, &dim);
		jboolean buff;
		short sa;

		for (long j = 0; j < size; j++)
		{
			jobject jBooleanObject = env->GetObjectArrayElement((jobjectArray)obj, j);
			buff = env->CallShortMethod(jBooleanObject, g_midBooleanBoolValue);
			sa = buff ? VARIANT_TRUE : VARIANT_FALSE;
			SafeArrayPutElement(pVariant->parray, &j, &sa);
		}
	}
	else if (env->IsInstanceOf(obj, g_clsStringArray))
	{
		int size = env->GetArrayLength((jobjectArray)obj);
		SAFEARRAYBOUND dim;
		dim.lLbound = 0;
		dim.cElements = size;
		pVariant->vt = VT_BSTR | VT_ARRAY;
		pVariant->parray = SafeArrayCreate(pVariant->vt & ~VT_ARRAY, 1, &dim);

		for (long j = 0; j < size; j++)
		{
			jobject jStringObject = env->GetObjectArrayElement((jobjectArray)obj, j);
			//const char *str = env->GetStringUTFChars((jstring)jStringObject, NULL); //NON UNICODE
			jboolean isCopy = JNI_FALSE; //UNICODE
			const jchar* str = env->GetStringChars((jstring)jStringObject, &isCopy); //UNICODE
			BSTR bstr = OTSysAllocString((OTChar*)str);
			SafeArrayPutElement(pVariant->parray, &j, bstr);
			//SafeArrayPutElement(pVariant->parray, &j, (void*)str);
			OTSysFreeString(bstr);
			//env->ReleaseStringUTFChars((jstring)jStringObject, str);// NON UNICODE
			env->ReleaseStringChars((jstring)jStringObject, str);
		}
	}
	else if (env->IsInstanceOf(obj, g_clsDateArray))
	{
		int size = env->GetArrayLength((jobjectArray)obj);
		SAFEARRAYBOUND dim;
		dim.lLbound = 0;
		dim.cElements = size;
		pVariant->vt = VT_DATE | VT_ARRAY;
		pVariant->parray = SafeArrayCreate(pVariant->vt & ~VT_ARRAY, 1, &dim);
		jfieldID fidJNIFILETIMELow = env->GetFieldID(g_clsJNIFILETIME, "dwLowDateTime", "I");
		jfieldID fidJNIFILETIMEHigh = env->GetFieldID(g_clsJNIFILETIME, "dwHighDateTime", "I");

		for (long j = 0; j < size; j++)
		{
			jobject jdt = env->GetObjectArrayElement((jobjectArray)obj, j);
			jobject ft = env->NewObject(g_clsJNIFILETIME, g_midJNIFILETIMECtor, jdt); //returns a FILETIME object
			jint high = env->GetIntField(ft, fidJNIFILETIMEHigh);
			jint low =  env->GetIntField(ft, fidJNIFILETIMELow);
			FILETIME filetime;
			filetime.dwHighDateTime = high;
			filetime.dwLowDateTime = low;
			SYSTEMTIME sysT;
			FileTimeToSystemTime(&filetime, &sysT);
			DOUBLE pvTime;
			SystemTimeToVariantTime(&sysT, &pvTime);
			VARIANT v;
			//SafeArrayPutElement(pVariant->parray, &j, &pvTime);
			v.date = pvTime;
			SafeArrayPutElement(pVariant->parray, &j, &(v.date));
		}
	}
	else if (env->IsInstanceOf(obj, g_clsFloatArray))
	{
		int size = env->GetArrayLength((jfloatArray)obj);
		SAFEARRAYBOUND dim;
		dim.lLbound = 0;
		dim.cElements = size;
		pVariant->vt = VT_R4 | VT_ARRAY;
		pVariant->parray = SafeArrayCreate(pVariant->vt & ~VT_ARRAY, 1, &dim);
		jfloat buff;

		//env->GetFloatArrayRegion((jfloatArray)obj, 0, size, buff);
		for (long j = 0; j < size; j++)
		{
			jobject jFloatObject = env->GetObjectArrayElement((jobjectArray)obj, j);
			buff = env->CallFloatMethod(jFloatObject, g_midFloatFloatValue);
			SafeArrayPutElement(pVariant->parray, &j, &buff);
		}
	}
	else if (env->IsInstanceOf(obj, g_clsDoubleArray))
	{
		int size = env->GetArrayLength((jdoubleArray)obj);
		SAFEARRAYBOUND dim;
		dim.lLbound = 0;
		dim.cElements = size;
		pVariant->vt = VT_R8 | VT_ARRAY;
		pVariant->parray = SafeArrayCreate(pVariant->vt & ~VT_ARRAY, 1, &dim);
		jdouble buff;

		for (long j = 0; j < size; j++)
		{
			jobject jDoubleObject = env->GetObjectArrayElement((jobjectArray)obj, j);
			buff = env->CallDoubleMethod(jDoubleObject, g_midDoubleDoubleValue);
			SafeArrayPutElement(pVariant->parray, &j, &buff);
		}
	}
	else if (env->IsInstanceOf(obj, g_clsLongArray))
	{
		int size = env->GetArrayLength((jlongArray)obj);
		SAFEARRAYBOUND dim;
		dim.lLbound = 0;
		dim.cElements = size;
		pVariant->vt = VT_I8 | VT_ARRAY;
		pVariant->parray = SafeArrayCreate(pVariant->vt & ~VT_ARRAY, 1, &dim);
		jlong buff;

		for (long j = 0; j < size; j++)
		{
			jobject jLongObject = env->GetObjectArrayElement((jobjectArray)obj, j);
			buff = env->CallLongMethod(jLongObject, g_midLongLongValue);
			SafeArrayPutElement(pVariant->parray, &j, &buff);
		}
	}
}




/*
* used conditionally by the Read and Write functions (CONTAINS COMMON CODE)
*/
jlong ReadWrite(bool bRead, JNIEnv* env, jobject obj, jlong objectHandle, jlong count, jlongArray itemHandles, jobjectArray itemIDs, jobjectArray itemPaths, jobject values, jobject results, jobject executionOptions, jlong maxAge)
{
	//cache class, field, method IDs
	cacheClsMethodIds(env);
	//preparing item handle
	OTCObjectHandle* itemHandlesOTB = NULL;
	jlong* bufih = NULL;

	if (itemHandles != NULL)
	{
		itemHandlesOTB = new OTCObjectHandle[count];
		bufih = new jlong[count];
		env->GetLongArrayRegion(itemHandles, 0, count, bufih);

		for (jint i = 0; i < count; i++)
		{
			itemHandlesOTB[i] = bufih[i];
		}
	}

	//prepare itemIDs
	OTChar** itemIDsOTB = NULL;

	if (itemIDs != NULL)
	{
		itemIDsOTB = new OTChar*[count];

		for (jint i = 0; i < count; i++)
		{
			jstring itemID = (jstring)(env->GetObjectArrayElement(itemIDs, i));
			//const char* str = env->GetStringUTFChars( itemID, NULL); //non unicode
			jboolean isCopy = JNI_FALSE;
			const jchar* str = env->GetStringChars(itemID, &isCopy) ;
			//itemIDsOTB[i] = strdup(str); // non unicode
			itemIDsOTB[i] = wcsdup(str);//(OTChar*)OTSysAllocString((OTChar*)str); //UNICODE
			//env->ReleaseStringUTFChars(itemID, str); //non unicode
			env->ReleaseStringChars(itemID, str); //unicode
			env->DeleteLocalRef(itemID);
		}
	}

	//prepare itemPaths
	OTChar** itemPathsOTB = NULL;

	if (itemPaths != NULL)
	{
		itemPathsOTB = new OTChar*[count];

		for (jint i = 0; i < count; i++)
		{
			jstring itemPath = (jstring)(env->GetObjectArrayElement(itemPaths, i));
			//const char* str = env->GetStringUTFChars( itemPath, NULL); //NON UNICODE
			jboolean isCopy = JNI_FALSE;
			const jchar* str = env->GetStringChars(itemPath, &isCopy) ; //UNICODE
			//itemPathsOTB[i] = strdup(str); //(OTChar*)OTSysAllocString((OTChar*)str);//NON UNICODE
			itemPathsOTB[i] = wcsdup(str); //unicode
			//env->ReleaseStringUTFChars(itemPath, str); //non unicode
			env->ReleaseStringChars(itemPath, str); //non unicode
			env->DeleteLocalRef(itemPath);
		}
	}

	//prepare values
	OTCValueData* valuesOTB = new OTCValueData[count];
	jfieldID fidJNIFILETIMELow = env->GetFieldID(g_clsJNIFILETIME, "dwLowDateTime", "I");
	jfieldID fidJNIFILETIMEHigh = env->GetFieldID(g_clsJNIFILETIME, "dwHighDateTime", "I");

	//DebugBreak();
	for (jint i = 0; i < count; i++)
	{
		//set the value
		valuesOTB[i].m_pValue = (OTVariant*)OTAllocateMemory(sizeof(OTVariant));
		OTVariantInit(valuesOTB[i].m_pValue);

		if (!bRead)
		{
			//the returned object it's a ValueQT object:
			jobject valueQT = env->CallObjectMethod(values, g_midArrayListGet, i);
			jobject value = env->CallObjectMethod(valueQT, g_midValueQTGetData);
			setVariant(env, valuesOTB[i].m_pValue, value);
			//set timestamp
			jobject t = env->CallObjectMethod(valueQT, g_midValueQTGetTimestamp);
			jobject ft = env->NewObject(g_clsJNIFILETIME, g_midJNIFILETIMECtor, t); //returns a FILETIME object
			valuesOTB[i].m_timestamp.dwHighDateTime = env->GetIntField(ft, fidJNIFILETIMEHigh);
			valuesOTB[i].m_timestamp.dwLowDateTime =  env->GetIntField(ft, fidJNIFILETIMELow);
			//set quality
			jshort quality = OT_QUALITY_NOT_SET;
			jobject qualEnum = env->CallObjectMethod(valueQT, g_midValueQTGetQuality);
			quality = env->CallShortMethod(qualEnum, g_midEnumQualityToShort);
			valuesOTB[i].m_quality = quality;
		}
	}

	//prepare results
	long* resultsOTB = new long[count];
	//prepare execution options
	jfieldID fidJNIOTCExecutionOptionsExecType = env->GetFieldID(g_clsJNIOTCExecutionOptions, "m_executionType", "B"); //byte
	jfieldID fidJNIOTCExecutionOptionsExecContext = env->GetFieldID(g_clsJNIOTCExecutionOptions, "m_executionContext", "J"); //long
	OTCExecutionOptions executionOptionsOTB;
	jbyte jb = env->GetByteField(executionOptions, fidJNIOTCExecutionOptionsExecType);
	executionOptionsOTB.m_executionType = jb;
	jlong jl = env->GetLongField(executionOptions, fidJNIOTCExecutionOptionsExecContext);
	executionOptionsOTB.m_executionContext = jl;
	long res;
	//The call to the OTB.dll functions

	if (bRead)
	{
		res = OTCRead(objectHandle, count, itemHandlesOTB, itemIDsOTB, itemPathsOTB, maxAge, valuesOTB, resultsOTB, &executionOptionsOTB);
	}
	else
	{
		res = OTCWrite(objectHandle, count, itemHandlesOTB, itemIDsOTB, itemPathsOTB, valuesOTB, resultsOTB, &executionOptionsOTB);
	}

	//pack the results and values back in Java
	if (executionOptionsOTB.m_executionType == OTC_EXECUTIONTYPE_SYCHRONOUS && SUCCEEDED(res))
	{
		for (jint i = 0; i < count; i++)
		{
			jobject resi = env->NewObject(g_clsInteger, g_midIntegerCtor, (jint)resultsOTB[i]);
			env->CallObjectMethod(results, g_midArrayListAdd, resi);
			bool bIsArray = false;
			jobject vjobject = getJObject(env, valuesOTB[i].m_pValue, &bIsArray);
			OTVariantClear(valuesOTB[i].m_pValue);
			jobject vjquality = env->CallStaticObjectMethod(g_clsEnumQuality, g_midEnumQualityGetEnum, (jshort)valuesOTB[i].m_quality);
			jobject ft = env->NewObject(g_clsJNIFILETIME, g_midJNIFILETIMECtor2, (jint)(valuesOTB[i].m_timestamp.dwLowDateTime), (jint)(valuesOTB[i].m_timestamp.dwHighDateTime)); //returns a FILETIME object
			jobject dt = env->CallObjectMethod(ft, g_midJNIFILETIMEToDate);
			jobject vali = NULL;

			if (!bIsArray)
			{
				vali = env->NewObject(g_clsValueQT, g_midValueQTCtor, vjobject, vjquality , dt);
			}
			else
			{
				vali = env->NewObject(g_clsValueQT, g_midValueQTCtor, (jobjectArray)vjobject, vjquality , dt);
			}

			env->CallObjectMethod(values, g_midArrayListAdd, vali);
		}
	}

	//clean
	if (itemIDs != NULL)
	{
		for (jint i = 0; i < count; i++)
		{
			free(itemIDsOTB[i]);
		}
	}

	if (itemPaths != NULL)
	{
		for (jint i = 0; i < count; i++)
		{
			free(itemPathsOTB[i]);
		}
	}

	for (jint i = 0 ; i < count; i++)
	{
		OTFreeMemory(valuesOTB[i].m_pValue);
	}

	delete[] resultsOTB;

	if (bufih != NULL)
	{
		delete[] bufih;
	}

	if (itemHandlesOTB != NULL)
	{
		delete[] itemHandlesOTB;
	}

	delete[] valuesOTB;
	delete[] itemIDsOTB;
	delete[] itemPathsOTB;
	return res;
}

JNIEXPORT jlong JNICALL Java_com_softing_opc_client_impl_jni_JNIOTClient_OTCWrite
(JNIEnv* env, jobject obj, jlong objectHandle, jlong count, jlongArray itemHandles, jobjectArray itemIDs, jobjectArray itemPaths, jobject values, jobject results, jobject executionOptions)
{
	//DebugBreak();
	return ReadWrite(false, env, obj, objectHandle, count, itemHandles, itemIDs, itemPaths, values, results, executionOptions, 0);
}


JNIEXPORT jlong JNICALL Java_com_softing_opc_client_impl_jni_JNIOTClient_OTCRead
(JNIEnv* env, jobject obj, jlong objectHandle, jlong count, jlongArray itemHandles, jobjectArray itemIDs, jobjectArray itemPaths, jlong maxAge, jobject values, jobject results, jobject executionOptions)
{
	return ReadWrite(true, env, obj, objectHandle, count, itemHandles, itemIDs, itemPaths, values, results, executionOptions, maxAge);
}


JNIEXPORT jlong JNICALL Java_com_softing_opc_client_impl_jni_JNIOTClient_OTCTerminate(JNIEnv* env, jobject obj)
{
	//cache class, field, method IDs
	cacheClsMethodIds(env);
	//Delete global references (JNI specific)
	env->DeleteGlobalRef((jobject)g_clsDoubleArray);
	env->DeleteGlobalRef((jobject)g_clsFloatArray);
	env->DeleteGlobalRef((jobject)g_clsLongArray);
	env->DeleteGlobalRef((jobject)g_clsDateArray);
	env->DeleteGlobalRef((jobject)g_clsBooleanArray);
	env->DeleteGlobalRef((jobject)g_clsIntegerArray);
	env->DeleteGlobalRef((jobject)g_clsShortArray);
	env->DeleteGlobalRef((jobject)g_clsByteArray);
	env->DeleteGlobalRef((jobject)g_clsBoolean);
	env->DeleteGlobalRef((jobject)g_midBooleanCtor);
	env->DeleteGlobalRef((jobject)g_midBooleanBoolValue);
	env->DeleteGlobalRef((jobject)g_clsByte);
	env->DeleteGlobalRef((jobject)g_midByteCtor);
	env->DeleteGlobalRef((jobject)g_midByteByteValue);
	env->DeleteGlobalRef((jobject)g_clsShort);
	env->DeleteGlobalRef((jobject)g_midShortCtor);
	env->DeleteGlobalRef((jobject)g_midShortShortValue);
	env->DeleteGlobalRef((jobject)g_clsInteger);
	env->DeleteGlobalRef((jobject)g_midIntegerCtor);
	env->DeleteGlobalRef((jobject)g_midIntegerIntValue);
	env->DeleteGlobalRef((jobject)g_clsDate);
	env->DeleteGlobalRef((jobject)g_clsFloat);
	env->DeleteGlobalRef((jobject)g_midFloatCtor);
	env->DeleteGlobalRef((jobject)g_midFloatFloatValue);
	env->DeleteGlobalRef((jobject)g_clsDouble);
	env->DeleteGlobalRef((jobject)g_midDoubleCtor);
	env->DeleteGlobalRef((jobject)g_midDoubleDoubleValue);
	env->DeleteGlobalRef((jobject)g_clsLong);
	env->DeleteGlobalRef((jobject)g_midLongCtor);
	env->DeleteGlobalRef((jobject)g_midLongLongValue);
	env->DeleteGlobalRef((jobject)g_clsArrayList);
	env->DeleteGlobalRef((jobject)g_midArrayListGet);
	env->DeleteGlobalRef((jobject)g_midArrayListAdd);
	env->DeleteGlobalRef((jobject)g_clsValueQT);
	env->DeleteGlobalRef((jobject)g_midValueQTCtor);
	env->DeleteGlobalRef((jobject)g_midValueQTGetQuality);
	env->DeleteGlobalRef((jobject)g_clsEnumQuality);
	env->DeleteGlobalRef((jobject)g_midEnumQualityGetEnum);
	env->DeleteGlobalRef((jobject)g_midEnumQualityToShort);
	env->DeleteGlobalRef((jobject)g_midValueQTGetData);
	env->DeleteGlobalRef((jobject)g_midValueQTGetTimestamp);
	env->DeleteGlobalRef((jobject)g_clsJNIFILETIME);
	env->DeleteGlobalRef((jobject)g_midJNIFILETIMECtor);
	env->DeleteGlobalRef((jobject)g_midJNIFILETIMECtor2);
	env->DeleteGlobalRef((jobject)g_midJNIFILETIMEToDate);
	env->DeleteGlobalRef((jobject)g_clsJNIOTCExecutionOptions);
	env->DeleteGlobalRef((jobject)g_clsJNIOTCallbackFunctions);

	if (g_clsCallbackRC != NULL)
	{
		env->DeleteGlobalRef((jobject)g_clsCallbackRC);
	}

	if (g_midCallbackRC != NULL)
	{
		env->DeleteGlobalRef((jobject)g_midCallbackRC);
	}

	if (g_clsCallbackWC != NULL)
	{
		env->DeleteGlobalRef((jobject)g_clsCallbackWC);
	}

	if (g_midCallbackWC != NULL)
	{
		env->DeleteGlobalRef((jobject)g_midCallbackWC);
	}

	if (g_clsCallbackDC != NULL)
	{
		env->DeleteGlobalRef((jobject)g_clsCallbackDC);
	}

	if (g_midCallbackDC != NULL)
	{
		env->DeleteGlobalRef((jobject)g_midCallbackDC);
	}

	env->DeleteGlobalRef((jobject)g_clsJNIOTObjectData);
	env->DeleteGlobalRef((jobject)g_midJNIOTObjectDataCtor);
	env->DeleteGlobalRef((jobject)g_clsJNIOTCObjectContext);
	env->DeleteGlobalRef((jobject)g_midJNIOTCObjectContextCtor);
	env->DeleteGlobalRef((jobject)g_clsJNIOTCObjectDataArray);
	env->DeleteGlobalRef((jobject)g_clsJNIOTValueDataArray);
	env->DeleteGlobalRef((jobject)g_clsString);
	env->DeleteGlobalRef((jobject)g_midStringCtor);
	env->DeleteGlobalRef((jobject)g_clsStringArray);
	env->DeleteGlobalRef((jobject)g_clsJNIOTValueData);
	env->DeleteGlobalRef((jobject)g_midJNIOTValueDataCtor);
	env->DeleteGlobalRef((jobject)g_clsValueData);
	env->DeleteGlobalRef((jobject)g_midValueDataGetVartype);
	env->DeleteGlobalRef((jobject)g_clsCharset);
	env->DeleteGlobalRef((jobject)g_midCharsetForName);
	g_bCacheClsMethodIds = false;
	return 0;
	//return OTCTerminate();
}

/*JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    jvm = vm;
    return JNI_VERSION_1_6;
}*/
