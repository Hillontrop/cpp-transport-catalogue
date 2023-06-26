#include "json.h"

using namespace std;

namespace json
{
// +++++++++++++++++++++++++++++ class Node begin +++++++++++++++++++++++++++++ //
    bool Node::IsNull() const
    {
        // �������� ���� ��� ������ std::holds_alternative().
        return std::holds_alternative<std::nullptr_t>(*this);
    }

    bool Node::IsInt() const
    {
        // �������� ���� ��� ������ std::holds_alternative().
        return std::holds_alternative<int>(*this);
    }

    bool Node::IsDouble() const
    {
        // �������� ���� ��� ������ std::holds_alternative().
        return std::holds_alternative<double>(*this) || std::holds_alternative<int>(*this);
    }

    bool Node::IsPureDouble() const
    {
        // �������� ���� ��� ������ std::holds_alternative().
        return std::holds_alternative<double>(*this);
    }

    bool Node::IsString() const
    {
        // �������� ���� ��� ������ std::holds_alternative().
        return std::holds_alternative<std::string>(*this);
    }

    bool Node::IsArray() const
    {
        // �������� ���� ��� ������ std::holds_alternative().
        return std::holds_alternative<Array>(*this);
    }

    bool Node::IsMap() const
    {
        // �������� ���� ��� ������ std::holds_alternative().
        return std::holds_alternative<Dict>(*this);
    }

    bool Node::IsBool() const
    {
        return std::holds_alternative<bool>(*this);
    }

    int Node::AsInt() const
    {
        if (Node::IsInt())
        {
            // ����� �� ���������, ��� variant ������ ������ int,
            // ����� ����� ����� ��� �������� ��� ������ std::get.
            return std::get<int>(*this);
        }
        else
        {
            throw std::logic_error("not an int");
        }
    }

    bool Node::AsBool() const
    {
        if (Node::IsBool())
        {
            return std::get<bool>(*this);
        }
        else
        {
            throw std::logic_error("not a bool");
        }
    }

    double Node::AsDouble() const
    {
        if (Node::IsInt())
        {
            return static_cast<double>(std::get<int>(*this));
        }
        else if (IsDouble())
        {
            return std::get<double>(*this);
        }
        else
        {
            throw std::logic_error("not a double");
        }
    }

    const std::string Node::AsString() const
    {
        if (Node::IsString())
        {
            return std::get<std::string>(*this);
        }
        else
        {
            throw std::logic_error("not a string");
        }
    }

    const Array& Node::AsArray() const
    {
        if (Node::IsArray())
        {
            return std::get<Array>(*this);
        }
        else
        {
            throw std::logic_error("not a array");
        }
    }

    const Dict& Node::AsMap() const
    {
        if (Node::IsMap())
        {
            return std::get<Dict>(*this);
        }
        else
        {
            throw std::logic_error("not a dict");
        }
    }
// _____________________________ class Node end _____________________________ //


// +++++++++++++++++++++++++++++ class Document begin +++++++++++++++++++++++++++++ //
    Document::Document(Node root) : root_(move(root))
    {
    }

    const Node& Document::GetRoot() const
    {
        return root_;
    }
// _____________________________ class Document end _____________________________ //


// ----------------------------- �������������� ������� ----------------------------- //
    namespace load
    {
        Node LoadNode(istream& input);

        Document Load(istream& input)
        {
            return Document{ LoadNode(input) };
        }

