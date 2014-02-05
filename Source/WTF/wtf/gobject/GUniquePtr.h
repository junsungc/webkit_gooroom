/*
 *  Copyright (C) 2014 Igalia S.L.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#ifndef GUniquePtr_h
#define GUniquePtr_h

#if USE(GLIB)

#include <gio/gio.h>

namespace WTF {

template<typename T>
struct GPtrDeleter {
    void operator()(T* ptr) const { g_free(ptr); }
};

template<typename T>
using GUniquePtr = std::unique_ptr<T, GPtrDeleter<T>>;

#define FOR_EACH_GLIB_DELETER(macro) \
    macro(GError, g_error_free) \
    macro(GList, g_list_free) \
    macro(GSList, g_slist_free) \
    macro(GPatternSpec, g_pattern_spec_free) \
    macro(GDir, g_dir_close) \
    macro(GTimer, g_timer_destroy) \
    macro(GKeyFile, g_key_file_free)

#define WTF_DEFINE_GPTR_DELETER(typeName, deleterFunc) \
    template<> struct GPtrDeleter<typeName> \
    { \
        void operator() (typeName* ptr) const \
        { \
            if (ptr) \
                deleterFunc(ptr); \
        } \
    };

FOR_EACH_GLIB_DELETER(WTF_DEFINE_GPTR_DELETER)
#undef FOR_EACH_GLIB_DELETER

} // namespace WTF

using WTF::GUniquePtr;

#endif // USE(GLIB)

#endif // GUniquePtr_h
