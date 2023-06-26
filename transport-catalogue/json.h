#pragma once

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace json
{
    class Node;
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;
    using Number = std::variant<int, double>;

// +++++++++++++++++++++++++++++ class ParsingError begin +++++++++++++++++++++++++++++ //

    // ��� ������ ������ ������������� ��� ������� �������� JSON
    class ParsingError : public std::runtime_error    
    {
    public:
        using runtime_error::runtime_error;
    };
// _____________________________ class ParsingError end _____________________________ //


// +++++++++++++++++++++++++++++ class Node begin +++++++++++++++++++++++++++++ //
    class Node final : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>    // ��������� ������������ �� variant
    {
    public:
        using variant::variant;

        bool IsNull() const;
        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsString() const;
        bool IsArray() const;
        bool IsMap() const;
        bool IsBool() const;

        int AsInt() const;
        bool AsBool() const;
        double AsDouble() const;
        const std::string AsString() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;

        bool operator==(const Node& other) const    // �������� ����������� (==)
        {
            if ((*this).index() != other.index())
            {
                return false; // ������ ���� ��������
            }
            return (*this) == other;
        }

        bool operator!=(const Node& other) const    // �������� ����������� (!=)
        {
            return !(*this == other);
        }
    };
// _____________________________ class Node end _____________________________ //


// +++++++++++++++++++++++++++++ class Document begin +++++++++++++++++++++++++++++ //
    class Document
    {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

        bool operator==(const Document& other) const
        {
            return root_ == other.root_;
        }

        bool operator!=(const Document& other) const
        {
            return !(*this == other);
        }

    private:
        Node root_;
    };

// ----------------------------- �������������� ������� ----------------------------- //
    namespace load
    {
        Document Load(std::istream& input);
        Number LoadNumber(std::istream& input);
        std::string LoadString(std::istream& input);
        Array LoadArray(std::istream& input);
        Dict LoadDict(std::istream& input);
        bool LoadBool(std::istream& input);
    }

    namespace print
    {
        void Print(const Document& doc, std::ostream& output);

// +++++++++++++++++++++++++++++ class PrintContext begin +++++++++++++++++++++++++++++ //
        // �������� ������, ������ ������ �� ����� ������ � ������� �����
        struct PrintContext
        {
            std::ostream& out;
            int indent_step = 4;
            int indent = 0;

            void PrintIndent() const;

            PrintContext Indented() const;      // ���������� ����� �������� ������ � ����������� ���������
        };
// _____________________________ class PrintContext end _____________________________ //


        // ������, ���������� ��� ������ double � int
        template <typename Value>
        void PrintValue(const Value& value, const PrintContext& ctx);

        // ���������� ������� PrintValue ��� ������ �������� null
        void PrintValue(std::nullptr_t, const PrintContext& ctx);

        // ���������� ������� PrintValue ��� ������ �������� Array
        void PrintValue(const Array& arr, const PrintContext& ctx);

        // ���������� ������� PrintValue ��� ������ �������� Dict
        void PrintValue(const Dict& dict, const PrintContext& ctx);

        // ���������� ������� PrintValue ��� ������ �������� string
        void PrintValue(const std::string& str, const PrintContext& ctx);

        // ���������� ������� PrintValue ��� ������ �������� bool
        void PrintValue(bool t_f, const PrintContext& ctx);

        // ������ ���������� ������� PrintValue ������� ����������
        void PrintNode(const Node& node, const PrintContext& ctx);
    }


// ----------------------------- ��������� ������ � ������� ----------------------------- //
    template <typename Value>
    void print::PrintValue(const Value& value, const print::PrintContext& ctx)
    {
        ctx.out << value;
    }
}  // namespace json