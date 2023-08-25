#pragma once

#include "ranges.h"

#include <cstdlib>
#include <vector>

namespace graph
{

    using VertexId = size_t;    // Id �������
    using EdgeId = size_t;      // Id �����

    struct InfoEdge
    {
        std::string_view name_bus = "";
        int span_count = 0;
        std::string_view stop_name_from;
        std::string_view stop_name_to;
    };

    template <typename Weight>
    struct Edge         // �����
    {
        InfoEdge info_edge;
        VertexId from;  // ��������� ��
        VertexId to;    // ��������� ��
        Weight weight;  // ����������
    };

    template <typename Weight>
    class DirectedWeightedGraph // ������������ ���������� ������
    {
    private:
        using IncidenceList = std::vector<EdgeId>;  // ������ id �����
        using IncidentEdgesRange = ranges::Range<typename IncidenceList::const_iterator>;   // �������� ����� begin-end

    public:
        DirectedWeightedGraph() = default;
        explicit DirectedWeightedGraph(size_t vertex_count);
        EdgeId AddEdge(const Edge<Weight>& edge);

        size_t GetVertexCount() const;
        size_t GetEdgeCount() const;
        const Edge<Weight>& GetEdge(EdgeId edge_id) const;
        IncidentEdgesRange GetIncidentEdges(VertexId vertex) const;

    private:
        std::vector<Edge<Weight>> edges_;
        std::vector<IncidenceList> incidence_lists_;
    };

    template <typename Weight>
    DirectedWeightedGraph<Weight>::DirectedWeightedGraph(size_t vertex_count) : incidence_lists_(vertex_count) {}

    template <typename Weight>
    EdgeId DirectedWeightedGraph<Weight>::AddEdge(const Edge<Weight>& edge) // ������� �����
    {
        edges_.push_back(edge);                         // ������� ����� � ������ �����
        const EdgeId id = edges_.size() - 1;            // ������� id �����
        incidence_lists_.at(edge.from).push_back(id);   // ������� � ����� �� ��� (,at(Id ������� [��������� ��])) -> id ������
        return id;                                      // ���������� id ������
    }

    template <typename Weight>
    size_t DirectedWeightedGraph<Weight>::GetVertexCount() const    // ���� ���-�� ������
    {
        return incidence_lists_.size();
    }

    template <typename Weight>
    size_t DirectedWeightedGraph<Weight>::GetEdgeCount() const     // ���� ���-�� �����
    {
        return edges_.size();
    }

    template <typename Weight>
    const Edge<Weight>& DirectedWeightedGraph<Weight>::GetEdge(EdgeId edge_id) const    // ���� ����� ��� edge_id
    {
        return edges_.at(edge_id);
    }

    template <typename Weight>
    typename DirectedWeightedGraph<Weight>::IncidentEdgesRange
        DirectedWeightedGraph<Weight>::GetIncidentEdges(VertexId vertex) const
    {
        return ranges::AsRange(incidence_lists_.at(vertex));    // ���� �������� ��� ������� ��� -> id �����
    }
}  // namespace graph