/*
   Copyright 2009 Last.fm Ltd.
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of liblastfm.

   liblastfm is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   liblastfm is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with liblastfm.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LASTFM_GLOBAL_H
#define LASTFM_GLOBAL_H

#define LASTFM_VERSION 0x00010009
#define LASTFM_VERSION_STRING "1.0.9"
#define LASTFM_MAJOR_VERSION 1
#define LASTFM_MINOR_VERSION 0
#define LASTFM_PATCH_VERSION 9


#ifndef LASTFM_LIB_STATIC
    #ifdef LASTFM_LIB
        #define LASTFM_DLLEXPORT Q_DECL_EXPORT
    #else
        #define LASTFM_DLLEXPORT Q_DECL_IMPORT
    #endif
    #ifdef LASTFM_FINGERPRINT_LIB
        #define LASTFM_FINGERPRINT_DLLEXPORT Q_DECL_EXPORT
    #else
        #define LASTFM_FINGERPRINT_DLLEXPORT Q_DECL_IMPORT
    #endif
#else // LASTFM_LIB_STATIC
    #define LASTFM_DLLEXPORT
    #define LASTFM_FINGERPRINT_DLLEXPORT
#endif // LASTFM_LIB_STATIC



#include <QMetaEnum>
#include <QString>

namespace lastfm
{
    /** http://labs.trolltech.com/blogs/2008/10/09/coding-tip-pretty-printing-enum-values
      * Tips for making this take a single parameter welcome! :)
      *
      * eg. lastfm::qMetaEnumString<QNetworkReply>( error, "NetworkError" );
      */
    template <typename T> static inline QString qMetaEnumString( int enum_value, const char* enum_name )
    {
        QMetaObject meta = T::staticMetaObject;
        for (int i=0; i < meta.enumeratorCount(); ++i)
        {
            QMetaEnum m = meta.enumerator(i);
            if (m.name() == QLatin1String(enum_name))
                return QLatin1String(m.valueToKey(enum_value));
        }
        return QString("Unknown enum value for \"%1\": %2").arg( enum_name ).arg( enum_value );
    }
}


#ifdef LASTFM_COLLAPSE_NAMESPACE
using namespace ::lastfm;
#endif


#endif //LASTFM_GLOBAL_H