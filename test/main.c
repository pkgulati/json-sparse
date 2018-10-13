#include <iostream>

#include "json_pack.hpp"

int test1() {
    char str[1000] = "{\"name\":\"praveen\",\"age\":\"48\"}";
    JsonPack obj(str, strlen(str));
    while(obj.ReadMember()) {
        std::cout << "key ";
        std::cout.write(obj.Key(), obj.KeyLength());
        std::cout.write(obj.Value(), obj.ValueLength());
        std::cout.write("\n",1);
    }
    return 1;
}

int test2() {
    char str[1000] = "{\"name\":\"praveen\",\"tags\":[\"pk1\",\"pk2\"],\"subobj\":{\"foo\":\"bar\",\"foo2\":\"bar2\" },\"age\":\"48\"}";
    JsonPack obj(str, strlen(str));
    while(obj.ReadMember()) {
        std::cout << "key ";
        std::cout.write(obj.Key(), obj.KeyLength());
        switch (obj.ValueType()) {
            case JSON_ARRAY:
                while(obj.ReadValue()) {
                    std::cout.write(obj.Value(), obj.ValueLength());
                    std::cout.write("\n",1);
                }
                break;
            case JSON_OBJECT:
                while(obj.ReadMember()) {
                    std::cout << "key ";
                    std::cout.write(obj.Key(), obj.KeyLength());
                    std::cout.write(obj.Value(), obj.ValueLength());
                    std::cout.write("\n",1);
                }
                break;
            default:
                std::cout.write(obj.Value(), obj.ValueLength());
                std::cout.write("\n",1);
                break;
        }
    }
    return 1;
}


int test3() {
    char str[1000] = "{\"name\":\"praveen\",\"tags\":[\"pk1\",\"pk2\"],\"subobj\":{\"foo\":\"bar\",\"foo2\":\"bar2\" },\"age\":\"48\"}";
    JsonPack obj(str, strlen(str));
    std::cout << obj.TokenType() << std::endl;
    obj.ReadObject();
    return 1;
}

int test4() {
    char str[1000] = "[{\"name\":\"praveen\",\"tags\":[\"pk1\",\"pk2\"]},{\"subobj\":{\"foo\":\"bar\",\"foo2\":\"bar2\" }},{\"age\":\"48\"}]";
    JsonPack obj(str, strlen(str));
    std::cout << obj.TokenType() << std::endl;
    obj.ReadArray();
    return 1;
}

int test5() {
    char str[1000] = "{\"name\":\"praveen\",\"surname\":\"gulati\",\"age\":2312.99E3,\"flag\":true,\"remarks\":null}";
    JsonPack obj(str, strlen(str));
    obj.ReadObject();
    return 1;
}

int main(int argc, const char * argv[]) {
    test5();
    return 0;
}
