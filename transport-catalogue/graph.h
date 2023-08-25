#pragma once

#include "ranges.h"

#include <cstdlib>
#include <vector>

namespace graph
{

    using VertexId = size_t;    // Id Вершины
    using EdgeId = size_t;      // Id Ребра

    struct InfoEdge
    {
        std::string_view name_bus = "";
        int span_count = 0;
        std::string_view stop_name_from;
        std::string_view stop_name_to;
    };

    template <typename Weight>
    struct Edge         // Ребро
    {
        InfoEdge info_edge;
        VertexId from;  // Остановка от
        VertexId to;    // Остановка до
        Weight weight;  // Расстояние
    };

    template <typename Weight>
    class DirectedWeightedGraph // Направленный взвешенный график
    {
    private:
        using IncidenceList = std::vector<EdgeId>;  // Вектор id ребер
        using IncidentEdgesRange = ranges::Range<typename IncidenceList::const_iterator>;   // Интервал ребер begin-end

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
    EdgeId DirectedWeightedGraph<Weight>::AddEdge(const Edge<Weight>& edge) // Добавть ребро
    {
        edges_.push_back(edge);                         // Добавть ребро в вектор ребер
        const EdgeId id = edges_.size() - 1;            // Получть id ребра
        incidence_lists_.at(edge.from).push_back(id);   // Добавть в вектр на поз (,at(Id Вершины [остаглвка от])) -> id ребера
        return id;                                      // Возвращаем id ребера
    }

    template <typename Weight>
    size_t DirectedWeightedGraph<Weight>::GetVertexCount() const    // Дать кол-во вершин
    {
        return incidence_lists_.size();
    }

    template <typename Weight>
    size_t DirectedWeightedGraph<Weight>::GetEdgeCount() const     // Дать кол-во ребер
    {
        return edges_.size();
    }

    template <typename Weight>
    const Edge<Weight>& DirectedWeightedGraph<Weight>::GetEdge(EdgeId edge_id) const    // дать ребро под edge_id
    {
        return edges_.at(edge_id);
    }

    template <typename Weight>
    typename DirectedWeightedGraph<Weight>::IncidentEdgesRange
        DirectedWeightedGraph<Weight>::GetIncidentEdges(VertexId vertex) const
    {
        return ranges::AsRange(incidence_lists_.at(vertex));    // Дать интервал для вершины его -> id ребер
    }
}  // namespace graph