/**
 * @file
 *
 * Adapter implementation for the var class in the Juce library.
 *
 * Include this file in your program to enable support for Juce var.
 *
 * This file defines the following classes (not in this order):
 *  - JuceVarAdapter
 *  - JuceVarArray
 *  - JuceVarArrayValueIterator
 *  - JuceVarFrozenValue
 *  - JuceVarObject
 *  - JuceVarObjectMember
 *  - JuceVarObjectMemberIterator
 *  - JuceVarValue
 *
 */

#ifndef JUCE_VALIJSON_VAR_ADAPTER_HPP
#define JUCE_VALIJSON_VAR_ADAPTER_HPP

#include <string>
#include "JuceHeader.h"

#include <valijson/adapters/adapter.hpp>
#include <valijson/adapters/basic_adapter.hpp>
#include <valijson/adapters/frozen_value.hpp>

namespace valijson {
namespace adapters {

class JuceVarAdapter;
class JuceVarArrayValueIterator;
class JuceVarObjectMemberIterator;

typedef std::pair<std::string, JuceVarAdapter> JuceVarObjectMember;

/**
 * Light weight wrapper for a Juce var array value.
 *
 * This class is light weight wrapper for a Juce var array. It provides a
 * minimum set of container functions and typedefs that allow it to be used as
 * an iterable container.
 *
 * An instance of this class contains a single reference to the underlying
 * Juce var, assumed to be an array, so there is very little overhead
 * associated with copy construction and passing by value.
 */
class JuceVarArray
{
public:

    typedef JuceVarArrayValueIterator const_iterator;
    typedef JuceVarArrayValueIterator iterator;

    /// Construct a Json11Array referencing an empty array.
    JuceVarArray()
    :   value (emptyArray())
    {
    }

    /**
     * Construct a JuceVarArray referencing a specific Juce var value.
     *
     * @param   valueToUse   reference to a Juce var
     *
     * @note    This constructor will throw an exception if the value is not
     * an array.
     */
    JuceVarArray (const var& valueToUse)
    :   value (valueToUse)
    {
        if (! value.isArray())
            throw std::runtime_error ("Value is not an array.");
    }

    /**
     * Return an iterator for the first element of the array.
     *
     * The iterator return by this function is effectively the iterator
     * returned by the underlying Juce implementation.
     */
    JuceVarArrayValueIterator begin() const;

    /**
     * Return an iterator for one-past the last element of the array.
     *
     * The iterator return by this function is effectively the iterator
     * returned by the underlying Juce implementation.
     */
    JuceVarArrayValueIterator end() const;

    /// Return the number of elements in the array
    size_t size() const
    {
        return (size_t) value.size();
    }

private:

    /**
     * Return a reference to a Juce var that is an empty array.
     *
     * Note that the value returned by this function is a singleton.
     */
    static const var& emptyArray()
    {
        static const var array ((Array<var>()));
        return array;
    }

    /// Reference to the contained value
    var value;
};

/**
 * Light weight wrapper for a Juce var object.
 *
 * This class is light weight wrapper for a Juce var object. It provides a
 * minimum set of container functions and typedefs that allow it to be used as
 * an iterable container.
 *
 * An instance of this class contains a single reference to the underlying
 * Juce var value, assumed to be an object, so there is very little overhead
 * associated with copy construction and passing by value.
 */
class JuceVarObject
{
public:

    typedef JuceVarObjectMemberIterator const_iterator;
    typedef JuceVarObjectMemberIterator iterator;

    /// Construct a JuceVarObject referencing an empty object singleton.
    JuceVarObject()
    :   value (emptyObject())
    {
    }

    /**
     * Construct a Json11Object referencing a specific Juce var value.
     *
     * @param   valueToUse  reference to a Juce var value
     *
     * @note    This constructor will throw an exception if the value is not
     * an object.
     */
    JuceVarObject (const var& valueToUse)
    :   value (valueToUse)
    {
        if (! value.isObject())
            throw std::runtime_error ("Value is not an object.");
    }

    /**
     * Return an iterator for this first object member
     *
     * The iterator return by this function is effectively a wrapper around
     * the iterator value returned by the underlying Juce var implementation.
     */
    JuceVarObjectMemberIterator begin() const;

