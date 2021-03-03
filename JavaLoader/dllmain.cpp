// dllmain.cpp : Defines the entry point for the DLL application.


#include "pch.h"

#include <string>

#include "jni.h"

#define JNI_VERSION_1_1 0x00010001
#define JNI_VERSION_1_2 0x00010002
#define JNI_VERSION_1_4 0x00010004
#define JNI_VERSION_1_6 0x00010006
#define JNI_VERSION_1_8 0x00010008
#define JNI_VERSION_9   0x00090000

const std::string MainClass = "Load";
const std::string MainMethod = "MainMethod";
const std::string LibraryAddress = "C:\\aaaa.jar";
const std::string MainMethodSignature = "MainMethodSignature";


void DebugLog(std::string datas) {
    
        printf("[Javaloader]: ");
        printf(datas.c_str());
        printf("\n");
   
}

BOOL GetJVM(JavaVM* &vm, JNIEnv* &jniEnvironment)
{
    jsize vmsAmt;
	//giving it big and large
    JNI_GetCreatedJavaVMs(nullptr, NULL, &vmsAmt); //obtain buffer length so this method is now safe 1!!1!!111
	if (vmsAmt <= 0)
	{
        JavaVMInitArgs vm_args;
        vm_args.version = JNI_VERSION_1_2;
        vm_args.nOptions = 0;
        vm_args.ignoreUnrecognized = false;
        // ReSharper disable once CppCStyleCast
        vm_args.options[0].optionString = (char*)MainClass.c_str();
        vm_args.ignoreUnrecognized = false;
        DebugLog("Creating JVM");
        jint res = JNI_CreateJavaVM(&vm, reinterpret_cast<void**>(&jniEnvironment), &vm_args);
        return FALSE;
	}
    auto* const buff = new JavaVM*[vmsAmt];
    JNI_GetCreatedJavaVMs(buff, vmsAmt, &vmsAmt);
    DebugLog("JVM already exists!");
    vm = buff[0];
    vm->GetEnv(reinterpret_cast<void**>(&jniEnvironment), JNI_VERSION_9);
    return TRUE;
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
    DebugLog("Getting Java VM");
    if (!GetJVM(vm, environment))
    {
        DebugLog("Could not find a JVM so created one");
    }
    else
        DebugLog("Found a JVM");
	
    DebugLog("Finding class....");
    auto* const mainClass = environment->FindClass(MainClass.c_str());
    DebugLog("Finding method.......");
    auto* const main_method = environment->GetStaticMethodID(mainClass, MainMethod.c_str(), MainMethodSignature.c_str());
    DebugLog("Attaching to thread...");
    vm->AttachCurrentThread(reinterpret_cast<void**>(&environment), NULL);
    DebugLog("Invoking method...");
    environment->CallStaticVoidMethod(mainClass, main_method);
    vm->DetachCurrentThread();

}

