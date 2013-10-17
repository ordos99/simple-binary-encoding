/*
 * Copyright 2013 Real Logic Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _FIELD_H_
#define _FIELD_H_

#include <vector>

#include "otf_api/Ir.h"

namespace sbe {
namespace on_the_fly {

class Field
{
public:
    /// invalid Schema ID value
    static const uint16_t INVALID_ID = 0xFFFF;
    /// index for field name
    static const int FIELD_INDEX = -1;
    /// index for composite name
    static const int COMPOSITE_INDEX = -2;

    enum Type
    {
        COMPOSITE = 1,
        ENCODING = 2,
        ENUM = 3,
        SET = 4
    };

    union EncodingValue
    {
        int64_t int64Value_;
        uint64_t uint64Value_;
        double doubleValue_;

        EncodingValue(const int64_t value) : int64Value_(value) {};
        EncodingValue(const uint64_t value) : uint64Value_(value) {};
        EncodingValue(const double value) : doubleValue_(value) {};
    };

    Field()
    {
        reset();
    };

    virtual ~Field() {};

    Type type() const
    {
        return type_;
    };

    // composite has > 1 encodings
    int numEncodings() const
    {
        return numEncodings_;
    };

    bool isComposite() const { return (COMPOSITE == type_) ? true : false; };
    bool isEnum() const { return (ENUM == type_) ? true : false; };
    bool isSet() const {return (SET == type_) ? true : false; };

    // query on overall field properties
    uint16_t schemaId() const
    {
        return schemaId_;
    };

    const std::string &name(const int index = FIELD_INDEX) const
    {
        if (index == FIELD_INDEX)
        {
            return name_;
        }
        else if (index == COMPOSITE_INDEX)
        {
            return compositeName_;
        }
        else
        {
            return encodingNames_[index];
        }        
    };

    // query on specific composite piece and encoding
    Ir::TokenPrimitiveType primitiveType(const int index = FIELD_INDEX) const
    {
        return (index == FIELD_INDEX) ? primitiveTypes_[0] : primitiveTypes_[index];
    };

    // encoding values. index = -1 means only 1 encoding (for set, enum, encoding) and exceptions on composite
    // enums and sets can have values as well. So, could grab encoding values.
    int64_t valueInt(const int index = FIELD_INDEX) const
    {
        return (index == FIELD_INDEX) ? encodingValues_[0].int64Value_ : encodingValues_[index].int64Value_;
    };

    uint64_t valueUInt(const int index = FIELD_INDEX) const
    {
        return (index == FIELD_INDEX) ? encodingValues_[0].uint64Value_ : encodingValues_[index].uint64Value_;
    };

    double valueDouble(const int index = FIELD_INDEX) const
    {
        return (index == FIELD_INDEX) ? encodingValues_[0].doubleValue_ : encodingValues_[index].doubleValue_;
    };

    // TODO: set and enum - exception on incorrect type of field
    const std::string &validValue() const
    {
        return validValue_;
    };

    const std::vector<std::string> &choices() const
    {
        return choiceValues_;
    }

protected:
    // builder-ish pattern - set by Listener
    Field &numEncodings(const uint16_t numEncodings)
    {
        numEncodings_ = numEncodings;
        return *this;
    };

    Field &type(const Type type)
    {
        type_ = type;
        return *this;
    };

    Field &name(const int index, const std::string &name)
    {
        if (index == FIELD_INDEX)
        {
            name_ = name;
        }
        else if (index == COMPOSITE_INDEX)
        {
            compositeName_ = name;
        }
        else
        {
            encodingNames_[index] = name;
        }
        return *this;
    };

    Field &schemaId(const uint16_t id)
    {
        schemaId_ = id;
        return *this;
    };

    Field &addEncoding(const std::string &name, const Ir::TokenPrimitiveType type, const int64_t value)
    {
        encodingNames_.push_back(name);
        primitiveTypes_.push_back(type);
        encodingValues_.push_back(EncodingValue(value));
        numEncodings_++;
        return *this;
    };

    Field &addEncoding(const std::string &name, const Ir::TokenPrimitiveType type, const uint64_t value)
    {
        encodingNames_.push_back(name);
        primitiveTypes_.push_back(type);
        encodingValues_.push_back(EncodingValue(value));
        numEncodings_++;
        return *this;        
    };

    Field &addEncoding(const std::string &name, const Ir::TokenPrimitiveType type, const double value)
    {
        encodingNames_.push_back(name);
        primitiveTypes_.push_back(type);
        encodingValues_.push_back(EncodingValue(value));
        numEncodings_++;
        return *this;
    };

    Field &addValidValue(const std::string value)
    {
        validValue_ = value;
        return *this;
    };

    Field &addChoice(const std::string value)
    {
        choiceValues_.push_back(value);
        return *this;
    };

    Field &reset()
    {
        name_ = "";
        compositeName_ = "";
        schemaId_ = INVALID_ID;
        numEncodings_ = 0;
        encodingNames_.clear();
        primitiveTypes_.clear();
        encodingValues_.clear();
        choiceValues_.clear();
        validValue_ = "";
        return *this;
    };

private:
    Type type_;
    std::string name_;
    std::string compositeName_;
    std::string validValue_;
    uint16_t schemaId_;
    uint16_t numEncodings_;

    std::vector<std::string> encodingNames_;
    std::vector<Ir::TokenPrimitiveType> primitiveTypes_;
    std::vector<EncodingValue> encodingValues_;
    std::vector<std::string> choiceValues_;

    friend class Listener;
};

} // namespace on_the_fly

} // namespace sbe

#endif /* _FIELD_H_ */
