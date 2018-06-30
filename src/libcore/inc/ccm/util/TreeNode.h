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

#ifndef __CCM_UTIL_TREENODE_H__
#define __CCM_UTIL_TREENODE_H__

#include "ccm/util/LinkedHashMap.h"

namespace ccm {
namespace util {

extern const InterfaceID IID_ITreeNode;

INTERFACE_ID(2b2b51d9-c2c4-4312-8a06-67de1c1f754b)
interface ITreeNode : public IInterface
{
    inline static ITreeNode* Probe(
        /* [in] */ IInterface* object)
    {
        if (object == nullptr) return nullptr;
        return (ITreeNode*)object->Probe(IID_ITreeNode);
    }
};

class TreeNode
    : public LinkedHashMap::LinkedHashMapEntry
    , public ITreeNode
{
public:
    TreeNode(
        /* [in] */ Integer hash,
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* val,
        /* [in] */ Node* next);

    CCM_INTERFACE_DECL();

    AutoPtr<TreeNode> Root();

    static void MoveRootToFront(
        /* [in] */ Array<Node*>& tab,
        /* [in] */ TreeNode* root);

    AutoPtr<TreeNode> GetTreeNode(
        /* [in] */ Integer h,
        /* [in] */ IInterface* k);

    void Treeify(
        /* [in] */ Array<Node*>& tab);

    AutoPtr<TreeNode> PutTreeVal(
        /* [in] */ HashMap* map,
        /* [in] */ Array<Node*>& tab,
        /* [in] */ Integer h,
        /* [in] */ IInterface* k,
        /* [in] */ IInterface* v);

    void RemoveTreeNode(
        /* [in] */ HashMap* map,
        /* [in] */ Array<Node*>& tab,
        /* [in] */ Boolean movable);

    void Split(
        /* [in] */ HashMap* map,
        /* [in] */ Array<Node*>& tab,
        /* [in] */ Integer index,
        /* [in] */ Integer bit);

    static Boolean IsTreeNode(
        /* [in] */ HashMap::Node* node);

public:
    TreeNode* mParent = nullptr;
    TreeNode* mPrev = nullptr;
};

inline Boolean TreeNode::IsTreeNode(
    /* [in] */ HashMap::Node* node)
{
    return ITreeNode::Probe((IMapEntry*)node) != nullptr;
}

}
}


#endif // __CCM_UTIL_TREENODE_H__
