// dllmain.cpp : Defines the entry point for the DLL application.


#include "pch.h"

#include <string>

#include "jni.h"

const auto MainClass = "Load";
const auto MainMethod = "MainMethod";
const auto LibraryAddress = "C:\\aaaa.jar";
const auto MainMethodSignature = "MainMethodSignature";


void DebugLog(std::string datas) {
    
        printf("[Javaloader]: ");
        printf(datas.c_str());
        printf("\n");
   
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{

    //this entire experience was cancer and I'm honestly looking forward to going back into the safe open arms of .net
	
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    DebugLog("Preparing JVM and environment");
    JavaVM* vm;
    JNIEnv* environment;
    JavaVMInitArgs vm_args;
    vm_args.version = JNI_VERSION_1_2;
    vm_args.nOptions = 0;
    vm_args.ignoreUnrecognized = false;
	vm_args.options[0].optionString = const_cast<char*>(LibraryAddress);
    vm_args.ignoreUnrecognized = false;
    DebugLog("Creating JVM");
    jint res = JNI_CreateJavaVM(&vm, reinterpret_cast<void**>(&environment), &vm_args);
    DebugLog("Finding class....");
    const jclass mainClass = environment->FindClass(MainClass);
    DebugLog("Finding method.......");
    const jmethodID mainMethod = environment->GetStaticMethodID(mainClass, MainMethod, MainMethodSignature);
    DebugLog("Invoking method...");
    environment->CallStaticVoidMethod(mainClass, mainMethod);

}

