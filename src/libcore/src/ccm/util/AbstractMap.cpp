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

#include "ccm/core/CStringBuilder.h"
#include "ccm/util/AbstractCollection.h"
#include "ccm/util/AbstractMap.h"
#include "ccm/util/AbstractSet.h"
#include "ccm.util.IIterator.h"
#include "ccm.util.IMapEntry.h"
#include "ccm.util.ISet.h"
#include <ccmautoptr.h>

using ccm::core::CStringBuilder;
using ccm::core::IStringBuilder;
using ccm::core::IID_IStringBuilder;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_1(AbstractMap, SyncObject, IMap);

ECode AbstractMap::GetSize(
    /* [out] */ Integer* size)
{
    VALIDATE_NOT_NULL(size);

    AutoPtr<ISet> entries;
    GetEntrySet(&entries);
    return entries->GetSize(size);
}

ECode AbstractMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Integer size;
    GetSize(&size);
    *result = size == 0;
    return NOERROR;
}

ECode AbstractMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ISet> entries;
    GetEntrySet(&entries);
    AutoPtr<IIterator> it;
    entries->GetIterator(&it);
    if (value == nullptr) {
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> e;
            it->Next(&e);
            AutoPtr<IInterface> v;
            if (IMapEntry::Probe(e)->GetValue(&v),
                    v == nullptr) {
                *result = true;
                return NOERROR;
            }
        }
    }
    else {
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> e;
            it->Next(&e);
            AutoPtr<IInterface> v;
            if (IMapEntry::Probe(e)->GetValue(&v),
                    Object::Equals(value, v)) {
                *result = true;
                return NOERROR;
            }
        }
    }
    *result = false;
    return NOERROR;
}

ECode AbstractMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ISet> entries;
    GetEntrySet(&entries);
    AutoPtr<IIterator> it;
    entries->GetIterator(&it);
    if (key == nullptr) {
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> e;
            it->Next(&e);
            AutoPtr<IInterface> k;
            if (IMapEntry::Probe(e)->GetKey(&k),
                    k == nullptr) {
                *result = true;
                return NOERROR;
            }
        }
    }
    else {
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> e;
            it->Next(&e);
            AutoPtr<IInterface> k;
            if (IMapEntry::Probe(e)->GetKey(&k),
                    Object::Equals(key, k)) {
                *result = true;
                return NOERROR;
            }
        }
    }
    *result = false;
    return NOERROR;
}

ECode AbstractMap::Get(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<ISet> entries;
    GetEntrySet(&entries);
    AutoPtr<IIterator> it;
    entries->GetIterator(&it);
    if (key == nullptr) {
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> e;
            it->Next(&e);
            AutoPtr<IInterface> k;
            if (IMapEntry::Probe(e)->GetKey(&k),
                    k == nullptr) {
                return IMapEntry::Probe(e)->GetValue(value);
            }
        }
    }
    else {
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> e;
            it->Next(&e);
            AutoPtr<IInterface> k;
            if (IMapEntry::Probe(e)->GetKey(&k),
                    Object::Equals(key, k)) {
                return IMapEntry::Probe(e)->GetValue(value);
            }
        }
    }
    *value = nullptr;
    return NOERROR;
}

ECode AbstractMap::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** prevValue)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode AbstractMap::Remove(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** prevValue)
{
    VALIDATE_NOT_NULL(prevValue);

    AutoPtr<IMapEntry> correctEntry;
    AutoPtr<ISet> entries;
    GetEntrySet(&entries);
    AutoPtr<IIterator> it;
    entries->GetIterator(&it);
    if (key == nullptr) {
        Boolean hasNext;
        while (correctEntry == nullptr &&
                (it->HasNext(&hasNext), hasNext)) {
            AutoPtr<IInterface> e;
            it->Next(&e);
            AutoPtr<IInterface> k;
            if (IMapEntry::Probe(e)->GetKey(&k),
                    k == nullptr) {
                correctEntry = IMapEntry::Probe(e);
            }
        }
    }
    else {
        Boolean hasNext;
        while (correctEntry == nullptr &&
                (it->HasNext(&hasNext), hasNext)) {
            AutoPtr<IInterface> e;
            it->Next(&e);
            AutoPtr<IInterface> k;
            if (IMapEntry::Probe(e)->GetKey(&k),
                    Object::Equals(key, k)) {
                correctEntry = IMapEntry::Probe(e);
            }
        }
    }

    if (correctEntry != nullptr) {
        if (prevValue != nullptr) {
            correctEntry->GetValue(prevValue);
            REFCOUNT_ADD(*prevValue);
        }
        it->Remove();
    }
    else {
        if (prevValue != nullptr) *prevValue = nullptr;
    }
    return NOERROR;
}

