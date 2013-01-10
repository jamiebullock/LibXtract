
%include "../xtract.i"
%include "arrays_java.i"

%apply double[] {const double *data};
%apply double *OUTPUT { double *result };
%typemap(in,numinputs=0) JNIEnv *env "$1 = jenv;"

%javamethodmodifiers arr2voidd "private";
%javamethodmodifiers arr2voidi "private";
%javamethodmodifiers freearrd "private";
%javamethodmodifiers freearri "private";

%inline %{
jlong arr2voidd(JNIEnv *env, jdoubleArray arr) {
  void *ptr = (*env)->GetFloatArrayElements(env, arr, NULL);
  return (intptr_t)ptr;
}

void freearrd(JNIEnv *env, jdoubleArray arr, jlong map) {
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
    if (o instanceof double[]) {
      return arr2voidd((double[])o);
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
    if (o instanceof double[]) {
      freearrd((double[])o, addr);
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

%include xtract_redeclare.i

