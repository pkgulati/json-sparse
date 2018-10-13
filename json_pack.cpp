//
//  pkgulati@gmail.com on 2018-10-13.
//  
#include "json_pack.hpp"
#include <iostream>
#include <stdio.h>

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
    if ('-' == *_pos) {
        ++_pos;
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
            case '-':
            case '.':    // decimal separator can not be , in case of array of decimal values assume . always for now
            case 'e':
            case 'E':
                ++_pos;
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
    _number = 23; // ToDo
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
            break;
        case JSON_TRUE_TOKEN:
            _value_type = JSON_BOOLEAN;
             break;
        case JSON_FALSE_TOKEN:
            _value_type = JSON_BOOLEAN;
            break;
        case JSON_NUMBER_TOKEN:
            _value_type = JSON_DECIMAL;
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
            case JSON_STRING:
                std::cout.write(Value(), ValueLength());
                std::cout.write("\n",1);
                break;
        }
    }
    return 1;
}