    /**
     * Return an iterator for an invalid object member that indicates the end
     * of the collection.
     *
     * The iterator return by this function is effectively a wrapper around
     * the iterator value returned by the underlying Juce var implementation.
     */
    JuceVarObjectMemberIterator end() const;

    /**
     * Return an iterator for the object member with the specified property
     * name.
     *
     * If an object member with the specified name does not exist, the iterator
     * returned will be the same as the iterator returned by the end() function.
     *
     * @param   propertyName  property name to search for
     */
    JuceVarObjectMemberIterator find (const std::string& propertyName) const;

    /// Returns the number of members belonging to this object.
    size_t size() const
    {
        if (DynamicObject* const object = dynamic_cast<DynamicObject*> (value.getObject()))
            return (size_t) object->getProperties().size();
        
        return 0;
    }

private:

    /**
     * @brief   Return a reference to a Juce var value that is empty object.
     *
     * Note that the value returned by this function is a singleton.
     */
    static const var& emptyObject()
    {
        static const var object (new DynamicObject());
        return object;
    }

    /// Reference to the contained object
    var value;
};

/**
 * Stores an independent copy of a Juce var value.
 *
 * This class allows a Juce var value to be stored independent of its original
 * document.
 *
 * @see FrozenValue
 */
class JuceVarFrozenValue: public FrozenValue
{
public:

    /**
     * Make a copy of a Juce var value
     *
     * @param  source  the Juce var value to be copied
     */
    JuceVarFrozenValue (const var& source)
    :   value (source.clone())
    {
    }

    FrozenValue* clone() const override
    {
        return new JuceVarFrozenValue (value);
    }

    bool equalTo (const Adapter& other, bool strict) const override;

private:

    /// Stored Juce var value
    var value;
};

/**
 * Light weight wrapper for a Juce var value.
 *
 * This class is passed as an argument to the BasicAdapter template class,
 * and is used to provide access to a Juce var value. This class is responsible
 * for the mechanics of actually reading a Juce var value, whereas the
 * BasicAdapter class is responsible for the semantics of type comparisons
 * and conversions.
 *
 * The functions that need to be provided by this class are defined implicitly
 * by the implementation of the BasicAdapter template class.
 *
 * @see BasicAdapter
 */
class JuceVarValue
{
public:

    /// Construct a wrapper for the empty object singleton
    JuceVarValue()
    :   value (emptyObject())
    {
    }

    /// Construct a wrapper for a specific Json11 value
    JuceVarValue (const var& valueToUse)
    :   value (valueToUse)
    {
    }

    /**
     * Create a new JuceVarFrozenValue instance that contains the value
     * referenced by this JuceVarValue instance.
     *
     * @returns pointer to a new Json11FrozenValue instance, belonging to the
     *          caller.
     */
    FrozenValue* freeze() const
    {
        return new JuceVarFrozenValue (value);
    }

    /**
     * Optionally return a JuceVarArray instance.
     *
     * If the referenced Juce var value is an array, this function will return
     * a std::optional containing a JuceVarArray instance referencing the
     * array.
     *
     * Otherwise it will return an empty optional.
     */
    opt::optional<JuceVarArray> getArrayOptional() const
    {
        if (value.isArray())
            return opt::make_optional (JuceVarArray (value));

        return opt::optional<JuceVarArray>();
    }

    /**
     * Retrieve the number of elements in the array
     *
     * If the referenced Json11 value is an array, this function will
     * retrieve the number of elements in the array and store it in the output
     * variable provided.
     *
     * @param   result  reference to size_t to set with result
     *
     * @returns true if the number of elements was retrieved, false otherwise.
     */
    bool getArraySize (size_t& result) const
    {
        if (value.isArray())
        {
            result = (size_t) value.size();
            return true;
        }

        return false;
    }

    bool getBool (bool& result) const
    {
        if (value.isBool())
        {
            result = value;
            return true;
        }

        return false;
    }

    bool getDouble (double& result) const
    {
        if (value.isDouble())
        {
            result = value;
            return true;
        }

        return false;
    }

    bool getInteger (int64_t& result) const
    {
        if (isInteger())
        {
            result = value;
            return true;
        }
        
        return false;
    }

    /**
     * Optionally return a JuceVarObject instance.
     *
     * If the referenced Juce var value is an object, this function will return a
     * std::optional containing a JuceVarObject instance referencing the
     * object.
     *
     * Otherwise it will return an empty optional.
     */
    opt::optional<JuceVarObject> getObjectOptional() const
    {
        if (value.isObject())
            return opt::make_optional (JuceVarObject (value));

        return opt::optional<JuceVarObject>();
    }

