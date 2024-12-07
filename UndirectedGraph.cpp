#include "UndirectedGraph.h"
#include <stdexcept>
#include <random>

UndirectedGraph::UndirectedGraph(bool weighted)
: weighted_(weighted)
{}

void UndirectedGraph::SetVertexCount(size_t count) {
    adjacency_.clear();
    adjacency_.resize(count);
    vertexWeights_.resize(count, 1);
}

size_t UndirectedGraph::GetVertexCount() const {
    return adjacency_.size();
}

void UndirectedGraph::AddEdge(size_t from, size_t to, int weight) {
    if (from >= GetVertexCount() || to >= GetVertexCount())
        throw std::out_of_range("Vertex index out of range");
    adjacency_[from][to] = weight;
    adjacency_[to][from] = weight;
}

void UndirectedGraph::RemoveEdge(size_t from, size_t to) {
    if (from >= GetVertexCount() || to >= GetVertexCount())
        return;
    adjacency_[from].erase(to);
    adjacency_[to].erase(from);
}

bool UndirectedGraph::HasEdge(size_t from, size_t to) const {
    if (from >= GetVertexCount() || to >= GetVertexCount())
        return false;
    return adjacency_[from].find(to) != adjacency_[from].end();
}

int UndirectedGraph::GetWeight(size_t from, size_t to) const {
    if (!HasEdge(from, to))
        throw std::runtime_error("Edge does not exist");
    return adjacency_[from].at(to);
}

void UndirectedGraph::SetWeight(size_t from, size_t to, int weight) {
    if (!HasEdge(from, to))
        throw std::runtime_error("Edge does not exist");
    adjacency_[from][to] = weight;
    adjacency_[to][from] = weight;
}

const std::unordered_map<size_t,int>& UndirectedGraph::operator[](size_t vertex) const {
    if (vertex >= GetVertexCount())
        throw std::out_of_range("Vertex index out of range");
    return adjacency_[vertex];
}

void UndirectedGraph::GenerateRandom(
    size_t minVertices, size_t maxVertices,
    size_t minEdges, size_t maxEdges,
    int minWeight, int maxWeight,
    bool weighted
) {
    // Корректируем диапазоны
    if (minVertices > maxVertices) std::swap(minVertices, maxVertices);
    if (minEdges > maxEdges) std::swap(minEdges, maxEdges);

    weighted_ = weighted;
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<size_t> distV(minVertices, maxVertices);
    size_t vertexCount = distV(gen);

    SetVertexCount(vertexCount);

    // Если нет или одна вершина - не генерируем рёбра
    if (vertexCount < 2) {
        return;
    }

    std::uniform_int_distribution<size_t> distE(minEdges, maxEdges);
    size_t edgeCount = distE(gen);

    std::uniform_int_distribution<size_t> distVertex(0, vertexCount - 1);
    std::uniform_int_distribution<int> distW(minWeight, maxWeight);

    // Генерация весов вершин
    if (weighted_ && vertexCount > 0) {
        for (auto &w : vertexWeights_) {
            w = distW(gen);
        }
    }

    size_t added = 0;
    while (added < edgeCount) {
        size_t from = distVertex(gen);
        size_t to = distVertex(gen);
        // Избегаем петель
        if (from == to && vertexCount > 1) {
            to = (to + 1) % vertexCount;
        }

        // Проверяем, есть ли уже ребро
        if (!HasEdge(from, to)) {
            int w = weighted_ ? distW(gen) : 1;
            AddEdge(from, to, w);
            added++;
        }
    }
}

void UndirectedGraph::SetVertexWeights(const std::vector<int>& weights) {
    if (weights.size() != vertexWeights_.size()) return;
    vertexWeights_ = weights;
}

int UndirectedGraph::GetVertexWeight(size_t v) const {
    if (v >= GetVertexCount()) throw std::out_of_range("Vertex index out of range");
    return vertexWeights_[v];
}