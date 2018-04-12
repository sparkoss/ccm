//=========================================================================
// Copyright (C) 2018 The C++ Component Model(CCM) Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __CCM_CCMTYPE_H__
#define __CCM_CCMTYPE_H__

#include "ccmdef.h"
#include "ccmuuid.h"

namespace ccm {

typedef unsigned char   Byte;
typedef short           Short;
typedef int             Integer;
typedef long long int   Long;
typedef float           Float;
typedef double          Double;
typedef int             Char;
typedef bool            Boolean;
typedef uintptr_t       HANDLE;
typedef int             ECode;

typedef Uuid            InterfaceID;
struct ComponentID;
struct CoclassID
{
    Uuid                mUuid;
    const ComponentID*  mCid;
};
struct ComponentID
{
    Uuid                mUuid;
    const char*         mUrl;
};

inline bool operator==(
    /* [in] */ const CoclassID& cid1,
    /* [in] */ const CoclassID& cid2)
{
    return !memcmp(&cid1.mUuid, &cid2.mUuid, sizeof(Uuid));
}

inline bool operator==(
    /* [in] */ const ComponentID& cid1,
    /* [in] */ const ComponentID& cid2)
{
    return !memcmp(&cid1.mUuid, &cid2.mUuid, sizeof(Uuid));
}

}

#include "ccmerror.h"
#include "ccmstring.h"
#include "ccmintfs.h"
#include "ccmtypekind.h"

namespace ccm {

extern String DumpUuid(
    /* [in] */ const Uuid& id);

template<class T>
struct Type2Kind
{
    inline static CcmTypeKind Kind();
};
template<class T>
CcmTypeKind Type2Kind<T>::Kind()
{
    return CcmTypeKind::Unknown;
}

#define TYPE2KIND_SPEC(type, kind) \
        template<> \
        struct Type2Kind<type> \
        { inline static CcmTypeKind Kind(); }; \
        CcmTypeKind Type2Kind<type>::Kind() \
        { return kind; }

TYPE2KIND_SPEC(Byte, CcmTypeKind::Byte);
TYPE2KIND_SPEC(Short, CcmTypeKind::Short);
TYPE2KIND_SPEC(Integer, CcmTypeKind::Integer);
TYPE2KIND_SPEC(Long, CcmTypeKind::Long);
TYPE2KIND_SPEC(Float, CcmTypeKind::Float);
TYPE2KIND_SPEC(Double, CcmTypeKind::Double);
TYPE2KIND_SPEC(Boolean, CcmTypeKind::Boolean);
TYPE2KIND_SPEC(String, CcmTypeKind::String);
TYPE2KIND_SPEC(HANDLE, CcmTypeKind::HANDLE);
TYPE2KIND_SPEC(CoclassID, CcmTypeKind::CoclassID);
TYPE2KIND_SPEC(InterfaceID, CcmTypeKind::InterfaceID);
TYPE2KIND_SPEC(IInterface*, CcmTypeKind::Interface);

template<class T, class U>
class Conversion
{
    typedef char Small;
    class Big { char dummy[2]; };
    static Small Test(U);
    static Big Test(...);
    static T MakeT();
public:
    enum { exists = sizeof(Test(MakeT())) == sizeof(Small) };
    enum { exists2Way = exists && Conversion<U, T>::exists };
    enum { sameType = false };
};

template<class T>
class Conversion<T, T>
{
    enum { exists = 1, exists2Way = 1, sameType = 1 };
};

#define SUPERSUBCLASS(T, U)                         \
    (Conversion<const U*, const T*>::exists &&      \
     !Conversion<const T*, const void*>::sameType)

#define SUPERSUBCLASS_STRICT(T, U)                  \
    (SUPERSUBCLASS(T,U) &&                          \
     !Conversion<const T, const U>::sameType)

}

#include "ccmarray.h"

#endif // __CCM_CCMTYPE_H__
