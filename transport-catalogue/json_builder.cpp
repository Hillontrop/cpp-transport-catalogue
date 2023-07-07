#include "json_builder.h"

namespace json
{
    Builder::BaseContext Builder::EndArray()
    {
        if (!nodes_stack_.back()->IsArray()) // ��������� � ��� �� ������� �����? ���� �� Array - �� ���� ������� ����������. ����� - ������� �� ����� �������.
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
        if (!nodes_stack_.back()->IsDict()) // ��������� � ��� �� ������� �����? ���� �� Dict - �� ���� ������� ����������. ����� - ������� �� ����� �������.
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
        // ��� ����������� ������� ����� ��������� �������� ����� ��� ��������� ���� ����-��������.
        // ��������� ����� ������ ����������� ������ �������� ��������������� ����� ����� �������� �
        // ������� ������ Value ��� �������� ��� ����������� � ������� StartDict ��� StartArray.

        current_key_ = key;
        availability_key_ = true;
        return Builder::KeyItemContext(*this);
    }

    Builder::DictItemContext Builder::StartDict()
    {
        // �������� ����������� �������� �������� - �������.���������� � ��� �� ����������, ��� � Value.
        // ��������� ������� ����������� ������ ���� Key ��� EndDict.

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
        // �������� ����������� �������� ��������-�������. ���������� � ��� �� ����������, ��� � Value.
        // ��������� ������� ����������� ������ ���� EndArray ��� �����, �������� ����� ��������: Value, StartDict ��� StartArray.
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
        // ���������� ������ json::Node, ���������� JSON, ��������� ����������� �������� �������.� ����� �������
        // ��� ������� Start* ������ ���� ������ ��������������� End* .��� ���� ��� ������ ������ ���� ��������,
        // �� ���� ����� json::Builder{}.Build() ����������.

        if (!nodes_stack_.empty())
        {
            throw std::logic_error("Invalid state -> root_");
        }

        return root_;
    }
}