#ifndef GRAPH_VISUALIZER_H
#define GRAPH_VISUALIZER_H

#include <Windows.h>
#include "Graph.h"
#include <vector>
#include <optional>

struct VertexPosition {
    POINT pos;
};

class GraphVisualizer {
public:
    GraphVisualizer(Graph& graph, bool directed);
    ~GraphVisualizer() = default;

    void LayoutVertices(RECT clientRect);
    void Draw(HDC hdc);

    // Обработка мыши
    void OnLButtonDown(int x, int y);
    void OnLButtonUp(int x, int y);
    void OnMouseMove(int x, int y);

    [[nodiscard]] std::optional<size_t> HitTestVertex(int x, int y) const;

    void SetDirected(bool directed) { directed_ = directed; }

private:
    Graph& graph_;
    std::vector<VertexPosition> vertexPositions_;
    bool directed_;

    int selectedVertex_ = -1;
    bool dragging_ = false;
    POINT dragOffset_;

    void DrawArrow(HDC hdc, int x1, int y1, int x2, int y2);

    static void DrawTextCentered(HDC hdc, int x, int y, const std::wstring &text);
};

#endif // GRAPH_VISUALIZER_H
