#ifndef DIRECTED_GRAPH_H
#define DIRECTED_GRAPH_H

#include "Graph.h"

class DirectedGraph : public Graph {
public:
    DirectedGraph(bool weighted);
    ~DirectedGraph() override = default;

    void SetVertexCount(size_t count) override;
    size_t GetVertexCount() const override;

    void AddEdge(size_t from, size_t to, int weight = 1) override;
    void RemoveEdge(size_t from, size_t to) override;
    bool HasEdge(size_t from, size_t to) const override;

    int GetWeight(size_t from, size_t to) const override;
    void SetWeight(size_t from, size_t to, int weight) override;

    const std::unordered_map<size_t,int>& operator[](size_t vertex) const override;

    void GenerateRandom(
        size_t minVertices, size_t maxVertices,
        size_t minEdges, size_t maxEdges,
        int minWeight, int maxWeight,
        bool weighted
    ) override;

    void SetVertexWeights(const std::vector<int>& weights) override;
    int GetVertexWeight(size_t v) const override;
    bool IsWeighted() const override { return weighted_; }

private:
    std::vector<std::unordered_map<size_t,int>> adjacency_;
    std::vector<int> vertexWeights_;
    bool weighted_;
};

#endif // DIRECTED_GRAPH_H
