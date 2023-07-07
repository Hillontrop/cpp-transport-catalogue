#include "json_builder.h"

namespace json
{
    Builder::BaseContext Builder::EndArray()
    {
        if (!nodes_stack_.back()->IsArray()) // проверяем А что на вершине стека? Если не Array - то надо бросать исключение. Иначе - снимаем со стека вершину.
        {
            throw std::logic_error("Invalid state -> EndArray called without StartArray");
        }
        else
        {
            nodes_stack_.pop_back();
        }
        return Builder::BaseContext(*this);
    }

    Builder::BaseContext Builder::EndDict()
    {
        if (!nodes_stack_.back()->IsDict()) // проверяем А что на вершине стека? Если не Dict - то надо бросать исключение. Иначе - снимаем со стека вершину.
        {
            throw std::logic_error("Invalid state -> EndDict called without StartDict");
        }
        else
        {
            nodes_stack_.pop_back();
        };
        return Builder::BaseContext(*this);
    }

    Builder::KeyItemContext Builder::Key(const std::string& key)
    {
        // При определении словаря задаёт строковое значение ключа для очередной пары ключ-значение.
        // Следующий вызов метода обязательно должен задавать соответствующее этому ключу значение с
        // помощью метода Value или начинать его определение с помощью StartDict или StartArray.

        current_key_ = key;
        availability_key_ = true;
        return Builder::KeyItemContext(*this);
    }

    Builder::DictItemContext Builder::StartDict()
    {
        // Начинает определение сложного значения - словаря.Вызывается в тех же контекстах, что и Value.
        // Следующим вызовом обязательно должен быть Key или EndDict.

        if (nodes_stack_.empty())
        {
            nodes_stack_.push_back(&root_);
        }
        else
        {
            if (nodes_stack_.back()->IsArray())
            {
                nodes_stack_.back()->AsArray().push_back(Node());
                nodes_stack_.push_back(&nodes_stack_.back()->AsArray().back());
            }
            else if (nodes_stack_.back()->IsDict())
            {
                nodes_stack_.push_back(&nodes_stack_.back()->AsDict()[current_key_]);
                current_key_.clear();
                availability_key_ = false;
            }
            else
            {
                throw std::logic_error("Invalid state -> other");
            }
        }
        *nodes_stack_.back() = Dict{};
        return Builder::DictItemContext(*this);
    }

    Builder::ArrayItemContext Builder::StartArray()
    {
        // Начинает определение сложного значения-массива. Вызывается в тех же контекстах, что и Value.
        // Следующим вызовом обязательно должен быть EndArray или любой, задающий новое значение: Value, StartDict или StartArray.
        if (nodes_stack_.empty())
        {
            nodes_stack_.push_back(&root_);
        }
        else
        {
            if (nodes_stack_.back()->IsArray())
            {
                nodes_stack_.back()->AsArray().push_back(Node());
                nodes_stack_.push_back(&nodes_stack_.back()->AsArray().back());
            }
            else if (nodes_stack_.back()->IsDict())
            {
                nodes_stack_.push_back(&nodes_stack_.back()->AsDict()[current_key_]);
                current_key_.clear();
                availability_key_ = false;
            }
            else
            {
                throw std::logic_error("Invalid state -> other");
            }
        }
        *nodes_stack_.back() = Array{};
        return Builder::ArrayItemContext(*this);
    }

    Node Builder::Build()
    {
        // Возвращает объект json::Node, содержащий JSON, описанный предыдущими вызовами методов.К этому моменту
        // для каждого Start* должен быть вызван соответствующий End* .При этом сам объект должен быть определён,
        // то есть вызов json::Builder{}.Build() недопустим.

        if (!nodes_stack_.empty())
        {
            throw std::logic_error("Invalid state -> root_");
        }

        return root_;
    }
}