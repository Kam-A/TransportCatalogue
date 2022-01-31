#pragma once

#include <string>
#include <vector>

#include "json.h"

namespace json {

class DictItemContext;
class ArrayContext;
class KeyContext;

class Builder {
public:
    Builder() = default;
    DictItemContext StartDict();
    ArrayContext StartArray();
    KeyContext Key(std::string);
    Builder& Value(Node::Value);
    Builder& EndDict();
    Builder& EndArray();
    Node Build();
    ~Builder() = default;
private:
    bool IsEmpty();
    Node* GetCurrentNodePtr();
    bool StartContainer(Node node);
    Node root_;
    std::vector<Node*> nodes_stack_;
};

class Context {
public:
    Context(Builder& builder) : builder_(builder) {}
protected:
    Builder& builder_;
};

class KeyContext : public Context {
public:
    DictItemContext Value(Node::Value value);
    DictItemContext StartDict();
    ArrayContext StartArray();
};

class DictItemContext : public Context {
public:
    KeyContext Key(std::string key);
    Builder& EndDict();
};

class ArrayContext : public Context {
public:
    ArrayContext Value(Node::Value value);
    DictItemContext StartDict();
    ArrayContext StartArray();
    Builder& EndArray();
};


}