    /**
     * Retrieve the number of members in the object
     *
     * If the referenced Juce var value is an object, this function will
     * retrieve the number of members in the object and store it in the output
     * variable provided.
     *
     * @param   result  reference to size_t to set with result
     *
     * @returns true if the number of members was retrieved, false otherwise.
     */
    bool getObjectSize (size_t& result) const
    {
        if (DynamicObject* const object = dynamic_cast<DynamicObject*> (value.getObject()))
        {
            result = (size_t) object->getProperties().size();
            return true;
        }

        return false;
    }

    bool getString (std::string& result) const
    {
        if (value.isString())
        {
            result = value.toString().toStdString();
            return true;
        }

        return false;
    }

    static bool hasStrictTypes()
    {
        return true;
    }

    bool isArray() const
    {
        return value.isArray();
    }

    bool isBool() const
    {
        return value.isBool();
    }

    bool isDouble() const
    {
        return value.isDouble();
    }

    bool isInteger() const
    {
        return value.isInt() || value.isInt64();
    }

    bool isNull() const
    {
        return value.isVoid();
    }

    bool isNumber() const
    {
        return value.isDouble() || isInteger();
    }

    bool isObject() const
    {
        return value.isObject();
    }

    bool isString() const
    {
        return value.isString();
    }

private:

    /// Return a reference to an empty object singleton
    static const var& emptyObject()
    {
        static const var object (new DynamicObject());
        return object;
    }

    /// Reference to the contained Juce var value.
    var value;
};

/**
 * An implementation of the Adapter interface supporting Juce var.
 *
 * This class is defined in terms of the BasicAdapter template class, which
 * helps to ensure that all of the Adapter implementations behave consistently.
 *
 * @see Adapter
 * @see BasicAdapter
 */
class JuceVarAdapter : public BasicAdapter<JuceVarAdapter,
                                           JuceVarArray,
                                           JuceVarObjectMember,
                                           JuceVarObject,
                                           JuceVarValue>
{
public:

    /// Construct a JuceVarAdapter that contains an empty object
    JuceVarAdapter()
    :   BasicAdapter()
    {
    }

    /// Construct a JuceVarAdapter containing a specific Juce var value
    JuceVarAdapter (const var& value)
    :   BasicAdapter (value)
    {
    }
};

/**
 * Class for iterating over values held in a JSON array.
 *
 * This class provides a JSON array iterator that dereferences as an instance of
 * JuceVarAdapter representing a value stored in the array.
 *
 * @see JuceVarArray
 */
class JuceVarArrayValueIterator
      : public std::iterator<std::bidirectional_iterator_tag,   // bi-directional iterator
                             JuceVarAdapter>                    // value type
{
public:
    
    /**
     * Construct a new JuceVarArrayValueIterator using an existing juce var
     * iterator.
     *
     * @param   itrToUse  Juce var array iterator to store
     */
    JuceVarArrayValueIterator (var* itrToUse)
    :   itr (itrToUse)
    {
    }

    /// Returns a Json11Adapter that contains the value of the current
    /// element.
    JuceVarAdapter operator*() const
    {
        return JuceVarAdapter (*itr);
    }

    DerefProxy<JuceVarAdapter> operator->() const
    {
        return DerefProxy<JuceVarAdapter>(**this);
    }

    /**
     * Compare this iterator against another iterator.
     *
     * @note  This directly compares the iterators, not the underlying values
     *        and assumes that two identical iterators will point to the same
     *        underlying object.
     *
     * @param   other  iterator to compare against
     *
     * @returns true   if the iterators are equal, false otherwise.
     */
    bool operator== (const JuceVarArrayValueIterator& other) const
    {
        return itr == other.itr;
    }

    bool operator!= (const JuceVarArrayValueIterator& other) const
    {
        return ! (itr == other.itr);
    }

    const JuceVarArrayValueIterator& operator++()
    {
        itr++;
        return *this;
    }

    JuceVarArrayValueIterator operator++ (int)
    {
        JuceVarArrayValueIterator iterator_pre (itr);
        ++(*this);
        
        return iterator_pre;
    }

    const JuceVarArrayValueIterator& operator--()
    {
        itr--;
        return *this;
    }

    void advance (std::ptrdiff_t n)
    {
        itr += n;
    }

private:
    var* itr;
};

/**
 * Class for iterating over the members belonging to a JSON object.
 *
 * This class provides a JSON object iterator that dereferences as an instance
 * of JuceVarObjectMember representing one of the members of the object.
 *
 * @see JuceVarObject
 * @see JuceVarObjectMember
 */
class JuceVarObjectMemberIterator : public std::iterator<std::bidirectional_iterator_tag,   // bi-directional iterator
                                                        JuceVarObjectMember>                // value type
{
public:

    /**
     * Construct an iterator from a Jcue var iterator.
     *
     * @param   itrToUse  Juce var iterator to store
     */
    JuceVarObjectMemberIterator (NamedValueSet::NamedValue* itrToUse)
    :   itr (itrToUse)
    {
    }

    /**
     * Returns a JuceVarObjectMember that contains the key and value
     *          belonging to the object member identified by the iterator.
     */
    JuceVarObjectMember operator*() const
    {
        if (itr == nullptr)
            return JuceVarObjectMember ("", JuceVarAdapter());
        
        return JuceVarObjectMember (itr->name.toString().toStdString(), itr->value);
    }

    DerefProxy<JuceVarObjectMember> operator->() const
    {
        return DerefProxy<JuceVarObjectMember>(**this);
    }

    /**
     * Compare this iterator with another iterator.
     *
     * Note that this directly compares the iterators, not the underlying
     * values, and assumes that two identical iterators will point to the same
     * underlying object.
     *
     * @param   other  Iterator to compare with
     *
     * @returns true if the underlying iterators are equal, false otherwise
     */
    bool operator== (const JuceVarObjectMemberIterator& other) const
    {
        return itr == other.itr;
    }

    bool operator!=(const JuceVarObjectMemberIterator& other) const
    {
        return ! (itr == other.itr);
    }

    const JuceVarObjectMemberIterator& operator++()
    {
        itr++;
        return *this;
    }

    JuceVarObjectMemberIterator operator++ (int)
    {
        JuceVarObjectMemberIterator iterator_pre (itr);
        ++(*this);
        return iterator_pre;
    }

    const JuceVarObjectMemberIterator& operator--()
    {
        itr--;
        return *this;
    }

private:

    /// Iternal copy of the original Juce var iterator
    NamedValueSet::NamedValue* itr;
};

/// Specialisation of the AdapterTraits template struct for JuceVarAdapter.
template<>
struct AdapterTraits<valijson::adapters::JuceVarAdapter>
{
    typedef var DocumentType;

