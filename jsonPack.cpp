//
//  JsonPack.cpp
//  JsonPack
//
//  Created by kpraveen on 2018-11-05.
//  Copyright © 2018 Praveen. All rights reserved.
//

#include "JsonPack.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

JsonPack::JsonPack() {
    members = 0;
    capacity = 0;
    buffer = NULL;
    current = NULL;
    status = 0;
}

void
JsonPack::resize(size_t insize) {
    if (insize <= capacity) {
        return;
    }
     capacity = insize;
    if (buffer) {
        buffer = realloc(buffer, capacity);
    }
}

void
JsonPack::_move(size_t t) {
    if (!buffer) {
        if (capacity == 0) {
            capacity = 256;
        }
        if (t > capacity - 2) {
            capacity = t + 256;
        }
        buffer = malloc(capacity);
        current = (char*)buffer;
    }
    else {
        size_t used = (char*)current - (char*)buffer;
        if (used + t > capacity - 2) {
            capacity = 2 * (capacity) + 1024;
            void* ptr = realloc(buffer, capacity);
            if (!ptr) {
                printf("no memory\n");
                throw "memory error";
                return;
            }
            this->buffer = ptr;
            current = (char*)buffer + used;
        }
    }
    if (members) {
        *current = ',';
        current++;
    } else {
        *current = '{';
        current++;
    }
    members++;
}

JsonPack::~JsonPack()
{
    free(buffer);
}

void
JsonPack::packInt(char* key, int value) {
    _move(16);
    int ret = sprintf(current, "\"%s\"=%d", key, value);
    current += ret;
}

void
JsonPack::packString(char* key, char* value) {
    _move(strlen(key)+strlen(value) + 10);
    int ret = sprintf(current, "\"%s\"=\"%s\"", key, value);
    current += ret;
}

void
JsonPack::packnString(char* key, char* value, int len) {
    _move(strlen(key)+len+10);
    int ret = sprintf(current, "\"%s\"=\"%.*s\"", key, len, value);
    current += ret;
}

void
JsonPack::packDouble(char* key, double value) {
    _move(strlen(key)+36);
    int ret = sprintf(current, "\"%s\"=%f", key, value);
    current += ret;
}

void
JsonPack::packRate(char* key, double rate) {
    _move(strlen(key)+36);
    int counter = 0;
    double temp = rate;
    if (temp < 0) {
        temp = -temp;
    }
    while(temp>0){
        temp=temp/10;
        counter+=1;
    }
    int ret = sprintf(current, "\"%s\"=%.*f", key, 10, rate);
    current += ret;
}

void
JsonPack::packObject(char *key, JsonPack &obj){
    _move(strlen(key)+10+obj.size());
    int ret = sprintf(current, "\"%s\"=%.*s", key, obj.size(), obj.pack());
    current += ret;
}

void
JsonPack::packArray(char *key, JsonArrayPack &obj){
    _move(strlen(key)+10+obj.size());
    int ret = sprintf(current, "\"%s\"=%.*s", key, obj.size(), obj.pack());
    current += ret;
}

void
JsonPack::dump() {
    size_t used = (char*)current - (char*)buffer;
    printf("buf %.*s\n", used, buffer);
    printf("size %d\n", capacity);
    printf("members %d\n", members);
}

void
JsonPack::close() {
    *current = '}';
    current++;
    status = 1;
}

char*
JsonPack::pack() {
    return (char*)buffer;
}

size_t
JsonPack::size() {
    return (char*)current - (char*)buffer;
};



JsonArrayPack::JsonArrayPack() {
    members = 0;
    capacity = 0;
    buffer = NULL;
    current = NULL;
    status = 0;
}

void
JsonArrayPack::resize(size_t insize) {
    if (insize <= capacity) {
        return;
    }
    capacity = insize;
    if (buffer) {
        buffer = realloc(buffer, capacity);
    }
}

void
JsonArrayPack::_move(size_t t) {
    if (!buffer) {
        if (capacity == 0) {
            capacity = 256;
        }
        if (t > capacity - 2) {
            capacity = t + 256;
        }
        buffer = malloc(capacity);
        current = (char*)buffer;
    }
    else {
        size_t used = (char*)current - (char*)buffer;
        if (used + t > capacity - 2) {
            capacity = 2 * (capacity) + 1024;
            void* ptr = realloc(buffer, capacity);
            if (!ptr) {
                printf("no memory\n");
                throw "memory error";
                return;
            }
            this->buffer = ptr;
            current = (char*)buffer + used;
        }
    }
    if (members) {
        *current = ',';
        current++;
    } else {
        *current = '[';
        current++;
    }
    members++;
}

JsonArrayPack::~JsonArrayPack()
{
    free(buffer);
}

void
JsonArrayPack::packInt(int value) {
    _move(16);
    int ret = sprintf(current, "%d",  value);
    current += ret;
}

void
JsonArrayPack::packString(char* value) {
    _move(strlen(value) + 10);
    int ret = sprintf(current, "\"%s\"", value);
    current += ret;
}

void
JsonArrayPack::packnString(char* value, int len) {
    _move(len+10);
    int ret = sprintf(current, "\"%.*s\"", len, value);
    current += ret;
}

void
JsonArrayPack::packDouble(double value) {
    _move(32);
    int ret = sprintf(current, "%f", value);
    current += ret;
}

void
JsonArrayPack::packRate(double rate) {
    _move(32);
    int counter = 0;
    double temp = rate;
    if (temp < 0) {
        temp = -temp;
    }
    while(temp>0){
        temp=temp/10;
        counter+=1;
    }
    int ret = sprintf(current, "%.*f", 10, rate);
    current += ret;
}

void
JsonArrayPack::packObject(JsonPack &obj){
    _move(obj.size()+10);
    int ret = sprintf(current, "%.*s", obj.size(), obj.pack());
    current += ret;
}

void
JsonArrayPack::packArray(JsonArrayPack &obj){
    _move(10+obj.size());
    int ret = sprintf(current, "%.*s", obj.size(), obj.pack());
    current += ret;
}

void
JsonArrayPack::dump() {
    size_t used = (char*)current - (char*)buffer;
    printf("buf %.*s\n", used, buffer);
    printf("size %d\n", capacity);
    printf("members %d\n", members);
}

void
JsonArrayPack::close() {
    *current = ']';
    current++;
    status = 1;
}

char*
JsonArrayPack::pack() {
    return (char*)buffer;
}

size_t
JsonArrayPack::size() {
    return (char*)current - (char*)buffer;
};


