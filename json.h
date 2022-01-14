#pragma once

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <variant>

namespace json {

class Node;
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;
using Data = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

std::ostream& operator<<(std::ostream& out, const Dict& dict);

std::ostream& operator<<(std::ostream& out, const Array& array);

class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node {
public:
    Node() = default;
    Node(Array data);
    Node(Dict data);
    Node(int data);
    Node(bool data);
    Node(double data);
    Node(std::string data);
    Node(std::nullptr_t data);
    bool IsNull() const;
    bool IsInt() const;
    bool IsBool() const;
    bool IsDouble() const;
    bool IsString() const;
    bool IsArray() const;
    bool IsMap() const;
    bool IsPureDouble() const;
    const Array& AsArray() const;
    const Dict& AsMap() const;
    int AsInt() const;
    bool AsBool() const;
    double AsDouble() const;
    double AsPureDouble() const;
    const std::string& AsString() const;
    bool operator==(const Node& rhs) const;
    bool operator!=(const Node& rhs) const;
private:
    Data data_;
};

std::ostream& operator<<(std::ostream& out, const Node& node);

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;
    bool operator==(const Document& rhs) const;
    bool operator!=(const Document& rhs) const;
private:
    Node root_;
};

Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);

}  // namespace json
