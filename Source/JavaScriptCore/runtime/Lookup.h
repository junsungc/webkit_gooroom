/*
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2003, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef Lookup_h
#define Lookup_h

#include "BatchedTransitionOptimizer.h"
#include "CallFrame.h"
#include "CustomGetterSetter.h"
#include "Identifier.h"
#include "IdentifierInlines.h"
#include "Intrinsic.h"
#include "JSGlobalObject.h"
#include "PropertySlot.h"
#include "PutPropertySlot.h"
#include <wtf/Assertions.h>

namespace JSC {

struct CompactHashIndex {
    const int16_t value;
    const int16_t next;
};

// FIXME: There is no reason this get function can't be simpler.
// ie. typedef JSValue (*GetFunction)(ExecState*, JSObject* baseObject)
typedef PropertySlot::GetValueFunc GetFunction;
typedef PutPropertySlot::PutValueFunc PutFunction;
typedef FunctionExecutable* (*BuiltinGenerator)(VM&);

// Hash table generated by the create_hash_table script.
struct HashTableValue {
    const char* m_key; // property name
    unsigned m_attributes; // JSObject attributes
    Intrinsic m_intrinsic;
    union ValueStorage {
        constexpr ValueStorage(intptr_t value1, intptr_t value2)
            : value1(value1)
            , value2(value2)
        { }
        constexpr ValueStorage(long long constant)
            : constant(constant)
        { }

        struct {
            intptr_t value1;
            intptr_t value2;
        };
        long long constant;
    } m_values;

    unsigned attributes() const { return m_attributes; }

    Intrinsic intrinsic() const { ASSERT(m_attributes & Function); return m_intrinsic; }
    BuiltinGenerator builtinGenerator() const { ASSERT(m_attributes & Builtin); return reinterpret_cast<BuiltinGenerator>(m_values.value1); }
    NativeFunction function() const { ASSERT(m_attributes & Function); return reinterpret_cast<NativeFunction>(m_values.value1); }
    unsigned char functionLength() const { ASSERT(m_attributes & Function); return static_cast<unsigned char>(m_values.value2); }

    GetFunction propertyGetter() const { ASSERT(!(m_attributes & BuiltinOrFunctionOrAccessorOrConstant)); return reinterpret_cast<GetFunction>(m_values.value1); }
    PutFunction propertyPutter() const { ASSERT(!(m_attributes & BuiltinOrFunctionOrAccessorOrConstant)); return reinterpret_cast<PutFunction>(m_values.value2); }

    NativeFunction accessorGetter() const { ASSERT(m_attributes & Accessor); return reinterpret_cast<NativeFunction>(m_values.value1); }
    NativeFunction accessorSetter() const { ASSERT(m_attributes & Accessor); return reinterpret_cast<NativeFunction>(m_values.value2); }

    long long constantInteger() const { ASSERT(m_attributes & ConstantInteger); return m_values.constant; }

    intptr_t lexerValue() const { ASSERT(!m_attributes); return m_values.value1; }
};

struct HashTable {
    int numberOfValues;
    int indexMask;
    bool hasSetterOrReadonlyProperties;

    const HashTableValue* values; // Fixed values generated by script.
    const CompactHashIndex* index;

    // Find an entry in the table, and return the entry.
    ALWAYS_INLINE const HashTableValue* entry(PropertyName propertyName) const
    {
        if (propertyName.isSymbol())
            return nullptr;

        auto uid = propertyName.uid();
        if (!uid)
            return nullptr;

        int indexEntry = IdentifierRepHash::hash(uid) & indexMask;
        int valueIndex = index[indexEntry].value;
        if (valueIndex == -1)
            return nullptr;

        while (true) {
            if (WTF::equal(uid, values[valueIndex].m_key))
                return &values[valueIndex];

            indexEntry = index[indexEntry].next;
            if (indexEntry == -1)
                return nullptr;
            valueIndex = index[indexEntry].value;
            ASSERT(valueIndex != -1);
        };
    }

    class ConstIterator {
    public:
        ConstIterator(const HashTable* table, int position)
            : m_table(table)
            , m_position(position)
        {
            skipInvalidKeys();
        }

        const HashTableValue* value()
        {
            return &m_table->values[m_position];
        }

        const char* key()
        {
            return m_table->values[m_position].m_key;
        }

        const HashTableValue* operator->()
        {
            return value();
        }

        bool operator!=(const ConstIterator& other)
        {
            ASSERT(m_table == other.m_table);
            return m_position != other.m_position;
        }

        ConstIterator& operator++()
        {
            ASSERT(m_position < m_table->numberOfValues);
            ++m_position;
            skipInvalidKeys();
            return *this;
        }

    private:
        void skipInvalidKeys()
        {
            ASSERT(m_position <= m_table->numberOfValues);
            while (m_position < m_table->numberOfValues && !m_table->values[m_position].m_key)
                ++m_position;
            ASSERT(m_position <= m_table->numberOfValues);
        }

        const HashTable* m_table;
        int m_position;
    };

    ConstIterator begin() const
    {
        return ConstIterator(this, 0);
    }
    ConstIterator end() const
    {
        return ConstIterator(this, numberOfValues);
    }
};

JS_EXPORT_PRIVATE bool setUpStaticFunctionSlot(ExecState*, const HashTableValue*, JSObject* thisObject, PropertyName, PropertySlot&);
JS_EXPORT_PRIVATE void reifyStaticAccessor(VM&, const HashTableValue&, JSObject& thisObject, PropertyName);

/**
 * This method does it all (looking in the hashtable, checking for function
 * overrides, creating the function or retrieving from cache, calling
 * getValueProperty in case of a non-function property, forwarding to parent if
 * unknown property).
 */
