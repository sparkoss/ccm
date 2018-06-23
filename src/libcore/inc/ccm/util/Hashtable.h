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

#ifndef __CCM_UTIL_HASHTABLE_H__
#define __CCM_UTIL_HASHTABLE_H__

#include "ccm/core/SyncObject.h"
#include "ccm.core.ICloneable.h"
#include "ccm.core.IInteger.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.ICollection.h"
#include "ccm.util.IDictionary.h"
#include "ccm.util.IEnumeration.h"
#include "ccm.util.IHashtable.h"
#include "ccm.util.IMap.h"
#include "ccm.util.IMapEntry.h"
#include "ccm.util.IIterator.h"

using ccm::core::ICloneable;
using ccm::core::IInteger;
using ccm::core::SyncObject;
using ccm::io::ISerializable;

namespace ccm {
namespace util {

class Hashtable
    : public SyncObject
    , public IHashtable
    , public IDictionary
    , public IMap
    , public ICloneable
    , public ISerializable
{
private:
    class HashtableEntry
        : public LightRefBase
        , public IMapEntry
    {
    public:
        HashtableEntry(
            /* [in] */ Integer hash,
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* value,
            /* [in] */ HashtableEntry* next)
            : mHash(hash)
            , mKey(key)
            , mValue(value)
            , mNext(next)
        {}

        CCM_INTERFACE_DECL();

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode GetHashCode(
            /* [out] */ Integer* hash) override;

        ECode GetKey(
            /* [out] */ IInterface** key) override;

        ECode GetValue(
            /* [out] */ IInterface** value) override;

        ECode SetValue(
            /* [in] */ IInterface* value,
            /* [out] */ IInterface** prevValue = nullptr) override;

    public:
        Integer mHash;
        AutoPtr<IInterface> mKey;
        AutoPtr<IInterface> mValue;
        HashtableEntry* mNext;
    };

    class Enumerator
        : public LightRefBase
        , public IEnumeration
        , public IIterator
    {
    public:
        Enumerator(
            /* [in] */ Integer type,
            /* [in] */ Boolean iterator)
            : mType(type)
            , mIterator(iterator)
        {}

        CCM_INTERFACE_DECL();

        ECode HasMoreElements(
            /* [out] */ Boolean* result) override;

        ECode GetNextElement(
            /* [out] */ IInterface** object) override;

        ECode GetNext(
            /* [out] */ IInterface** object) override;

        ECode HasNext(
            /* [out] */ Boolean* result) override;

        ECode Remove() override;

    public:
        Integer mType;

        Boolean mIterator;
    };

public:
    ECode GetSize(
        /* [out] */ Integer* size) override;

    ECode IsEmpty(
        /* [out] */ Boolean* result) override;

    ECode GetKeys(
        /* [out] */ IEnumeration** keys) override;

    ECode GetElements(
        /* [out] */ IEnumeration** elements) override;

    ECode Contains(
        /* [in] */ IInterface* value,
        /* [out] */ Boolean* result) override;

    ECode ContainsValue(
        /* [in] */ IInterface* value,
        /* [out] */ Boolean* result) override;

    ECode ContainsKey(
        /* [in] */ IInterface* key,
        /* [out] */ Boolean* result) override;

    ECode Get(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** value) override;

    ECode Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode Remove(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode PutAll(
        /* [in] */ IMap* m) override;

    ECode Clear() override;

    ECode Clone(
        /* [out] */ IInterface** obj) override;

    ECode ToString(
        /* [out] */ String* str) override;

    ECode GetKeySet(
        /* [out] */ ISet** keys) override;

    ECode GetEntrySet(
        /* [out] */ ISet** entries) override;

protected:
    void Rehash();

private:
    void AddEntry(
        /* [in] */ Integer hash,
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [in] */ Integer index);

private:
    /**
     * The hash table data.
     */
    Array<HashtableEntry*> mTable;

    /**
     * The total number of entries in the hash table.
     */
    Integer mCount = 0;

    /**
     * The table is rehashed when its size exceeds this threshold.  (The
     * value of this field is (int)(capacity * loadFactor).)
     */
    Integer mThreshold = 0;

    /**
     * The load factor for the hashtable.
     */
    Float mLoadFactor = 0;

    /**
     * The number of times this Hashtable has been structurally modified
     * Structural modifications are those that change the number of entries in
     * the Hashtable or otherwise modify its internal structure (e.g.,
     * rehash).  This field is used to make iterators on Collection-views of
     * the Hashtable fail-fast.  (See ConcurrentModificationException).
     */
    Integer mModCount = 0;

    static constexpr Integer MAX_ARRAY_SIZE = IInteger::MAX_VALUE - 8;

    AutoPtr<ISet> mKeySet;
    AutoPtr<ISet> mEntrySet;
    AutoPtr<ICollection> mValues;

    static constexpr Integer KEYS = 0;
    static constexpr Integer VALUES = 1;
};

}
}

#endif // __CCM_UTIL_HASHTABLE_H__
