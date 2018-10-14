//
// By pkgulati@gmail.com
//

#include "json_pack.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

JsonPack::JsonPack(char* ptr, int len){
    _start = ptr;
    _pos = _start;
    _end = ptr+len;
    _key = NULL;
    _key_length=0;
    _value_type = JSON_NO_VALUE;
    _read_next_token();
}

JsonTokenType
JsonPack::TokenType() {
    return _tokenType;
}

JsonValueType
JsonPack::ValueType() {
    return _value_type;
}

char*
JsonPack::Key() {
    return _key;
}

int
JsonPack::KeyLength() {
    return _key_length;
}

double
JsonPack::Number() {
    return _number;
}
long
JsonPack::Quantity() {
    return _qty;
}

bool
JsonPack::Flag() {
    return _flag;
}

char*
JsonPack::Value() {
    return _str;
}

int
JsonPack::ValueLength() {
    return _strlen;
}

void
JsonPack::_ReadNullToken()
{
    if (_end - _pos < 4)
    {
        _tokenType = JSON_INVALID_TOKEN;
    }
    else if (strncmp(_pos, "null", 4) == 0)
    {
        _tokenType = JSON_NULL_TOKEN;
        _value_type = JSON_NULL;
        _str =  _pos;
        _strlen =  4;
        _pos += 4;
    }
    else
    {
        _tokenType = JSON_INVALID_TOKEN;
    }
}

void
JsonPack::_ReadTrueToken()
{
    if (_end - _pos < 4)
    {
        _tokenType = JSON_INVALID_TOKEN;
    }
    else if (strncmp(_pos, "true", 4) == 0)
    {
        _tokenType = JSON_TRUE_TOKEN;
        _value_type = JSON_BOOLEAN;
        _str =  _pos;
        _strlen =  4;
        _flag = true;
        _pos += 4;
    }
    else
    {
        _tokenType = JSON_INVALID_TOKEN;
    }
}

void
JsonPack::_ReadFalseToken()
{
    if (_end - _pos < 5)
    {
        _tokenType = JSON_INVALID_TOKEN;
    }
    else if (strncmp(_pos, "false", 5) == 0)
    {
        _tokenType = JSON_FALSE_TOKEN;
        _value_type = JSON_BOOLEAN;
        _str =  _pos;
        _strlen =  5;
        _flag = false;
        _pos += 5;
    }
    else
    {
        _tokenType = JSON_INVALID_TOKEN;
    }
    
}

void
JsonPack::_ReadStringToken()
{
    ++_pos;
    char* begin = _pos;
    while(_pos != _end && *_pos != '"')
    {
        if (*_pos == '\\')
            ++_pos;
        ++_pos;
    }
    
    _tokenType = JSON_STRING_TOKEN;
    _value_type = JSON_STRING;
    _str =  begin;
    _strlen =  _pos - begin;
    _pos++;
}


int JsonPack::_read_next_token() {
    if (_pos == _end) {
        return false;
    }
    
    switch(*_pos)
    {
            // white space
        case ' ':
        case '\n':
        case '\r':
        case '\t':
            ++_pos;
            _read_next_token();
            break;
        case '\0':
            _tokenType = JSON_INVALID_TOKEN;
            break;
        case '{' :
            _tokenType = JSON_BEGIN_TOKEN;
            ++_pos;
            break;
        case '}':
            _tokenType = JSON_END_TOKEN;
            ++_pos;
            break;
        case '[':
            _tokenType = JSON_ARRAY_BEGIN;
            _value_type = JSON_ARRAY;
            _pos++;
            break;
        case ']':
            _tokenType = JSON_ARRAY_END;
            ++_pos;
            break;
        case ':':
            _tokenType = JSON_COLON_TOKEN;
            ++_pos;
            break;
        case ',':
            _tokenType = JSON_ARRAY_SEPARATOR;
            ++_pos;
            break;
        case 'n':
            _ReadNullToken();
            break;
        case 't':
            _ReadTrueToken();
            break;
        case 'f':
            _ReadFalseToken();
            break;
            
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            _ReadNumberToken();
            break;
            
        case '"':
            _ReadStringToken();
            break;
        default:
            _tokenType = JSON_INVALID_TOKEN;
    }
    //std::cout << "next token _tokenType " << _tokenType << std::endl;
    return true;
};

void
JsonPack::_ReadNumberToken()
{
    char* begin = _pos;
    double num1 = 0, num2=0, multiplier = 1;
    int exp = 0;
    int decimal = 0;
    int negative = 1;
    _qty = 0;
    _number = 0;
    if ('-' == *_pos) {
        ++_pos;
        negative = -1;
    }
    
    bool numberEnd = false;
    while(_pos != _end)
    {
        switch (*_pos)
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                num1 = num1 * 10.0 + (float)((*_pos)-'0');
                if (!exp && decimal) {
                    multiplier /= 10.0;
                }
                 ++_pos;
                break;
            case '-':
                multiplier = -1;
                ++_pos;
                break;
            case '.':    // decimal separator can not be , in case of array of decimal values assume . always for now
                decimal = 1;
                 ++_pos;
                break;
            case 'e':
            case 'E':
                ++_pos;
                num2 = num1;
                num1 = 0;
                exp = 1;
                break;
            default:
                numberEnd = true;
                break;
        }
        
        if (numberEnd)
        {
            break;
        }
    }
    
    _tokenType = JSON_NUMBER_TOKEN;
    _str =  begin;
    _strlen =  _pos - begin;
    if (!decimal && !exp) {
        _value_type = JSON_INTEGER;
        _number = negative * num1 * multiplier;
        _qty = negative * num1;
    }
    else if (exp) {
        _value_type = JSON_DECIMAL;
        _number = negative * num2 * multiplier * pow(10, num1);
    } else {
        _value_type = JSON_DECIMAL;
        _number = negative * num1 * multiplier;
    }
}

