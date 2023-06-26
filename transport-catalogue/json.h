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

    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error    
    {
    public:
        using runtime_error::runtime_error;
    };
// _____________________________ class ParsingError end _____________________________ //


// +++++++++++++++++++++++++++++ class Node begin +++++++++++++++++++++++++++++ //
    class Node final : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>    // Добавлено наследование от variant
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

        bool operator==(const Node& other) const    // Оператор неравенства (==)
        {
            if ((*this).index() != other.index())
            {
                return false; // Разные типы значений
            }
            return (*this) == other;
        }

        bool operator!=(const Node& other) const    // Оператор неравенства (!=)
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

// ----------------------------- Дополнительные функции ----------------------------- //
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
        // Контекст вывода, хранит ссылку на поток вывода и текущий отсуп
        struct PrintContext
        {
            std::ostream& out;
            int indent_step = 4;
            int indent = 0;

            void PrintIndent() const;

            PrintContext Indented() const;      // Возвращает новый контекст вывода с увеличенным смещением
        };
// _____________________________ class PrintContext end _____________________________ //


        // Шаблон, подходящий для вывода double и int
        template <typename Value>
        void PrintValue(const Value& value, const PrintContext& ctx);

        // Перегрузка функции PrintValue для вывода значений null
        void PrintValue(std::nullptr_t, const PrintContext& ctx);

        // Перегрузка функции PrintValue для вывода значений Array
        void PrintValue(const Array& arr, const PrintContext& ctx);

        // Перегрузка функции PrintValue для вывода значений Dict
        void PrintValue(const Dict& dict, const PrintContext& ctx);

        // Перегрузка функции PrintValue для вывода значений string
        void PrintValue(const std::string& str, const PrintContext& ctx);

        // Перегрузка функции PrintValue для вывода значений bool
        void PrintValue(bool t_f, const PrintContext& ctx);

        // Другие перегрузки функции PrintValue пишутся аналогично
        void PrintNode(const Node& node, const PrintContext& ctx);
    }


// ----------------------------- Шаблонные методы и функции ----------------------------- //
    template <typename Value>
    void print::PrintValue(const Value& value, const print::PrintContext& ctx)
    {
        ctx.out << value;
    }
}  // namespace json