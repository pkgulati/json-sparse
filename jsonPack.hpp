//
//  JsonPack.hpp
//  JsonPack
//
//  Created by kpraveen on 2018-11-05.
//  Copyright © 2018 Praveen. All rights reserved.
//

#ifndef JsonPack_hpp
#define JsonPack_hpp

#include <stdio.h>

class JsonPack;
class JsonArrayPack;

class JsonPack {
  
public:
    JsonPack();
    ~JsonPack();
    void resize(size_t size);
  
    void packInt(char* key, int value);
    void packBoolean(char* key, int value);
    void packChar(char* key, char value);
    void packString(char* key, char* value);
    void packnString(char* key, char* value, int len);
    void packDouble(char* key, double value);
    void packArray(char* key, JsonArrayPack& array);
    void packRate(char* key, double rate);
    void packObject(char* key, JsonPack& obj);
    void dump();
    void close();
    char* pack();
    size_t size();
    
private:
    void    _move(size_t t);
    void    *buffer;
    char    *current;
    size_t  capacity;
    int     members;
    int     status;
};

class JsonArrayPack {
public:
    JsonArrayPack();
    ~JsonArrayPack();
    void resize(size_t size);
    void packInt(int value);
    void packString(char* value);
    void packnString(char* value, int len);
    void packDouble(double value);
    void packRate(double rate);
    void packObject(JsonPack& obj);
    void packArray(JsonArrayPack& array);
    void dump();
    void close();
    char* pack();
    size_t size();
    
private:
    void    _move(size_t t);
    void    *buffer;
    char    *current;
    size_t  capacity;
    int     members;
    int     status;

};

#endif /* JsonPack_hpp */