        Number LoadNumber(std::istream& input)
        {
            using namespace std::literals;

            std::string parsed_num;

            // ��������� � parsed_num ��������� ������ �� input
            auto read_char = [&parsed_num, &input]
            {
                parsed_num += static_cast<char>(input.get());
                if (!input)
                {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // ��������� ���� ��� ����� ���� � parsed_num �� input
            auto read_digits = [&input, read_char]
            {
                if (!std::isdigit(input.peek()))
                {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek()))
                {
                    read_char();
                }
            };

            if (input.peek() == '-')
            {
                read_char();
            }
            // ������ ����� ����� �����
            if (input.peek() == '0')
            {
                read_char();
                // ����� 0 � JSON �� ����� ���� ������ �����
            }
            else
            {
                read_digits();
            }

            bool is_int = true;
            // ������ ������� ����� �����
            if (input.peek() == '.')
            {
                read_char();
                read_digits();
                is_int = false;
            }

            // ������ ���������������� ����� �����
            if (int ch = input.peek(); ch == 'e' || ch == 'E')
            {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-')
                {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try
            {
                if (is_int)
                {
                    // ������� ������� ������������� ������ � int
                    try
                    {
                        return std::stoi(parsed_num);
                    }
                    catch (...)
                    {
                        // � ������ �������, ��������, ��� ������������,
                        // ��� ���� ��������� ������������� ������ � double
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...)
            {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        // ��������� ���������� ���������� �������� JSON-���������
        // ������� ������� ������������ ����� ���������� ������������ ������� ":
        std::string LoadString(std::istream& input)
        {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true)
            {
                if (it == end)
                {
                    // ����� ���������� �� ����, ��� ��������� ����������� �������?
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"')
                {
                    // ��������� ����������� �������
                    ++it;
                    break;
                }
                else if (ch == '\\')
                {
                    // ��������� ������ escape-������������������
                    ++it;
                    if (it == end)
                    {
                        // ����� ���������� ����� ����� ������� �������� ����� �����
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // ������������ ���� �� �������������������: \\, \n, \t, \r, \"
                    switch (escaped_char)
                    {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        // ��������� ����������� escape-������������������
                        throw json::ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r')
                {
                    // ��������� ������� ������- JSON �� ����� ����������� ��������� \r ��� \n
                    throw json::ParsingError("Unexpected end of line"s);
                }
                else
                {
                    // ������ ��������� ��������� ������ � �������� ��� � �������������� ������
                    s.push_back(ch);
                }
                ++it;
            }
            return s;
        }

        json::Array LoadArray(std::istream& input)
        {
            json::Array result;

            char c;
            while (input >> c && c != ']')
            {
                if (c != ',')
                {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
                if (input.eof() && c != ']')
                {
                    throw json::ParsingError("Array parsing error: Missing closing bracket");
                }
            }
            if (c != ']')
            {
                throw json::ParsingError("Array parsing error: Missing closing bracket");
            }
            return result;
        }

        json::Dict LoadDict(std::istream& input)
        {
            json::Dict result;

            char c;
            while (input >> c && c != '}')
            {
                if (c == ',')
                {
                    input >> c;
                }

                std::string key = LoadString(input);
                if (!(input >> c) || c != ':')
                {
                    throw json::ParsingError("Dict parsing error: Missing colon");
                }
                result.insert({ std::move(key), LoadNode(input) });
                if (input.eof() && c != '}')
                {
                    throw json::ParsingError("Dict parsing error: Missing closing bracket");
                }
            }
            if (c != '}')
            {
                throw json::ParsingError("Dict parsing error: Missing closing bracket");
            }
            return result;
        }


        bool LoadBool(std::istream& input)
        {
            std::string bool_str;
            char c;
            int count = input.peek() == 't' ? 4 : 5;
            for (size_t i = 0; i < count; ++i)  // 4/5
            {
                input >> c;
                if (!std::isalpha(c) && input.eof())
                {
                    throw json::ParsingError("Failed to parse boolean value");
                }
                bool_str.push_back(c);

                if (bool_str == "true" || bool_str == "false")     // true/false
                {
                    return bool_str == "true";    // true/false
                }
                if (input.eof())
                {
                    throw json::ParsingError("Failed to parse boolean value");
                }
            }
            //if (bool_str == "true" || bool_str == "false")     // true/false
            //{
                return bool_str == "true";    // true/false
            //}
        }


        Node LoadNode(istream& input)
        {
            char firstChar = input.peek();
            while (firstChar == '\t' || firstChar == '\r' || firstChar == '\n' || firstChar == ' ')
            {
                input.get(); // ������� ������ �� ������
                firstChar = input.peek(); // �������� ��������� ������
            }

            if (firstChar == '[')
            {
                input.get(); // ������� ������ �� ������
                return Node(LoadArray(input));
            }
            else if (firstChar == '{')
            {
                input.get(); // ������� ������ �� ������
                return Node(LoadDict(input));
            }
            else if (firstChar == '"')
            {
                input.get(); // ������� ������ �� ������
                return Node(LoadString(input));//+
            }
            else if (std::isdigit(firstChar) || firstChar == '-') //+
            {
                Number number = LoadNumber(input);
                if (std::holds_alternative<int>(number))
                {
                    return get<int>(number);
                }
                else if (std::holds_alternative<double>(number))
                {
                    return get<double>(number);
                }
                else
                {
                    return "null";
                }
            }
            else if (firstChar == 't' || firstChar == 'f')
            {
                return Node(LoadBool(input));
            }
            else if (firstChar == 'n')
            {
                std::string null_str;
                while (input >> firstChar && std::isalpha(firstChar))
                {
                    null_str.push_back(firstChar);
                }
                if (null_str == "null")
                {
                    return Node(nullptr);
                }
                else
                {
                    throw ParsingError("Failed to parse null value");
                }
            }
            else
            {
                throw ParsingError("Unexpected character: " + std::string(1, firstChar));
            }
        }
    }   // namespace load

    namespace print
    {
        void Print(const Document& doc, std::ostream& output)
        {
            const Node& root = doc.GetRoot();
            PrintNode(root, { output });
        }

// +++++++++++++++++++++++++++++ class PrintContext begin +++++++++++++++++++++++++++++ //
        void PrintContext::PrintIndent() const
        {
            for (int i = 0; i < indent; ++i)
            {
                out.put(' ');
            }
        }

        PrintContext PrintContext::Indented() const
        {
            return { out, indent_step, indent_step + indent };
        }
// _____________________________ class PrintContext end _____________________________ //


        void PrintValue(std::nullptr_t, const PrintContext& ctx)
        {
            ctx.out << "null"sv;
        }

        void PrintValue(const Array& arr, const PrintContext& ctx)
        {
            ctx.out << "[";

            for (size_t i = 0; i < arr.size(); ++i)
            {
                PrintNode(arr[i], ctx);
                if (i != arr.size() - 1)
                {
                    ctx.out << ", ";
                }
                else
                {
                    ctx.out << "]";
                }
            }
            if (arr.empty())
            {
                ctx.out << "]";
            }
        }

        void PrintValue(const Dict& dict, const PrintContext& ctx)
        {
            ctx.out << "{";

            size_t count = 0;
            for (const auto& [str, node] : dict)
            {
                ctx.out << '"' << str << "\": ";
                PrintNode(node, ctx);
                ++count;

                if (count != static_cast<size_t>(dict.size()))
                {
                    ctx.out << ", ";
                }
                else
                {
                    ctx.out << "}";
                }
            }
            if (dict.empty())
            {
                ctx.out << "}";
            }
        }

        void PrintValue(const std::string& str, const PrintContext& ctx)
        {
            std::string new_str;
            for (size_t i = 0; i < str.size(); ++i)
            {
                switch (str[i])
                {
                case '\n':
                    new_str.push_back('\\');
                    new_str.push_back('n');
                    break;
                case '\r':
                    new_str.push_back('\\');
                    new_str.push_back('r');
                    break;
                case '"':
                    new_str.push_back('\\');
                    new_str.push_back('"');
                    break;
                case '\\':
                    new_str.push_back('\\');
                    new_str.push_back('\\');
                    break;
                default:
                    new_str.push_back(str[i]);
                    break;
                }
            }
            ctx.out << '"' << new_str << '"';
        }

        void PrintValue(bool t_f, const PrintContext& ctx)
        {
            ctx.out << std::boolalpha << t_f;
        }

        void PrintNode(const Node& node, const PrintContext& ctx)   // ��� ����� ���� ������
        {
            if (node.IsNull())
            {
                PrintValue(nullptr, ctx);
            }
            else if (node.IsInt())
            {
                PrintValue(node.AsInt(), ctx);
            }
            else if (node.IsDouble())
            {
                PrintValue(node.AsDouble(), ctx);
            }
            else if (node.IsString())
            {
                PrintValue(node.AsString(), ctx);
            }
            else if (node.IsArray())
            {
                PrintValue(node.AsArray(), ctx);
            }
            else if (node.IsMap())
            {
                PrintValue(node.AsMap(), ctx);
            }
            else if (node.IsBool())
            {
                PrintValue(node.AsBool(), ctx);
            }
        }
    }   // namespace print
}  // namespace json