ECode AbstractMap::PutAll(
    /* [in] */ IMap* m)
{
    AutoPtr<ISet> entries;
    m->GetEntrySet(&entries);
    AutoPtr<IIterator> it;
    entries->GetIterator(&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> e;
        it->Next(&e);
        AutoPtr<IInterface> k, v;
        IMapEntry::Probe(e)->GetKey(&k);
        IMapEntry::Probe(e)->GetValue(&v);
        Put(k, v);
    }
    return NOERROR;
}

ECode AbstractMap::PutIfAbsent(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** prevValue)
{
    AutoPtr<IInterface> v;
    Get(key, &v);
    if (v == nullptr) {
        Put(key, value, &v);
    }

    if (prevValue != nullptr) {
        *prevValue = v;
        REFCOUNT_ADD(*prevValue);
    }
    return NOERROR;
}

ECode AbstractMap::Clear()
{
    AutoPtr<ISet> entries;
    GetEntrySet(&entries);
    return entries->Clear();
}

ECode AbstractMap::GetKeySet(
    /* [out] */ ISet** keys)
{
    VALIDATE_NOT_NULL(keys);

    AutoPtr<ISet> ks = mKeySet;
    if (ks == nullptr) {
        class _Set
            : public AbstractSet
        {
        public:
            _Set(
                /* [in] */ AbstractMap* owner)
                : mOwner(owner)
            {}

            ECode GetIterator(
                /* [out] */ IIterator** it) override
            {
                VALIDATE_NOT_NULL(it);

                class _Iterator
                    : public LightRefBase
                    , public IIterator
                {
                public:
                    _Iterator(
                        /* [in] */ IIterator* i)
                        : mIt(i)
                    {}

                    Integer AddRef(
                        /* [in] */ HANDLE id = 0) override
                    {
                        return LightRefBase::AddRef(id);
                    }

                    Integer Release(
                        /* [in] */ HANDLE id = 0) override
                    {
                        return LightRefBase::Release(id);
                    }

                    IInterface* Probe(
                        /* [in] */ const InterfaceID& iid) override
                    {
                        if (iid == IID_IInterface) {
                            return (IInterface*)(IIterator*)this;
                        }
                        else if (iid == IID_IIterator) {
                            return (IIterator*)this;
                        }
                        return nullptr;
                    }

                    ECode GetInterfaceID(
                        /* [in] */ IInterface* object,
                        /* [out] */ InterfaceID* iid) override
                    {
                        VALIDATE_NOT_NULL(iid);

                        if (object == (IInterface*)(IIterator*)this) {
                            *iid = IID_IIterator;
                            return NOERROR;
                        }
                        return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }

                    ECode Next(
                        /* [out] */ IInterface** object = nullptr) override
                    {
                        AutoPtr<IInterface> e;
                        mIt->Next(&e);
                        return object != nullptr ?
                                IMapEntry::Probe(e)->GetKey(object) : NOERROR;
                    }

                    ECode HasNext(
                        /* [out] */ Boolean* result) override
                    {
                        return mIt->HasNext(result);
                    }

                    ECode Remove() override
                    {
                        return mIt->Remove();
                    }

                private:
                    AutoPtr<IIterator> mIt;
                };

                AutoPtr<ISet> entries;
                mOwner->GetEntrySet(&entries);
                AutoPtr<IIterator> eit;
                entries->GetIterator(&eit);
                *it = new _Iterator(eit);
                return NOERROR;
            }

            ECode GetSize(
                /* [out] */ Integer* size) override
            {
                return mOwner->GetSize(size);
            }

            ECode IsEmpty(
                /* [out] */ Boolean* result) override
            {
                return mOwner->IsEmpty(result);
            }

            ECode Clear() override
            {
                return mOwner->Clear();
            }

            ECode Contains(
                /* [in] */ IInterface* obj,
                /* [out] */ Boolean* result) override
            {
                return mOwner->ContainsKey(obj, result);
            }

        private:
            AbstractMap* mOwner;
        };
        mKeySet = ks = new _Set(this);
    }
    *keys = ks;
    REFCOUNT_ADD(*keys);
    return NOERROR;
}

