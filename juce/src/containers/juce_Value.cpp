/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-10 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#include "../core/juce_StandardHeader.h"

BEGIN_JUCE_NAMESPACE

#include "juce_Value.h"

//==============================================================================
Value::ValueSource::ValueSource()
{
}

Value::ValueSource::~ValueSource()
{
}

void Value::ValueSource::sendChangeMessage (const bool synchronous)
{
    if (synchronous)
    {
        for (int i = valuesWithListeners.size(); --i >= 0;)
        {
            Value* const v = valuesWithListeners[i];

            if (v != 0)
                v->callListeners();
        }
    }
    else
    {
        triggerAsyncUpdate();
    }
}

void Value::ValueSource::handleAsyncUpdate()
{
    sendChangeMessage (true);
}

//==============================================================================
class SimpleValueSource  : public Value::ValueSource
{
public:
    SimpleValueSource()
    {
    }

    SimpleValueSource (const var& initialValue)
        : value (initialValue)
    {
    }

    ~SimpleValueSource()
    {
    }

    const var getValue() const
    {
        return value;
    }

    void setValue (const var& newValue)
    {
        if (newValue != value)
        {
            value = newValue;
            sendChangeMessage (false);
        }
    }

private:
    var value;

    JUCE_DECLARE_NON_COPYABLE (SimpleValueSource);
};


//==============================================================================
Value::Value()
    : value (new SimpleValueSource())
{
}

Value::Value (ValueSource* const value_)
    : value (value_)
{
    jassert (value_ != 0);
}

Value::Value (const var& initialValue)
    : value (new SimpleValueSource (initialValue))
{
}

Value::Value (const Value& other)
    : value (other.value)
{
}

Value& Value::operator= (const Value& other)
{
    value = other.value;
    return *this;
}

Value::~Value()
{
    if (listeners.size() > 0)
        value->valuesWithListeners.removeValue (this);
}

//==============================================================================
const var Value::getValue() const
{
    return value->getValue();
}

Value::operator const var() const
{
    return getValue();
}

void Value::setValue (const var& newValue)
{
    value->setValue (newValue);
}

const String Value::toString() const
{
    return value->getValue().toString();
}

Value& Value::operator= (const var& newValue)
{
    value->setValue (newValue);
    return *this;
}

void Value::referTo (const Value& valueToReferTo)
{
    if (valueToReferTo.value != value)
    {
        if (listeners.size() > 0)
        {
            value->valuesWithListeners.removeValue (this);
            valueToReferTo.value->valuesWithListeners.add (this);
        }

        value = valueToReferTo.value;
        callListeners();
    }
}

bool Value::refersToSameSourceAs (const Value& other) const
{
    return value == other.value;
}

bool Value::operator== (const Value& other) const
{
    return value == other.value || value->getValue() == other.getValue();
}

bool Value::operator!= (const Value& other) const
{
    return value != other.value && value->getValue() != other.getValue();
}

//==============================================================================
void Value::addListener (ValueListener* const listener)
{
    if (listener != 0)
    {
        if (listeners.size() == 0)
            value->valuesWithListeners.add (this);

        listeners.add (listener);
    }
}

void Value::removeListener (ValueListener* const listener)
{
    listeners.remove (listener);

    if (listeners.size() == 0)
        value->valuesWithListeners.removeValue (this);
}

void Value::callListeners()
{
    Value v (*this); // (create a copy in case this gets deleted by a callback)
    listeners.call (&ValueListener::valueChanged, v);
}

OutputStream& JUCE_CALLTYPE operator<< (OutputStream& stream, const Value& value)
{
    return stream << value.toString();
}

END_JUCE_NAMESPACE
