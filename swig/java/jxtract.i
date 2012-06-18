
%include "../xtract.i"
%include "arrays_java.i"

/*
%{
int mean(const float *data, const int N, void *in, float *result) {
    return xtract_mean(data, N, in, result);
}
%}
*/
%apply float[] {const float *data};
/*%apply void *in {const void *argv}; */
%apply float *OUTPUT { float *result };
%typemap(in,numinputs=0) JNIEnv *env "$1 = jenv;"

%javamethodmodifiers arr2voidf "private";
%javamethodmodifiers arr2voidi "private";
%javamethodmodifiers freearrf "private";
%javamethodmodifiers freearri "private";
%inline %{
jlong arr2voidf(JNIEnv *env, jfloatArray arr) {
  void *ptr = (*env)->GetFloatArrayElements(env, arr, NULL);
  return (intptr_t)ptr;
}

void freearrf(JNIEnv *env, jfloatArray arr, jlong map) {
  void *ptr = 0;
  ptr = *(void **)&map;
  (*env)->ReleaseFloatArrayElements(env, arr, ptr, JNI_ABORT);
}

jlong arr2voidi(JNIEnv *env, jintArray arr) {
  void *ptr = (*env)->GetIntArrayElements(env, arr, NULL);
  return (intptr_t)ptr;
}

void freearri(JNIEnv *env, jintArray arr, jlong map) {
  void *ptr = 0;
  ptr = *(void **)&map;
  (*env)->ReleaseIntArrayElements(env, arr, ptr, JNI_ABORT);
}
%}


%pragma(java) modulecode=%{
  private static long arrPtr(Object o) {
    if (o instanceof float[]) {
      return arr2voidf((float[])o);
    }
    else if (o instanceof int[]) {
      return arr2voidi((int[])o);
    }
    else if (o == null) {
        return 0L;
    }
    throw new IllegalArgumentException();
  }

  private static void freeArrPtr(Object o, long addr) {
    if (o instanceof float[]) {
      freearrf((float[])o, addr);
      return;
    }
    else if (o instanceof int[]) {
      freearri((int[])o, addr);
      return;
    }
    else if (o == null) {
        return;
    }
    throw new IllegalArgumentException();
  }
%}

%typemap(jstype) void *argv "Object"
%typemap(javain,pre="    long tmp$javainput = arrPtr($javainput);",post="      freeArrPtr($javainput, tmp$javainput);") void *argv "tmp$javainput"

/*
int xtract_mean(const float *data, const int N, void *argv, float *result);
int xtract_variance(const float *data, const int N, void *argv, float *result);
*/
%include xtract_redeclare.i

/*%{
#include "xtract/xtract_scalar.h"
%}*/