ECode AbstractMap::GetValues(
    /* [out] */ ICollection** values)
{
    VALIDATE_NOT_NULL(values);

    AutoPtr<ICollection> vals = mValues;
    if (vals == nullptr) {
        class _Collection
            : public AbstractCollection
        {
        public:
            _Collection(
                /* [in] */ AbstractMap* owner)
                : mOwner(owner)
            {}

            ECode GetIterator(
                /* [out] */ IIterator** it) override
            {
                VALIDATE_NOT_NULL(it);

                class _Iterator
                    : public LightRefBase
                    , public IIterator
                {
                public:
                    _Iterator(
                        /* [in] */ IIterator* i)
                        : mIt(i)
                    {}

                    Integer AddRef(
                        /* [in] */ HANDLE id = 0) override
                    {
                        return LightRefBase::AddRef(id);
                    }

                    Integer Release(
                        /* [in] */ HANDLE id = 0) override
                    {
                        return LightRefBase::Release(id);
                    }

                    IInterface* Probe(
                        /* [in] */ const InterfaceID& iid) override
                    {
                        if (iid == IID_IInterface) {
                            return (IInterface*)(IIterator*)this;
                        }
                        else if (iid == IID_IIterator) {
                            return (IIterator*)this;
                        }
                        return nullptr;
                    }

                    ECode GetInterfaceID(
                        /* [in] */ IInterface* object,
                        /* [out] */ InterfaceID* iid) override
                    {
                        VALIDATE_NOT_NULL(iid);

                        if (object == (IInterface*)(IIterator*)this) {
                            *iid = IID_IIterator;
                            return NOERROR;
                        }
                        return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }

                    ECode Next(
                        /* [out] */ IInterface** object = nullptr) override
                    {
                        AutoPtr<IInterface> e;
                        mIt->Next(&e);
                        return object != nullptr ?
                                IMapEntry::Probe(e)->GetValue(object) : NOERROR;
                    }

                    ECode HasNext(
                        /* [out] */ Boolean* result) override
                    {
                        return mIt->HasNext(result);
                    }

                    ECode Remove() override
                    {
                        return mIt->Remove();
                    }

                private:
                    AutoPtr<IIterator> mIt;
                };

                AutoPtr<ISet> entries;
                mOwner->GetEntrySet(&entries);
                AutoPtr<IIterator> eit;
                entries->GetIterator(&eit);
                *it = new _Iterator(eit);
                return NOERROR;
            }

            ECode GetSize(
                /* [out] */ Integer* size) override
            {
                return mOwner->GetSize(size);
            }

            ECode IsEmpty(
                /* [out] */ Boolean* result) override
            {
                return mOwner->IsEmpty(result);
            }

            ECode Clear() override
            {
                return mOwner->Clear();
            }

            ECode Contains(
                /* [in] */ IInterface* obj,
                /* [out] */ Boolean* result) override
            {
                return mOwner->ContainsValue(obj, result);
            }

        private:
            AbstractMap* mOwner;
        };
        mValues = vals = new _Collection(this);
    }
    *values = vals;
    REFCOUNT_ADD(*values);
    return NOERROR;
}

ECode AbstractMap::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (IInterface::Equals(obj, (IMap*)this)) {
        *result = true;
        return NOERROR;
    }

    if (IMap::Probe(obj) == nullptr) {
        *result = false;
        return NOERROR;
    }
    IMap* m = IMap::Probe(obj);
    Integer othSize, thisSize;
    if (m->GetSize(&othSize), GetSize(&thisSize), othSize != thisSize) {
        *result = false;
        return NOERROR;
    }

    AutoPtr<ISet> entries;
    GetEntrySet(&entries);
    AutoPtr<IIterator> it;
    entries->GetIterator(&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> e;
        it->Next(&e);
        AutoPtr<IInterface> key, value;
        IMapEntry::Probe(e)->GetKey(&key);
        IMapEntry::Probe(e)->GetValue(&value);
        if (value == nullptr) {
            AutoPtr<IInterface> othValue;
            Boolean constains;
            if ((m->Get(key, &othValue), othValue != nullptr) ||
                    (m->ContainsKey(key, &constains), !constains)) {
                *result = false;
                return NOERROR;
            }
        }
        else {
            AutoPtr<IInterface> othValue;
            if (m->Get(key, &othValue), !Object::Equals(value, othValue)) {
                *result = false;
                return NOERROR;
            }
        }
    }

    *result = true;
    return NOERROR;
}

ECode AbstractMap::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    Integer h = 0;
    AutoPtr<ISet> entries;
    GetEntrySet(&entries);
    AutoPtr<IIterator> it;
    entries->GetIterator(&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> e;
        it->Next(&e);
        h += Object::GetHashCode(e);
    }
    *hash = h;
    return NOERROR;
}

ECode AbstractMap::ToString(
    /* [out] */ String* str)
{
    AutoPtr<ISet> entries;
    GetEntrySet(&entries);
    AutoPtr<IIterator> it;
    entries->GetIterator(&it);
    Boolean hasNext;
    if (it->HasNext(&hasNext), !hasNext) {
        *str = "{}";
        return NOERROR;
    }

    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    sb->Append(U'{');
    for (;;) {
        AutoPtr<IInterface> e;
        it->Next(&e);
        AutoPtr<IInterface> key, value;
        IMapEntry::Probe(e)->GetKey(&key);
        IMapEntry::Probe(e)->GetValue(&value);
        if (IInterface::Equals(key, (IMap*)this)) {
            sb->Append(String("(this Map)"));
        }
        else {
            sb->Append(key);
        }
        sb->Append(U'=');
        if (IInterface::Equals(value, (IMap*)this)) {
            sb->Append(String("(this Map)"));
        }
        else {
            sb->Append(value);
        }
        if (it->HasNext(&hasNext), !hasNext) {
            sb->Append(U'}');
            return sb->ToString(str);
        }
        sb->Append(U',');
        sb->Append(U' ');
    }
}

}
}
