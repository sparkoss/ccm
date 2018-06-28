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

#include "ccm/core/ClassLoader.h"
#include "ccm/core/System.h"

namespace ccm {
namespace core {

AutoPtr<IClassLoader> ClassLoader::SystemClassLoader::sLoader = CreateSystemClassLoader();

CCM_INTERFACE_IMPL_1(ClassLoader, SyncObject, IClassLoader);

ECode ClassLoader::LoadCoclass(
    /* [in] */ const String& fullName,
    /* [out] */ IMetaCoclass** klass)
{

}

AutoPtr<IClassLoader> ClassLoader::CreateSystemClassLoader()
{
    String classPath, librarySearchPath;
    System::GetProperty(String("ccm.class.path"), String("."), &classPath);
    System::GetProperty(String("ccm.library.path"), String(""), &librarySearchPath);
}

}
}
