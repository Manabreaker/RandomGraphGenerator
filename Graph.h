#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <vector>
#include <iostream>

class Graph {
public:
    virtual ~Graph() = default;

    virtual void SetVertexCount(size_t count) = 0;
    [[nodiscard]] virtual size_t GetVertexCount() const = 0;

    virtual void AddEdge(size_t from, size_t to, int weight = 1) = 0;
    virtual void RemoveEdge(size_t from, size_t to) = 0;
    [[nodiscard]] virtual bool HasEdge(size_t from, size_t to) const = 0;

    [[nodiscard]] virtual int GetWeight(size_t from, size_t to) const = 0;
    virtual void SetWeight(size_t from, size_t to, int weight) = 0;

    virtual const std::unordered_map<size_t, int>& operator[](size_t vertex) const = 0;

    virtual void GenerateRandom(
        size_t minVertices, size_t maxVertices,
        size_t minEdges, size_t maxEdges,
        int minWeight, int maxWeight,
        bool weighted
    ) = 0;

    // Можно добавить веса вершин, если необходимо:
    virtual void SetVertexWeights(const std::vector<int>& weights) = 0;
    [[nodiscard]] virtual int GetVertexWeight(size_t v) const = 0;
    [[nodiscard]] virtual bool IsWeighted() const = 0;
};

#endif // GRAPH_H
