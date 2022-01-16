#include "json.h"

using namespace std;

namespace json {

namespace {

Node LoadNode(istream& input);

using Number = std::variant<int, double>;

Node LoadNumber(std::istream& input) {
    using namespace std::literals;

    std::string parsed_num;

    // Считывает в parsed_num очередной символ из input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };

    // Считывает одну или более цифр в parsed_num из input
    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw ParsingError("A digit is expected"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }
    // Парсим целую часть числа
    if (input.peek() == '0') {
        read_char();
        // После 0 в JSON не могут идти другие цифры
    } else {
        read_digits();
    }

    bool is_int = true;
    // Парсим дробную часть числа
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // Парсим экспоненциальную часть числа
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            // Сначала пробуем преобразовать строку в int
            try {
                return Node(std::stoi(parsed_num));
            } catch (...) {
                // В случае неудачи, например, при переполнении,
                // код ниже попробует преобразовать строку в double
            }
        }
        return Node(std::stod(parsed_num));
    } catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}

Node LoadString(istream& input) {
    string line;
    while (char c = input.get()) {
        if (c == '\\') {
            c = input.get();
            switch (c) {
                case '"':
                    line += "\"";
                    break;
                case 'n':
                    line += "\n";
                    break;
                case 'r':
                    line += "\r";
                    break;
                case 't':
                    line += "\t";
                    break;
                default:
                    line += "\\";
                    break;
            }
        } else if (c == '"') {
            break;
        } else if (c == EOF) {
            throw ParsingError("There's no second \""s);
        } else {
            line += c;
        }
        
    }
    return Node(line);
}

Node LoadArray(istream& input) {
    Array result;
    
    for (char c; input >> c;) {
        if (c == ']') {
            return Node(move(result));
        }
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }
    throw ParsingError("there's no closed paren's"s);
}

Node LoadDict(istream& input) {
    Dict result;
    for (char c; input >> c;) {
        if (c == '}') {
            return Node(move(result));
        }
        if (c == ',') {
            input >> c;
        }

        string key = LoadString(input).AsString();
        input >> c;
        result.insert({move(key), LoadNode(input)});
    }
    throw ParsingError("there's no closed paren's"s);
}

Node LoadNull(istream& input) {
    string line;
    line += 'n';
    for (char c; input >> c;) {
        if (c == ',' || c == ']' || c == '}') {
            input.putback(c);
            break;
        }
        line += c;
    }
    if (line != "null"s)
        throw ParsingError("Wrong string"s);
    return Node();
}

Node LoadBool(istream& input) {
    string line;
    for (char c; input >> c;) {
        if (c == ',' || c == ']' || c == '}') {
            input.putback(c);
            break;
        }
        line += c;
    }
    if (line == "false"s) {
        return Node(false);
    } else if (line == "true"s) {
        return Node(true);
    } else {
        throw ParsingError("Wrong string"s);
    }
}

Node LoadNode(istream& input) {
    char c;
    input >> c;

    if (c == '[') {
        return LoadArray(input);
    } else if (c == '{') {
        return LoadDict(input);
    } else if (c == '"') {
        return LoadString(input);
    } else if (c == 'n') {
        return LoadNull(input);
    } else if (c == 't' || c == 'f') {
        input.putback(c);
        return LoadBool(input);
    } else {
        input.putback(c);
        return LoadNumber(input);
    }
}

}  // namespace

bool Node::operator==(const Node& rhs) const {
    return static_cast<Data>(*this) == rhs;
}

bool Node::operator!=(const Node& rhs) const {
    return !(*this == rhs);
}

std::ostream& operator<<(std::ostream& out, const Dict& dict) {
    out << '{'<< endl;
    for (auto it = dict.begin(); it != dict.end(); ++it) {
        out << '"' << it->first << '"'  << ':' << it->second;
        if (std::next(it) != dict.end()) {
            out << ',';
       }
       out << endl;
    }
    out << '}';
    return out;
}

std::ostream& operator<<(std::ostream& out, const Array& array) {
    out << '[';
    int array_size = static_cast<int>(array.size());
    for( int i = 0; i < array_size;++i) {
        out << array[i];
        if (i != array_size-1) {
            out << ',';
        }
    }
    out << ']';
    return out;
}


std::ostream& operator<<(std::ostream& out, const Node& node) {
    if (node.IsNull()) {
        out << "null"s;
    } else if (node.IsInt()) {
        out << node.AsInt();
    } else if (node.IsString()) {
        out << "\""s;
        for (const auto c : node.AsString()) {
            switch (c) {
                case '"':
                    out << "\\\""s;
                    break;
                case '\n':
                    out << "\\n"s;
                    break;
                case '\r':
                    out << "\\r"s;
                    break;
                case '\t':
                    out << "\\t"s;
                    break;
                case '\\':
                    out << "\\"s;
                    break;
                default:
                    out << c;
                    break;
            }
        }
        out << "\"";
    } else if (node.IsMap()) {
        out << node.AsMap();
    } else if (node.IsArray()) {
        out << node.AsArray();
    } else if (node.IsBool()) {
        if (node.AsBool()) {
            out << "true"s;
        } else {
            out << "false"s;
        }
    } else if (node.IsDouble()) {
        out << node.AsDouble();
    } else {
        out << node.AsPureDouble();
    }
    return out;
}

bool Node::IsNull() const {
    return holds_alternative<std::nullptr_t>(*this);
}

bool Node::IsInt() const {
    return holds_alternative<int>(*this);
}

bool Node::IsBool() const {
    return holds_alternative<bool>(*this);
}
bool Node::IsDouble() const {
    return holds_alternative<double>(*this) || IsInt();
}

bool Node::IsPureDouble() const {
    return holds_alternative<double>(*this);
}

bool Node::IsString() const {
    return holds_alternative<string>(*this);
}

bool Node::IsArray() const {
    return holds_alternative<Array>(*this);
}

bool Node::IsMap() const {
    return holds_alternative<Dict>(*this);
}

const Array& Node::AsArray() const {
    if (!IsArray()) {
        throw logic_error("not array"s);
    }
    return get<Array>(*this);
}

const Dict& Node::AsMap() const {
    if (!IsMap()) {
        throw logic_error("not map"s);
    }
    return get<Dict>(*this);
}

int Node::AsInt() const {
    if (!IsInt()) {
        throw logic_error("not int"s);
    }
    return get<int>(*this);
}

bool Node::AsBool() const {
    if (!IsBool()) {
        throw logic_error("not bool"s);
    }
    return get<bool>(*this);
}

const string& Node::AsString() const {
    if (!IsString()) {
        throw logic_error("not string"s);
    }
    return get<string>(*this);
}

double Node::AsDouble() const {
    if (!IsInt() && !IsDouble()) {
        throw logic_error("not double"s);
    }
    if (const double* data = get_if<double>(&*this)) {
        return *data;
    } else {
        return get<int>(*this);
    }
}

double Node::AsPureDouble() const {
    if (!IsPureDouble()) {
        throw logic_error("not pure double"s);
    }
    return get<double>(*this);
}

Document::Document(Node root)
    : root_(move(root)) {
}

bool Document::operator==(const Document& rhs) const {
    return root_ == rhs.root_;
}

bool Document::operator!=(const Document& rhs) const {
    return root_ != rhs.root_;
}

const Node& Document::GetRoot() const {
    return root_;
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

void Print(const Document& doc, std::ostream& output) {
    output <<  doc.GetRoot();
}

}  // namespace json
