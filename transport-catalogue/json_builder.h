#pragma once

#include "json.h"

#include <variant>

namespace json
{
    class Builder
    {
    private:
        class BaseContext;
        class DictItemContext;
        class ArrayItemContext;
        class KeyItemContext;
    public:
        BaseContext EndArray();
        BaseContext EndDict();
        DictItemContext StartDict();
        KeyItemContext Key(const std::string& key);
        ArrayItemContext StartArray();
        Node Build();

        template<typename T>
        BaseContext Value(T value);
    private:

        class BaseContext
        {
        public:
            BaseContext(Builder& base) : builder_(base) {}

            template<typename T>
            BaseContext Value(T value)
            {
                return builder_.Value(value);
            }

            BaseContext StartArray()
            {
                return builder_.StartArray();
            }

            BaseContext EndArray()
            {
                return builder_.EndArray();
            }

            BaseContext StartDict()
            {
                return builder_.StartDict();
            }

            KeyItemContext Key(std::string key)
            {
                return builder_.Key(key);
            }

            BaseContext EndDict()
            {
                return builder_.EndDict();
            }

            Node Build()
            {
                return builder_.Build();
            }

        protected:
            Builder& builder_;
        };

        class DictItemContext : public BaseContext
        {
        public:
            DictItemContext(BaseContext base) : BaseContext(base)
            {
            }

            template<typename T>
            BaseContext Value(T value) = delete;

            BaseContext StartArray() = delete;
            BaseContext EndArray() = delete;
            BaseContext StartDict() = delete;
            Node Build() = delete;
        };

        class ArrayItemContext : public BaseContext
        {
        public:
            ArrayItemContext(BaseContext base) : BaseContext(base)
            {
            }

            template<typename T>
            ArrayItemContext Value(T value)
            {
                return builder_.Value(value);
            }

            BaseContext Key(std::string key) = delete;
            BaseContext EndDict() = delete;
            Node Build() = delete;
        };

        class KeyItemContext : public BaseContext
        {
        public:
            KeyItemContext(BaseContext base) : BaseContext(base)
            {
            }

            template<typename T>
            DictItemContext Value(T value)
            {
                return builder_.Value(value);
            }

            BaseContext EndArray() = delete;
            BaseContext Key(std::string key) = delete;
            BaseContext EndDict() = delete;
            Node Build() = delete;
        };

    private:
        Node root_;
        std::vector<Node*> nodes_stack_;
        std::string current_key_;
        bool availability_key_ = false;
    };

    template<typename T>
    Builder::BaseContext Builder::Value(T value)
    {
        // «адаЄт значение, соответствующее ключу при определении словар€, очередной элемент массива или,
        // если вызвать сразу после конструктора json::Builder, всЄ содержимое конструируемого JSON - объекта.
        // ћожет принимать как простой объект Ч число или строку Ч так и целый массив или словарь.
        // «десь Node::Value Ч это синоним дл€ базового класса Node, шаблона variant с набором возможных типов -
        // значений.—мотрите заготовку кода.

        if (root_ == nullptr)
        {
            root_ = Node(value);
            return BaseContext(*this);
        }
        else if (nodes_stack_.back()->IsDict() && availability_key_)
        {

            json::Dict& dict = std::get<json::Dict>(nodes_stack_.back()->GetValue());
            dict.emplace(current_key_, std::move(value));
            current_key_.clear();
            availability_key_ = false;
            return Builder::DictItemContext(*this);
        }
        else if (nodes_stack_.back()->IsArray())
        {
            json::Array& array = std::get<json::Array>(nodes_stack_.back()->GetValue());
            array.emplace_back(std::move(value));
            return Builder::ArrayItemContext(*this);
        }
        else
        {
            throw std::logic_error("Invalid state -> other");
        }
    }
}  // namespace json
