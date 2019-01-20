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

#ifndef __LIBCORE_ICU_TIMEZONENAMES_H__
#define __LIBCORE_ICU_TIMEZONENAMES_H__

#include "ccm.util.ILocale.h"

using ccm::util::ILocale;

namespace libcore {
namespace icu {

class TimeZoneNames
{
public:
    static ECode GetDisplayName(
        /* [in] */ const Array<Array<String>>& zoneStrings,
        /* [in] */ const String& id,
        /* [in] */ Boolean daylight,
        /* [in] */ Integer style,
        /* [out] */ String* name)
    {
        return NOERROR;
    }

    static ECode GetZoneStrings(
        /* [in] */ ILocale* locale,
        /* [out, callee] */ Array<Array<String>>* zoneStrings);

private:
    TimeZoneNames();
};

}
}

#endif // __LIBCORE_ICU_TIMEZONENAMES_H__
