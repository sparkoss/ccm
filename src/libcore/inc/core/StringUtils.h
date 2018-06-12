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

#ifndef __CCM_CORE_STRINGUTILS_H__
#define __CCM_CORE_STRINGUTILS_H__

#include <ccmtypes.h>

namespace ccm {
namespace core {

class StringUtils
{
public:
    COM_PUBLIC static ECode ParseByte(
        /* [in] */ const String& s,
        /* [out] */ Byte* value);

    COM_PUBLIC static ECode ParseByte(
        /* [in] */ const String& s,
        /* [in] */ Integer radix,
        /* [out] */ Byte* value);

    COM_PUBLIC static ECode ParseShort(
        /* [in] */ const String& s,
        /* [out] */ Short* value);

    COM_PUBLIC static ECode ParseShort(
        /* [in] */ const String& s,
        /* [in] */ Integer radix,
        /* [out] */ Short* value);

    COM_PUBLIC static ECode ParseInt(
        /* [in] */ const String& s,
        /* [out] */ Integer* value);

    COM_PUBLIC static ECode ParseInt(
        /* [in] */ const String& s,
        /* [in] */ Integer radix,
        /* [out] */ Integer* value);

    COM_PUBLIC static ECode ParseLong(
        /* [in] */ const String& s,
        /* [out] */ Long* value);

    COM_PUBLIC static ECode ParseLong(
        /* [in] */ const String& s,
        /* [in] */ Integer radix,
        /* [out] */ Long* value);

    COM_PUBLIC static ECode ParseFloat(
        /* [in] */ const String& s,
        /* [out] */ Float* value);

    COM_PUBLIC static ECode ParseDouble(
        /* [in] */ const String& s,
        /* [out] */ Double* value);

    COM_PUBLIC static String ToString(
        /* [in] */ Byte b);

    COM_PUBLIC static String ToString(
        /* [in] */ Short b);

    COM_PUBLIC static String ToString(
        /* [in] */ Integer i);

    COM_PUBLIC static String ToString(
        /* [in] */ Integer i,
        /* [in] */ Integer radix);

    COM_PUBLIC static String ToString(
        /* [in] */ Long i);

    COM_PUBLIC static String ToString(
        /* [in] */ Long i,
        /* [in] */ Integer radix);

private:
    StringUtils();
    StringUtils(const StringUtils&);
    ~StringUtils();
};

inline ECode StringUtils::ParseByte(
    /* [in] */ const String& s,
    /* [out] */ Byte* value)
{
    return ParseByte(s, 10, value);
}

inline ECode StringUtils::ParseShort(
    /* [in] */ const String& s,
    /* [out] */ Short* value)
{
    return ParseShort(s, 10, value);
}

inline ECode StringUtils::ParseInt(
    /* [in] */ const String& s,
    /* [out] */ Integer* value)
{
    return ParseInt(s, 10, value);
}

inline ECode StringUtils::ParseLong(
    /* [in] */ const String& s,
    /* [out] */ Long* value)
{
    return ParseLong(s, 10, value);
}

inline String StringUtils::ToString(
    /* [in] */ Byte b)
{
    return ToString((Integer)b);
}

inline String StringUtils::ToString(
    /* [in] */ Short b)
{
    return ToString((Integer)b);
}

}
}

#endif // __CCM_CORE_STRINGUTILS_H__