int
JsonPack::ReadValue() {
    if (_tokenType == JSON_ARRAY_END) {
        _read_next_token();
        return 0;
    }
   
    if (_tokenType != JSON_COLON_TOKEN &&
        _tokenType != JSON_ARRAY_BEGIN &&
        _tokenType != JSON_ARRAY_SEPARATOR) {
        std::cout << "not at JSON_COLON_TOKEN,JSON_ARRAY_BEGIN,JSON_ARRAY_SEPARATOR " << _tokenType << std::endl;
        _value_type = JSON_NO_VALUE;
        return 0;
    }
    _read_next_token();
    switch(_tokenType) {
        case JSON_ARRAY_BEGIN:
            _value_type = JSON_ARRAY;
            break;
        case JSON_ARRAY_END:
            _value_type = JSON_NO_VALUE;
            _read_next_token();
            return 0;
            break;
        case JSON_BEGIN_TOKEN:
            _value_type = JSON_OBJECT;
            break;
        case JSON_END_TOKEN:
            _value_type = JSON_NO_VALUE;
            _read_next_token();
            break;
        case JSON_STRING_TOKEN:
            _value_type = JSON_STRING;
            _read_next_token();
            break;
        case JSON_NULL_TOKEN:
            _value_type = JSON_NULL;
            _read_next_token();
            break;
        case JSON_TRUE_TOKEN:
            _value_type = JSON_BOOLEAN;
            _read_next_token();
             break;
        case JSON_FALSE_TOKEN:
            _value_type = JSON_BOOLEAN;
            _read_next_token();
            break;
        case JSON_NUMBER_TOKEN:
            _read_next_token();
            break;
        default :
            _value_type = JSON_NO_VALUE;
    }
    return 1;
}

int
JsonPack::ReadMember() {
    if (_tokenType == JSON_END_TOKEN) {
        _read_next_token();
        return 0;
    }
    if (_tokenType != JSON_BEGIN_TOKEN &&
        _tokenType != JSON_ARRAY_SEPARATOR) {
        std::cout << "not1 at JSON_BEGIN_TOKEN, JSON_ARRAY_SEPARATOR " << _tokenType << std::endl;
        return 0;
    }
    _read_next_token();
    if (_tokenType != JSON_STRING_TOKEN) {
        return 0;
    }
    _key = _str;
    _key_length = _strlen;
    _read_next_token();
    if (_tokenType != JSON_COLON_TOKEN) {
        std::cout << "Expected colon " << std::endl;
        return 0;
    }
    return ReadValue();
}


int
JsonPack::ReadArray() {
    int n = 0;
    while(ReadValue()) {
        std::cout << "Array [" << n++ << "] ";
        switch(ValueType()) {
            case JSON_ARRAY:
                ReadArray();
                break;
            case JSON_OBJECT:
                ReadObject();
                break;
            case JSON_STRING:
                std::cout << "value ";
                std::cout.write(_str, _strlen);
                std::cout.write(" ",1);
                break;
        }
    }
    std::cout.write("\n",1);
    return 1;
}

void JsonPack::PrintValue() {
    switch(ValueType()) {
        case JSON_STRING:
        {
            std::cout.write(Value(), ValueLength());
            std::cout.write("\n",1);
            break;
        }
        case JSON_DECIMAL:
        {
            std::cout.write(Value(), ValueLength());
            std::cout.write(" ",1);
            // actually we will calculate number when reading itself
            std::cout << "_number " << std::fixed  << _number;
            std::cout.write("\n",1);
            break;
        }
        case JSON_INTEGER:
        {
            std::cout.write(Value(), ValueLength());
            std::cout.write(" ",1);
            // actually we will calculate number when reading itself
            std::cout << "_number " << _qty;
            std::cout.write("\n",1);
            break;
        }
        case JSON_BOOLEAN:
        {
            std::cout << _flag;
            std::cout.write(" ",1);
            std::cout.write(Value(), ValueLength());
            std::cout.write(" ",1);
            if (_flag) {
                 std::cout << "true";
            } else {
                std::cout << "false";
            }
            std::cout.write("\n",1);
            break;
        }
        case JSON_NULL:
        {
            std::cout << "null";
            std::cout.write("\n",1);
            break;
        }
    }
}

int
JsonPack::ReadObject() {
    while(ReadMember()) {
        std::cout.write(Key(), KeyLength());
        std::cout.write(":",1);
        switch(ValueType()) {
            case JSON_ARRAY:
                std::cout << "array ";
                ReadArray();
                break;
            case JSON_OBJECT:
                std::cout << "sub object" << std::endl;
                ReadObject();
                break;
            default:
                PrintValue();
                break;
        }
    }
    return 1;
}


int 
JsonPack::toFieldId(char* str) {
    int hash = 0, i=0, chr=0;
    if (strlen(str) == 0) return hash;
    char *ptr = str;
    while(*ptr) {
        int a = *ptr;
        hash = (hash << 5) - hash + a;
        hash |= 0; // Convert to 32bit integer
        ptr++;
    }
    return hash & 0x7FFFFFFF;
};

enum fenum {
    FLD_TEST1 = 110251487,
    FLD_TEST2 = 110251488
};
