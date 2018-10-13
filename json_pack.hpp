//
// By pkgulati@gmail.com
// https://github.com/pkgulati/json-sparse

#ifndef json_pack_hpp
#define json_pack_hpp

#include <stdio.h>

enum JsonValueType
{
    JSON_NO_VALUE,
    JSON_STRING,
    JSON_DECIMAL,
    JSON_INTEGER,
    JSON_BOOLEAN,
    JSON_ARRAY,
    JSON_OBJECT,
    JSON_NULL,
    JSON_INVALID
};

//typedef void (JsonFn)(JsonValue* key, JsonValue* value, void*ctx);

enum JsonTokenType
{
    JSON_INVALID_TOKEN     = 0,
    JSON_BEGIN_TOKEN         = 1,     // {        1
    JSON_STRING_TOKEN         = 2,    // "asdasd"
    JSON_COLON_TOKEN         = 3,    // :
    JSON_ARRAY_SEPARATOR     = 4,    // ,
    JSON_END_TOKEN         = 5,     // }
    JSON_NUMBER_TOKEN         = 6,    // 23.45
    JSON_TRUE_TOKEN         = 7,    // true
    JSON_FALSE_TOKEN         = 8,    // false    -- 7
    JSON_NULL_TOKEN         = 9,    // null     -- 8
    JSON_ARRAY_BEGIN         = 10,   // [        -- 9
    JSON_ARRAY_END         = 11    // ]
};

class JsonPack {
    
public:
    
    JsonPack(char* ptr, int len);
   
    int ReadObject();
    int ReadMember();
    int ReadValue();
    int ReadArray();
    
    char* Key();
    int KeyLength();
    char* Value();
    int ValueLength();
    double Number();
    long Quantity();
    
    void PrintValue();
    
    JsonValueType ValueType();
    JsonTokenType TokenType();
    
private:
    double  _number;
    long    _qty;
    int     _array;
    char    * _key;
    int     _key_length;
    char    * _str;
    int     _strlen;
    bool _flag;
    JsonTokenType   _tokenType;
    JsonValueType   _value_type;
    char*   _start;
    char*   _end;
    char*   _pos;
    int _read_next_token();
    void _ReadNullToken();
    void _ReadTrueToken();
    void _ReadFalseToken();
    void _ReadNumberToken();
    void _ReadStringToken();
};

#endif /* json_pack_hpp */
