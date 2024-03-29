/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_softing_opc_client_impl_jni_JNIOTClient */

#ifndef _Included_com_softing_opc_client_impl_jni_JNIOTClient
#define _Included_com_softing_opc_client_impl_jni_JNIOTClient
#ifdef __cplusplus
extern "C" {
#endif
	/*
	 * Class:     com_softing_opc_client_impl_jni_JNIOTClient
	 * Method:    OTCWrite
	 * Signature: (JJ[J[Ljava/lang/String;[Ljava/lang/String;Ljava/util/ArrayList;Ljava/util/ArrayList;Lcom/softing/opc/client/impl/jni/JNIOTCExecutionOptions;)J
	 */
	JNIEXPORT jlong JNICALL Java_com_softing_opc_client_impl_jni_JNIOTClient_OTCWrite
	(JNIEnv*, jobject, jlong, jlong, jlongArray, jobjectArray, jobjectArray, jobject, jobject, jobject);

	/*
	 * Class:     com_softing_opc_client_impl_jni_JNIOTClient
	 * Method:    OTCRead
	 * Signature: (JJ[J[Ljava/lang/String;[Ljava/lang/String;JLjava/util/ArrayList;Ljava/util/ArrayList;Lcom/softing/opc/client/impl/jni/JNIOTCExecutionOptions;)J
	 */
	JNIEXPORT jlong JNICALL Java_com_softing_opc_client_impl_jni_JNIOTClient_OTCRead
	(JNIEnv*, jobject, jlong, jlong, jlongArray, jobjectArray, jobjectArray, jlong, jobject, jobject, jobject);

	/*
	 * Class:     com_softing_opc_client_impl_jni_JNIOTClient
	 * Method:    OTCTerminate
	 * Signature: ()J
	 */
	JNIEXPORT jlong JNICALL Java_com_softing_opc_client_impl_jni_JNIOTClient_OTCTerminate
	(JNIEnv*, jobject);

	/*
	 * Class:     com_softing_opc_client_impl_jni_JNIOTClient
	 * Method:    OTCAdvise
	 * Signature: (Lcom/softing/opc/client/impl/jni/JNIOTCallbackFunctions;)J
	 */
	JNIEXPORT jlong JNICALL Java_com_softing_opc_client_impl_jni_JNIOTClient_OTCAdvise
	(JNIEnv*, jobject, jobject);


#ifdef __cplusplus
}
#endif
#endif