template <class ThisImp, class ParentImp>
inline bool getStaticPropertySlot(ExecState* exec, const HashTable& table, ThisImp* thisObj, PropertyName propertyName, PropertySlot& slot)
{
    const HashTableValue* entry = table.entry(propertyName);

    if (!entry) // not found, forward to parent
        return ParentImp::getOwnPropertySlot(thisObj, exec, propertyName, slot);

    if (entry->attributes() & BuiltinOrFunctionOrAccessor)
        return setUpStaticFunctionSlot(exec, entry, thisObj, propertyName, slot);

    if (entry->attributes() & ConstantInteger) {
        slot.setValue(thisObj, entry->attributes(), jsNumber(entry->constantInteger()));
        return true;
    }

    slot.setCacheableCustom(thisObj, entry->attributes(), entry->propertyGetter());
    return true;
}

/**
 * Simplified version of getStaticPropertySlot in case there are only functions.
 * Using this instead of getStaticPropertySlot allows 'this' to avoid implementing
 * a dummy getValueProperty.
 */
template <class ParentImp>
inline bool getStaticFunctionSlot(ExecState* exec, const HashTable& table, JSObject* thisObj, PropertyName propertyName, PropertySlot& slot)
{
    if (ParentImp::getOwnPropertySlot(thisObj, exec, propertyName, slot))
        return true;

    const HashTableValue* entry = table.entry(propertyName);
    if (!entry)
        return false;

    return setUpStaticFunctionSlot(exec, entry, thisObj, propertyName, slot);
}

/**
 * Simplified version of getStaticPropertySlot in case there are no functions, only "values".
 * Using this instead of getStaticPropertySlot removes the need for a FuncImp class.
 */
template <class ThisImp, class ParentImp>
inline bool getStaticValueSlot(ExecState* exec, const HashTable& table, ThisImp* thisObj, PropertyName propertyName, PropertySlot& slot)
{
    const HashTableValue* entry = table.entry(propertyName);

    if (!entry) // not found, forward to parent
        return ParentImp::getOwnPropertySlot(thisObj, exec, propertyName, slot);

    ASSERT(!(entry->attributes() & BuiltinOrFunctionOrAccessor));

    if (entry->attributes() & ConstantInteger) {
        slot.setValue(thisObj, entry->attributes(), jsNumber(entry->constantInteger()));
        return true;
    }

    slot.setCacheableCustom(thisObj, entry->attributes(), entry->propertyGetter());
    return true;
}

inline void putEntry(ExecState* exec, const HashTableValue* entry, JSObject* base, PropertyName propertyName, JSValue value, PutPropertySlot& slot)
{
    // If this is a function put it as an override property.
    if (entry->attributes() & BuiltinOrFunction) {
        if (JSObject* thisObject = jsDynamicCast<JSObject*>(slot.thisValue()))
            thisObject->putDirect(exec->vm(), propertyName, value);
    } else if (entry->attributes() & Accessor) {
        if (slot.isStrictMode())
            throwTypeError(exec, StrictModeReadonlyPropertyWriteError);
    } else if (!(entry->attributes() & ReadOnly)) {
        entry->propertyPutter()(exec, base, JSValue::encode(slot.thisValue()), JSValue::encode(value));
        slot.setCustomProperty(base, entry->propertyPutter());
    } else if (slot.isStrictMode())
        throwTypeError(exec, StrictModeReadonlyPropertyWriteError);
}

/**
 * This one is for "put".
 * It looks up a hash entry for the property to be set.  If an entry
 * is found it sets the value and returns true, else it returns false.
 */
inline bool lookupPut(ExecState* exec, PropertyName propertyName, JSObject* base, JSValue value, const HashTable& table, PutPropertySlot& slot)
{
    const HashTableValue* entry = table.entry(propertyName);

    if (!entry)
        return false;

    putEntry(exec, entry, base, propertyName, value, slot);
    return true;
}

template<unsigned numberOfValues>
inline void reifyStaticProperties(VM& vm, const HashTableValue (&values)[numberOfValues], JSObject& thisObj)
{
    BatchedTransitionOptimizer transitionOptimizer(vm, &thisObj);
    for (auto& value : values) {
        if (!value.m_key)
            continue;

        Identifier propertyName = Identifier::fromString(&vm, reinterpret_cast<const LChar*>(value.m_key), strlen(value.m_key));
        if (value.attributes() & Builtin) {
            thisObj.putDirectBuiltinFunction(vm, thisObj.globalObject(), propertyName, value.builtinGenerator()(vm), value.attributes());
            continue;
        }

        if (value.attributes() & Function) {
            thisObj.putDirectNativeFunction(vm, thisObj.globalObject(), propertyName, value.functionLength(),
                value.function(), value.intrinsic(), value.attributes());
            continue;
        }

        if (value.attributes() & ConstantInteger) {
            thisObj.putDirect(vm, propertyName, jsNumber(value.constantInteger()), value.attributes());
            continue;
        }

        if (value.attributes() & Accessor) {
            reifyStaticAccessor(vm, value, thisObj, propertyName);
            continue;
        }

        CustomGetterSetter* customGetterSetter = CustomGetterSetter::create(vm, value.propertyGetter(), value.propertyPutter());
        thisObj.putDirectCustomAccessor(vm, propertyName, customGetterSetter, value.attributes());
    }
}

} // namespace JSC

#endif // Lookup_h
