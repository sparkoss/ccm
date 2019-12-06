//=========================================================================
// Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project
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

//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
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

#include "comorpc.h"
#include "CProxy.h"
#include "util/comolog.h"
#include <sys/mman.h>

namespace como {

#ifndef PAGE_SIZE
#define PAGE_SIZE (1u << 12)
#endif
#ifndef PAGE_MASK
#define PAGE_MASK (~(PAGE_SIZE - 1))
#endif
#ifndef PAGE_ALIGN
#define PAGE_ALIGN(va) (((va) + PAGE_SIZE - 1) & PAGE_MASK)
#endif

#if defined(__aarch64__)

#define GET_REG(reg, var)

#define GET_XREG(reg, var)

#define GET_RBP(var)

#define GET_STACK(rsp, off, var)

EXTERN_C void __entry();

__asm__(
    ".text;"
    ".align 8;"
    ".global __entry;"
    "__entry:"
    "stp    lr, x1, [sp, #-16];"
    "mov    w1, #0xff;"
    "ldr    lr, [x0, #8];"
    "br     lr;"
);

#elif defined(__x86_64__)

#define GET_REG(reg, var)       \
    __asm__ __volatile__(       \
        "mov    %%"#reg", %0;"  \
        : "=m"(var)             \
    )

#define GET_XREG(reg, var)      \
    __asm__ __volatile__(       \
        "movsd  %%"#reg", %0;"  \
        : "=m"(var)             \
    )

#define GET_RBP(var)                \
    __asm__ __volatile__(           \
        "mov    (%%rbp), %%rax;"    \
        "mov    %%rax, %0;"         \
        : "=m"(var)                 \
    );

#define GET_STACK(rsp, off, var)        \
    __asm__ __volatile__(               \
        "mov    %1, %%rax;"             \
        "mov    %2, %%ebx;"             \
        "add    %%rbx, %%rax;"          \
        "mov    (%%rax), %%rax;"        \
        "mov    %%rax, %0;"             \
        : "=m"(var)                     \
        : "m"(rsp)                      \
        , "m"(off)                      \
    )

EXTERN_C void __entry();

EXTERN_C void __end();

__asm__(
    ".text;"
    ".align 8;"
    ".global __entry;"
    "__entry:"
    "pushq  %rbp;"
    "movq   %rsp, %rbp;"
    "mov    $0xff, %bx;"
    "call   *8(%rdi);"
    "movq   %rbp, %rsp;"
    "popq   %rbp;"
    "ret;"
);

#endif

HANDLE PROXY_ENTRY = 0;

static constexpr Integer PROXY_ENTRY_SIZE = 16;
static constexpr Integer PROXY_ENTRY_SHIFT = 4;
static constexpr Integer PROXY_ENTRY_NUMBER = 240;
#if defined(__aarch64__)
static constexpr Integer PROXY_INDEX_OFFSET = 8;
#elif defined(__x86_64__)
static constexpr Integer PROXY_INDEX_OFFSET = 6;
#endif

static constexpr Integer METHOD_MAX_NUMBER = PROXY_ENTRY_NUMBER + 4;

static HANDLE sProxyVtable[METHOD_MAX_NUMBER];

void Init_Proxy_Entry()
{
    PROXY_ENTRY = reinterpret_cast<HANDLE>(mmap(nullptr,
            PAGE_ALIGN(PROXY_ENTRY_SIZE * PROXY_ENTRY_NUMBER),
            PROT_READ | PROT_WRITE | PROT_EXEC,
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (PROXY_ENTRY == 0) {
        Logger::E("CProxy", "Mmap PROXY_ENTRY failed.\n");
        return;
    }

#if defined(__aarch64__)
    Byte* p = (Byte*)PROXY_ENTRY;
    for (Integer i = 0; i < PROXY_ENTRY_NUMBER; i++) {
        memcpy(p, reinterpret_cast<void*>(&__entry), PROXY_ENTRY_SIZE);
        p[8] = i;
        p += PROXY_ENTRY_SIZE;
    }
#elif defined(__x86_64__)
    Byte* p = (Byte*)PROXY_ENTRY;
    for (Integer i = 0; i < PROXY_ENTRY_NUMBER; i++) {
        memcpy(p, reinterpret_cast<void*>(&__entry), PROXY_ENTRY_SIZE);
        *(Integer*)(p + PROXY_INDEX_OFFSET) = i;
        p += PROXY_ENTRY_SIZE;
    }
#endif

    sProxyVtable[0] = reinterpret_cast<HANDLE>(&InterfaceProxy::S_AddRef);
    sProxyVtable[1] = reinterpret_cast<HANDLE>(&InterfaceProxy::S_Release);
    sProxyVtable[2] = reinterpret_cast<HANDLE>(&InterfaceProxy::S_Probe);
    sProxyVtable[3] = reinterpret_cast<HANDLE>(&InterfaceProxy::S_GetInterfaceID);
    for (Integer i = 4; i < METHOD_MAX_NUMBER; i++) {
        sProxyVtable[i] = PROXY_ENTRY + ((i - 4) << PROXY_ENTRY_SHIFT);
    }
}

void Uninit_Proxy_Entry()
{
    if (PROXY_ENTRY != 0) {
        munmap(reinterpret_cast<void*>(PROXY_ENTRY),
                PAGE_ALIGN(PROXY_ENTRY_SIZE * PROXY_ENTRY_NUMBER));
        PROXY_ENTRY = 0;
    }
}

//----------------------------------------------------------------------------------

Integer InterfaceProxy::AddRef(
    /* [in] */ HANDLE id)
{
    return 1;
}

Integer InterfaceProxy::Release(
    /* [in] */ HANDLE)
{
    return 1;
}

Integer InterfaceProxy::S_AddRef(
    /* [in] */ InterfaceProxy* thisObj,
    /* [in] */ HANDLE id)
{
    return thisObj->mOwner->AddRef(id);
}

Integer InterfaceProxy::S_Release(
    /* [in] */ InterfaceProxy* thisObj,
    /* [in] */ HANDLE id)
{
    return thisObj->mOwner->Release(id);
}

IInterface* InterfaceProxy::S_Probe(
    /* [in] */ InterfaceProxy* thisObj,
    /* [in] */ const InterfaceID& iid)
{
    return thisObj->mOwner->Probe(iid);
}

ECode InterfaceProxy::S_GetInterfaceID(
    /* [in] */ InterfaceProxy* thisObj,
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID& iid)
{
    return thisObj->mOwner->GetInterfaceID(object, iid);
}

ECode InterfaceProxy::MarshalArguments(
    /* [in] */ Registers& regs,
    /* [in] */ IMetaMethod* method,
    /* [in] */ IParcel* argParcel)
{
    Integer N;
    method->GetParameterNumber(N);
    Integer intNum = 1, fpNum = 0;
    for (Integer i = 0; i < N; i++) {
        AutoPtr<IMetaParameter> param;
        method->GetParameter(i, param);
        AutoPtr<IMetaType> type;
        param->GetType(type);
        TypeKind kind;
        type->GetTypeKind(kind);
        IOAttribute ioAttr;
        param->GetIOAttribute(ioAttr);
        if (ioAttr == IOAttribute::IN) {
            switch (kind) {
                case TypeKind::Char: {
                    Char value = (Char)GetLongValue(regs, intNum++, fpNum);
                    argParcel->WriteChar(value);
                    break;
                }
                case TypeKind::Byte: {
                    Byte value = (Byte)GetLongValue(regs, intNum++, fpNum);
                    argParcel->WriteByte(value);
                    break;
                }
                case TypeKind::Short: {
                    Short value = (Short)GetLongValue(regs, intNum++, fpNum);
                    argParcel->WriteShort(value);
                    break;
                }
                case TypeKind::Integer: {
                    Integer value = (Integer)GetLongValue(regs, intNum++, fpNum);
                    argParcel->WriteInteger(value);
                    break;
                }
                case TypeKind::Long: {
                    Long value = GetLongValue(regs, intNum++, fpNum);
                    argParcel->WriteLong(value);
                    break;
                }
                case TypeKind::Float: {
                    Float value = (Float)GetDoubleValue(regs, intNum, fpNum++);
                    argParcel->WriteFloat(value);
                    break;
                }
                case TypeKind::Double: {
                    Double value = GetDoubleValue(regs, intNum, fpNum++);
                    argParcel->WriteDouble(value);
                    break;
                }
                case TypeKind::Boolean: {
                    Boolean value = (Boolean)GetLongValue(regs, intNum++, fpNum);
                    argParcel->WriteBoolean(value);
                    break;
                }
                case TypeKind::String: {
                    String value = *reinterpret_cast<String*>(GetLongValue(regs, intNum++, fpNum));
                    argParcel->WriteString(value);
                    break;
                }
                case TypeKind::ECode: {
                    ECode value = (ECode)GetLongValue(regs, intNum++, fpNum);
                    argParcel->WriteECode(value);
                    break;
                }
                case TypeKind::Enum: {
                    Integer value = (Integer)GetLongValue(regs, intNum++, fpNum);
                    argParcel->WriteEnumeration(value);
                    break;
                }
                case TypeKind::Array: {
                    AutoPtr<IMetaType> aType, eType;
                    eType = type;
                    TypeKind eKind = kind;
                    while (eKind == TypeKind::Array) {
                        aType = eType;
                        aType->GetElementType(eType);
                        eType->GetTypeKind(eKind);
                    }
                    if (eKind == TypeKind::CoclassID ||
                            eKind == TypeKind::ComponentID ||
                            eKind == TypeKind::InterfaceID ||
                            eKind == TypeKind::HANDLE) {
                        Logger::E("CProxy", "Invalid [in] Array(%d), param index: %d.\n", eKind, i);
                        return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }

                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argParcel->WriteArray(value);
                    break;
                }
                case TypeKind::Interface: {
                    IInterface* value = reinterpret_cast<IInterface*>(GetLongValue(regs, intNum++, fpNum));
                    argParcel->WriteInterface(value);
                    break;
                }
                case TypeKind::CoclassID:
                case TypeKind::ComponentID:
                case TypeKind::InterfaceID:
                case TypeKind::HANDLE:
                case TypeKind::Triple:
                default:
                    Logger::E("CProxy", "Invalid [in] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (ioAttr == IOAttribute::IN_OUT) {
            switch (kind) {
                case TypeKind::Char: {
                    Char* value = reinterpret_cast<Char*>(GetLongValue(regs, intNum++, fpNum));
                    argParcel->WriteChar(*value);
                    break;
                }
                case TypeKind::Byte: {
                    Byte* value = reinterpret_cast<Byte*>(GetLongValue(regs, intNum++, fpNum));
                    argParcel->WriteByte(*value);
                    break;
                }
                case TypeKind::Short: {
                    Short* value = reinterpret_cast<Short*>(GetLongValue(regs, intNum++, fpNum));
                    argParcel->WriteShort(*value);
                    break;
                }
                case TypeKind::Integer: {
                    Integer* value = reinterpret_cast<Integer*>(GetLongValue(regs, intNum++, fpNum));
                    argParcel->WriteInteger(*value);
                    break;
                }
                case TypeKind::Long: {
                    Long* value = reinterpret_cast<Long*>(GetLongValue(regs, intNum++, fpNum));
                    argParcel->WriteLong(*value);
                    break;
                }
                case TypeKind::Float: {
                    Float* value = reinterpret_cast<Float*>(GetLongValue(regs, intNum++, fpNum));
                    argParcel->WriteFloat(*value);
                    break;
                }
                case TypeKind::Double: {
                    Double* value = reinterpret_cast<Double*>(GetLongValue(regs, intNum++, fpNum));
                    argParcel->WriteDouble(*value);
                    break;
                }
                case TypeKind::Boolean: {
                    Boolean* value = reinterpret_cast<Boolean*>(GetLongValue(regs, intNum++, fpNum));
                    argParcel->WriteBoolean(*value);
                    break;
                }
                case TypeKind::String: {
                    String* value = reinterpret_cast<String*>(GetLongValue(regs, intNum++, fpNum));
                    argParcel->WriteString(*value);
                    break;
                }
                case TypeKind::ECode: {
                    ECode* value = reinterpret_cast<ECode*>(GetLongValue(regs, intNum++, fpNum));
                    argParcel->WriteECode(*value);
                    break;
                }
                case TypeKind::Enum: {
                    Integer* value = reinterpret_cast<Integer*>(GetLongValue(regs, intNum++, fpNum));
                    argParcel->WriteInteger(*value);
                    break;
                }
                case TypeKind::Array: {
                    AutoPtr<IMetaType> aType, eType;
                    eType = type;
                    TypeKind eKind = kind;
                    while (eKind == TypeKind::Array) {
                        aType = eType;
                        aType->GetElementType(eType);
                        eType->GetTypeKind(eKind);
                    }
                    if (eKind == TypeKind::CoclassID ||
                            eKind == TypeKind::ComponentID ||
                            eKind == TypeKind::InterfaceID ||
                            eKind == TypeKind::HANDLE) {
                        Logger::E("CProxy", "Invalid [in, out] Array(%d), param index: %d.\n", eKind, i);
                        return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }

                    HANDLE value = (HANDLE)GetLongValue(regs, intNum++, fpNum);
                    argParcel->WriteArray(value);
                    break;
                }
                case TypeKind::Interface: {
                    IInterface** value = reinterpret_cast<IInterface**>(GetLongValue(regs, intNum++, fpNum));
                    argParcel->WriteInterface(*value);
                    break;
                }
                case TypeKind::CoclassID:
                case TypeKind::ComponentID:
                case TypeKind::InterfaceID:
                case TypeKind::HANDLE:
                case TypeKind::Triple:
                default:
                    Logger::E("CProxy", "Invalid [in, out] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (ioAttr == IOAttribute::OUT) {
            switch (kind) {
                case TypeKind::Char:
                case TypeKind::Byte:
                case TypeKind::Short:
                case TypeKind::Integer:
                case TypeKind::Long:
                case TypeKind::Float:
                case TypeKind::Double:
                case TypeKind::Boolean:
                case TypeKind::String:
                case TypeKind::ECode:
                case TypeKind::Enum:
                case TypeKind::Interface:
                    intNum++;
                    break;
                case TypeKind::Array: {
                    AutoPtr<IMetaType> aType, eType;
                    eType = type;
                    TypeKind eKind = kind;
                    while (eKind == TypeKind::Array) {
                        aType = eType;
                        aType->GetElementType(eType);
                        eType->GetTypeKind(eKind);
                    }
                    if (eKind == TypeKind::CoclassID ||
                            eKind == TypeKind::ComponentID ||
                            eKind == TypeKind::InterfaceID ||
                            eKind == TypeKind::HANDLE) {
                        Logger::E("CProxy", "Invalid [out] Array(%d), param index: %d.\n", eKind, i);
                        return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }

                    intNum++;
                    break;
                }
                case TypeKind::CoclassID:
                case TypeKind::ComponentID:
                case TypeKind::InterfaceID:
                case TypeKind::HANDLE:
                case TypeKind::Triple:
                default:
                    Logger::E("CProxy", "Invalid [out] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (ioAttr == IOAttribute::OUT_CALLEE) {
            switch (kind) {
                case TypeKind::Array: {
                    AutoPtr<IMetaType> aType, eType;
                    eType = type;
                    TypeKind eKind = kind;
                    while (eKind == TypeKind::Array) {
                        aType = eType;
                        aType->GetElementType(eType);
                        eType->GetTypeKind(eKind);
                    }
                    if (eKind == TypeKind::CoclassID ||
                            eKind == TypeKind::ComponentID ||
                            eKind == TypeKind::InterfaceID ||
                            eKind == TypeKind::HANDLE) {
                        Logger::E("CProxy", "Invalid [out, callee] Array(%d), param index: %d.\n", eKind, i);
                        return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }

                    intNum++;
                    break;
                }
                case TypeKind::Char:
                case TypeKind::Byte:
                case TypeKind::Short:
                case TypeKind::Integer:
                case TypeKind::Long:
                case TypeKind::Float:
                case TypeKind::Double:
                case TypeKind::Boolean:
                case TypeKind::String:
                case TypeKind::ECode:
                case TypeKind::Enum:
                case TypeKind::Interface:
                case TypeKind::CoclassID:
                case TypeKind::ComponentID:
                case TypeKind::InterfaceID:
                case TypeKind::HANDLE:
                case TypeKind::Triple:
                default:
                    Logger::E("CProxy", "Invalid [out, callee] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }

    return NOERROR;
}

ECode InterfaceProxy::UnmarshalResults(
        /* [in] */ Registers& regs,
        /* [in] */ IMetaMethod* method,
        /* [in] */ IParcel* resParcel)
{
    Integer N;
    method->GetParameterNumber(N);
    Integer intNum = 1, fpNum = 0;
    for (Integer i = 0; i < N; i++) {
        AutoPtr<IMetaParameter> param;
        method->GetParameter(i, param);
        AutoPtr<IMetaType> type;
        param->GetType(type);
        TypeKind kind;
        type->GetTypeKind(kind);
        IOAttribute ioAttr;
        param->GetIOAttribute(ioAttr);
        if (ioAttr == IOAttribute::IN) {
            switch (kind) {
                case TypeKind::Char:
                case TypeKind::Byte:
                case TypeKind::Short:
                case TypeKind::Integer:
                case TypeKind::Long:
                case TypeKind::Boolean:
                case TypeKind::String:
                case TypeKind::ECode:
                case TypeKind::Enum:
                case TypeKind::Array:
                case TypeKind::Interface:
                    intNum++;
                    break;
                case TypeKind::Float:
                case TypeKind::Double:
                    fpNum++;
                    break;
                case TypeKind::CoclassID:
                case TypeKind::ComponentID:
                case TypeKind::InterfaceID:
                case TypeKind::HANDLE:
                case TypeKind::Triple:
                default:
                    Logger::E("CProxy", "Invalid [in] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (ioAttr == IOAttribute::OUT || ioAttr == IOAttribute::IN_OUT) {
            switch (kind) {
                case TypeKind::Char: {
                    Char* addr = reinterpret_cast<Char*>(
                            GetValueAddress(regs, intNum++, fpNum));
                    resParcel->ReadChar(*addr);
                    break;
                }
                case TypeKind::Byte: {
                    Byte* addr = reinterpret_cast<Byte*>(
                            GetValueAddress(regs, intNum++, fpNum));
                    resParcel->ReadByte(*addr);
                    break;
                }
                case TypeKind::Short: {
                    Short* addr = reinterpret_cast<Short*>(
                            GetValueAddress(regs, intNum++, fpNum));
                    resParcel->ReadShort(*addr);
                    break;
                }
                case TypeKind::Integer: {
                    Integer* addr = reinterpret_cast<Integer*>(
                            GetValueAddress(regs, intNum++, fpNum));
                    resParcel->ReadInteger(*addr);
                    break;
                }
                case TypeKind::Long: {
                    Long* addr = reinterpret_cast<Long*>(
                            GetValueAddress(regs, intNum++, fpNum));
                    resParcel->ReadLong(*addr);
                    break;
                }
                case TypeKind::Float: {
                    Float* addr = reinterpret_cast<Float*>(
                            GetValueAddress(regs, intNum++, fpNum));
                    resParcel->ReadFloat(*addr);
                    break;
                }
                case TypeKind::Double: {
                    Double* addr = reinterpret_cast<Double*>(
                            GetValueAddress(regs, intNum++, fpNum));
                    resParcel->ReadDouble(*addr);
                    break;
                }
                case TypeKind::Boolean: {
                    Boolean* addr = reinterpret_cast<Boolean*>(
                            GetValueAddress(regs, intNum++, fpNum));
                    resParcel->ReadBoolean(*addr);
                    break;
                }
                case TypeKind::String: {
                    String* addr = reinterpret_cast<String*>(
                            GetValueAddress(regs, intNum++, fpNum));
                    resParcel->ReadString(*addr);
                    break;
                }
                case TypeKind::ECode: {
                    ECode* addr = reinterpret_cast<ECode*>(
                            GetValueAddress(regs, intNum++, fpNum));
                    resParcel->ReadECode(*addr);
                    break;
                }
                case TypeKind::Enum: {
                    Integer* addr = reinterpret_cast<Integer*>(
                            GetValueAddress(regs, intNum++, fpNum));
                    resParcel->ReadEnumeration(*addr);
                    break;
                }
                case TypeKind::Array: {
                    Triple* t = reinterpret_cast<Triple*>(
                            GetValueAddress(regs, intNum++, fpNum));
                    resParcel->ReadArray(reinterpret_cast<HANDLE>(t));
                    break;
                }
                case TypeKind::Interface: {
                    AutoPtr<IInterface>* intf = reinterpret_cast<AutoPtr<IInterface>*>(
                            GetValueAddress(regs, intNum++, fpNum));
                    resParcel->ReadInterface(*intf);
                    break;
                }
                case TypeKind::CoclassID:
                case TypeKind::ComponentID:
                case TypeKind::InterfaceID:
                case TypeKind::HANDLE:
                case TypeKind::Triple:
                default:
                    Logger::E("CProxy", "Invalid [in, out] or [out] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else if (ioAttr == IOAttribute::OUT_CALLEE) {
            switch (kind) {
                case TypeKind::Array: {
                    Triple* t = reinterpret_cast<Triple*>(
                            GetValueAddress(regs, intNum++, fpNum));
                    resParcel->ReadArray(reinterpret_cast<HANDLE>(t));
                    break;
                }
                case TypeKind::Char:
                case TypeKind::Byte:
                case TypeKind::Short:
                case TypeKind::Integer:
                case TypeKind::Long:
                case TypeKind::Float:
                case TypeKind::Double:
                case TypeKind::Boolean:
                case TypeKind::String:
                case TypeKind::ECode:
                case TypeKind::Enum:
                case TypeKind::Interface:
                case TypeKind::CoclassID:
                case TypeKind::ComponentID:
                case TypeKind::InterfaceID:
                case TypeKind::Triple:
                default:
                    Logger::E("CProxy", "Invalid [out, callee] type(%d), param index: %d.\n", kind, i);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }

    return NOERROR;
}

Long InterfaceProxy::GetLongValue(
    /* [in] */ Registers& regs,
    /* [in] */ Integer intIndex,
    /* [in] */ Integer fpIndex)
{
    switch (intIndex) {
        case 0:
            return regs.rdi;
        case 1:
            return regs.rsi;
        case 2:
            return regs.rdx;
        case 3:
            return regs.rcx;
        case 4:
            return regs.r8;
        case 5:
            return regs.r9;
        default: {
            Long val;
            Integer off = fpIndex <= 7 ? (intIndex - 5 + 1) * 8 :
                    (intIndex - 5 + fpIndex - 8 + 1) * 8;
            GET_STACK(regs.rbp, off, val);
            return val;
        }
    }
}

Double InterfaceProxy::GetDoubleValue(
    /* [in] */ Registers& regs,
    /* [in] */ Integer intIndex,
    /* [in] */ Integer fpIndex)
{
    switch (fpIndex) {
        case 0:
            return regs.xmm0;
        case 1:
            return regs.xmm1;
        case 2:
            return regs.xmm2;
        case 3:
            return regs.xmm3;
        case 4:
            return regs.xmm4;
        case 5:
            return regs.xmm5;
        case 6:
            return regs.xmm6;
        case 7:
            return regs.xmm7;
        default: {
            Double val;
            Integer off = intIndex <= 5 ? (fpIndex - 7 + 1) * 8 :
                    (fpIndex - 7 + intIndex - 6 + 1) * 8;
            GET_STACK(regs.rbp, off, val);
            return val;
        }
    }
}

HANDLE InterfaceProxy::GetValueAddress(
    /* [in] */ Registers& regs,
    /* [in] */ Integer intIndex,
    /* [in] */ Integer fpIndex)
{
    switch (intIndex) {
        case 0:
            return static_cast<HANDLE>(regs.rdi);
        case 1:
            return static_cast<HANDLE>(regs.rsi);
        case 2:
            return static_cast<HANDLE>(regs.rdx);
        case 3:
            return static_cast<HANDLE>(regs.rcx);
        case 4:
            return static_cast<HANDLE>(regs.r8);
        case 5:
            return static_cast<HANDLE>(regs.r9);
        default: {
            Long val;
            Integer off = fpIndex <= 7 ? (intIndex - 5 + 1) * 8 :
                    (intIndex - 5 + fpIndex - 8 + 1) * 8;
            GET_STACK(regs.rbp, off, val);
            return static_cast<HANDLE>(val);
        }
    }
}

ECode InterfaceProxy::ProxyEntry(
    /* [in] */ HANDLE args)
{
    InterfaceProxy* thisObj = reinterpret_cast<InterfaceProxy*>(args);

    Integer methodIndex;
    GET_REG(ebx, methodIndex);

    Registers regs;
    GET_REG(rbp, regs.rbp);
    // GET_RBP(regs.rbp);
    GET_REG(rdi, regs.rdi);
    GET_REG(rsi, regs.rsi);
    GET_REG(rdx, regs.rdx);
    GET_REG(rcx, regs.rcx);
    GET_REG(r8, regs.r8);
    GET_REG(r9, regs.r9);

    GET_XREG(xmm0, regs.xmm0);
    GET_XREG(xmm1, regs.xmm1);
    GET_XREG(xmm2, regs.xmm2);
    GET_XREG(xmm3, regs.xmm3);
    GET_XREG(xmm4, regs.xmm4);
    GET_XREG(xmm5, regs.xmm5);
    GET_XREG(xmm6, regs.xmm6);
    GET_XREG(xmm7, regs.xmm7);

    if (DEBUG) {
        String name, ns;
        thisObj->mTargetMetadata->GetName(name);
        thisObj->mTargetMetadata->GetNamespace(ns);
        Logger::D("CProxy", "Call ProxyEntry with interface \"%s%s\"",
                ns.string(), name.string());
    }

    AutoPtr<IMetaMethod> method;
    thisObj->mTargetMetadata->GetMethod(methodIndex + 4, method);

    if (DEBUG) {
        String name, signature;
        method->GetName(name);
        method->GetSignature(signature);
        Logger::D("CProxy", "Call ProxyEntry with method \"%s(%s)\"",
                name.string(), signature.string());
    }

    RPCType type;
    thisObj->mOwner->mChannel->GetRPCType(type);
    AutoPtr<IParcel> inParcel, outParcel;
    CoCreateParcel(type, inParcel);
    inParcel->WriteInteger(RPC_MAGIC_NUMBER);
    inParcel->WriteInteger(thisObj->mIndex);
    inParcel->WriteInteger(methodIndex + 4);
    ECode ec = thisObj->MarshalArguments(regs, method, inParcel);
    if (FAILED(ec)) {
        goto ProxyExit;
    }

    ec = thisObj->mOwner->mChannel->Invoke(
            thisObj->mOwner, method, inParcel, outParcel);
    if (FAILED(ec)) {
        goto ProxyExit;
    }

    ec = thisObj->UnmarshalResults(regs, method, outParcel);

ProxyExit:
    if (DEBUG) {
        Logger::D("CProxy", "Exit ProxyEntry with ec(0x%x)", ec);
    }

    return ec;
}

//----------------------------------------------------------------------

const CoclassID CID_CProxy =
        {{0x228c4e6a,0x1df5,0x4130,0xb46e,{0xd0,0x32,0x2b,0x67,0x69,0x76}}, &CID_COMORuntime};

COMO_OBJECT_IMPL(CProxy);

CProxy::~CProxy()
{
    for (Integer i = 0; i < mInterfaces.GetLength(); i++) {
        InterfaceProxy* iproxy = mInterfaces[i];
        mInterfaces[i] = nullptr;
        delete iproxy;
    }
}

Integer CProxy::AddRef(
    /* [in] */ HANDLE id)
{
    return Object::AddRef(id);
}

Integer CProxy::Release(
    /* [in] */ HANDLE id)
{
    return Object::Release(id);
}

IInterface* CProxy::Probe(
    /* [in] */ const InterfaceID& iid)
{
    if (IID_IInterface == iid) {
        return (IObject*)this;
    }
    else if (IID_IObject == iid) {
        return (IObject*)this;
    }
    else if (IID_IProxy == iid) {
        return (IProxy*)this;
    }
    for (Integer i = 0; i < mInterfaces.GetLength(); i++) {
        InterfaceProxy* iproxy = mInterfaces[i];
        if (iproxy->mIid == iid) {
            return reinterpret_cast<IInterface*>(&iproxy->mVtable);
        }
    }
    return nullptr;
}

ECode CProxy::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID& iid)
{
    if (object == (IObject*)this) {
        iid = IID_IObject;
        return NOERROR;
    }
    for (Integer i = 0; i < mInterfaces.GetLength(); i++) {
        InterfaceProxy* iproxy = mInterfaces[i];
        if ((IInterface*)iproxy == object) {
            iid = iproxy->mIid;
            return NOERROR;
        }
    }
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CProxy::GetTargetCoclass(
    /* [out] */ AutoPtr<IMetaCoclass>& target)
{
    target = mTargetMetadata;
    return NOERROR;
}

ECode CProxy::IsStubAlive(
    /* [out] */ Boolean& alive)
{
    return mChannel->IsPeerAlive(alive);
}

ECode CProxy::LinkToDeath(
    /* [in] */ IDeathRecipient* recipient,
    /* [in] */ HANDLE cookie,
    /* [in] */ Integer flags)
{
    return mChannel->LinkToDeath(recipient, cookie, flags);
}

ECode CProxy::UnlinkToDeath(
    /* [in] */ IDeathRecipient* recipient,
    /* [in] */ HANDLE cookie,
    /* [in] */ Integer flags,
    /* [out] */ AutoPtr<IDeathRecipient>* outRecipient)
{
    return mChannel->UnlinkToDeath(recipient, cookie, flags, outRecipient);
}

AutoPtr<IRPCChannel> CProxy::GetChannel()
{
    return mChannel;
}

CoclassID CProxy::GetTargetCoclassID()
{
    return mCid;
}

ECode CProxy::CreateObject(
    /* [in] */ const CoclassID& cid,
    /* [in] */ IRPCChannel* channel,
    /* [out] */ AutoPtr<IProxy>& proxy)
{
    proxy = nullptr;

    AutoPtr<IMetaCoclass> mc;
    CoGetCoclassMetadata(cid, nullptr, mc);

    AutoPtr<CProxy> proxyObj = new CProxy();
    mc->GetCoclassID(proxyObj->mCid);
    proxyObj->mTargetMetadata = mc;
    proxyObj->mChannel = channel;

    Integer interfaceNumber;
    mc->GetInterfaceNumber(interfaceNumber);
    Array<IMetaInterface*> interfaces(interfaceNumber);
    mc->GetAllInterfaces(interfaces);
    proxyObj->mInterfaces = Array<InterfaceProxy*>(interfaceNumber);
    for (Integer i = 0; i < interfaceNumber; i++) {
        AutoPtr<InterfaceProxy> iproxy = new InterfaceProxy();
        iproxy->mIndex = i;
        iproxy->mOwner = proxyObj;
        iproxy->mTargetMetadata = interfaces[i];
        iproxy->mTargetMetadata->GetInterfaceID(iproxy->mIid);
        iproxy->mVtable = sProxyVtable;
        iproxy->mProxyEntry = reinterpret_cast<HANDLE>(&InterfaceProxy::ProxyEntry);
        proxyObj->mInterfaces[i] = iproxy;
    }

    proxy = proxyObj;
    return NOERROR;
}

} // namespace como