    static std::string adapterName()
    {
        return "JuceVarAdapter";
    }
};

inline bool JuceVarFrozenValue::equalTo (const Adapter& other, bool strict) const
{
    return JuceVarAdapter (value).equalTo (other, strict);
}

inline JuceVarArrayValueIterator JuceVarArray::begin() const
{
    if (Array<var>* const array = value.getArray())
        return array->begin();
    
    return nullptr;
}

inline JuceVarArrayValueIterator JuceVarArray::end() const
{
    if (Array<var>* const array = value.getArray())
        return array->end();
    
    return nullptr;
}

inline JuceVarObjectMemberIterator JuceVarObject::begin() const
{
    if (DynamicObject* const object = dynamic_cast<DynamicObject*> (value.getObject()))
        return object->getProperties().begin();
    
    return nullptr;
}

inline JuceVarObjectMemberIterator JuceVarObject::end() const
{
    if (DynamicObject* const object = dynamic_cast<DynamicObject*> (value.getObject()))
        return object->getProperties().end();
    
    return nullptr;
}

inline JuceVarObjectMemberIterator JuceVarObject::find (const std::string& propertyName) const
{
    if (DynamicObject* const object = dynamic_cast<DynamicObject*> (value.getObject()))
    {
        const Identifier propertyId ((String (propertyName)));
        
        NamedValueSet::NamedValue* iter = object->getProperties().begin();
        
        for (; iter != object->getProperties().end(); ++iter)
            if (iter->name == propertyId)
                return iter;
        
        return iter;
    }
    
    return nullptr;
}

}  // namespace adapters
}  // namespace valijson

#endif // JUCE_VALIJSON_VAR_ADAPTER_HPP
