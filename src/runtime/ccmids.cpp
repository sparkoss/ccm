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

#include "ccmtypes.h"

namespace ccm {

const CoclassID CoclassID::Null =
    {{0x00000000,0x0000,0x0000,0x0000,{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}}, nullptr};

const InterfaceID InterfaceID::Null =
    {{0x00000000,0x0000,0x0000,0x0000,{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}}, nullptr};

COM_PUBLIC extern const ComponentID CID_CCMRuntime =
    {{0xffffffff,0xffff,0xffff,0xffff,{0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf}},
    "http://ccm.org/ccmrt.so"};

COM_PUBLIC extern const InterfaceID IID_IInterface =
    {{0x00000000,0x0000,0x0000,0x0000,{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1}}, &CID_CCMRuntime};

}
