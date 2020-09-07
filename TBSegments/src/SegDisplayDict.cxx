// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME srcdISegDisplayDict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "include/SegDisplay.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *SegDisplay_Dictionary();
   static void SegDisplay_TClassManip(TClass*);
   static void delete_SegDisplay(void *p);
   static void deleteArray_SegDisplay(void *p);
   static void destruct_SegDisplay(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SegDisplay*)
   {
      ::SegDisplay *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SegDisplay));
      static ::ROOT::TGenericClassInfo 
         instance("SegDisplay", "SegDisplay.h", 32,
                  typeid(::SegDisplay), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &SegDisplay_Dictionary, isa_proxy, 0,
                  sizeof(::SegDisplay) );
      instance.SetDelete(&delete_SegDisplay);
      instance.SetDeleteArray(&deleteArray_SegDisplay);
      instance.SetDestructor(&destruct_SegDisplay);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SegDisplay*)
   {
      return GenerateInitInstanceLocal((::SegDisplay*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::SegDisplay*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *SegDisplay_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SegDisplay*)0x0)->GetClass();
      SegDisplay_TClassManip(theClass);
   return theClass;
   }

   static void SegDisplay_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrapper around operator delete
   static void delete_SegDisplay(void *p) {
      delete ((::SegDisplay*)p);
   }
   static void deleteArray_SegDisplay(void *p) {
      delete [] ((::SegDisplay*)p);
   }
   static void destruct_SegDisplay(void *p) {
      typedef ::SegDisplay current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SegDisplay

namespace {
  void TriggerDictionaryInitialization_SegDisplayDict_Impl() {
    static const char* headers[] = {
"include/SegDisplay.h",
0
    };
    static const char* includePaths[] = {
"/Work/apps/hipo/hipo4",
"./include",
"/usr/local/apps/root_build/include",
"/Work/CLAS12/Online/TBSegments/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "SegDisplayDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$include/SegDisplay.h")))  SegDisplay;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "SegDisplayDict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "include/SegDisplay.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"SegDisplay", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("SegDisplayDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_SegDisplayDict_Impl, {}, classesHeaders, /*has no C++ module*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_SegDisplayDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_SegDisplayDict() {
  TriggerDictionaryInitialization_SegDisplayDict_Impl();
}
