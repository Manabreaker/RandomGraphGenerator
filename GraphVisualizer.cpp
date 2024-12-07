#include "GraphVisualizer.h"
#include <cmath>
#include <algorithm>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GraphVisualizer::GraphVisualizer(Graph& graph, bool directed)
: graph_(graph), directed_(directed)
{}

void GraphVisualizer::LayoutVertices(RECT clientRect) {
    vertexPositions_.clear();
    size_t V = graph_.GetVertexCount();
    if (V == 0) return;

    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;

    double angleStep = (V > 0) ? (2.0 * M_PI / static_cast<double>(V)) : 0.0;
    int radius = (std::min(width, height)) / 3;

    int cx = width / 2;
    int cy = height / 2;

    vertexPositions_.resize(V);
    for (size_t i = 0; i < V; ++i) {
        double angle = i * angleStep;
        POINT p;
        p.x = cx + static_cast<int>(radius * std::cos(angle));
        p.y = cy + static_cast<int>(radius * std::sin(angle));
        vertexPositions_[i].pos = p;
    }
}

void GraphVisualizer::Draw(HDC hdc) {
    size_t V = graph_.GetVertexCount();
    if (V == 0) return;

    // Рисуем рёбра
    HPEN edgePen = CreatePen(PS_SOLID, 2, RGB(0,0,0));
    HPEN oldPen = (HPEN)SelectObject(hdc, edgePen);

    for (size_t v = 0; v < V; ++v) {
        const auto& edges = graph_[v];
        for (const auto& [to, w] : edges) {
            int x1 = vertexPositions_[v].pos.x;
            int y1 = vertexPositions_[v].pos.y;
            int x2 = vertexPositions_[to].pos.x;
            int y2 = vertexPositions_[to].pos.y;

            // Учитываем смещение для стрелок (если направленный)
            double angle = atan2(y2 - y1, x2 - x1);
            double offset = 15.0;
            int startX = x1 + (int)(offset * cos(angle));
            int startY = y1 + (int)(offset * sin(angle));
            int endX = x2 - (int)(offset * cos(angle));
            int endY = y2 - (int)(offset * sin(angle));

            MoveToEx(hdc, startX, startY, NULL);
            LineTo(hdc, endX, endY);

            // Если взвешенный граф, показываем вес ребра
            if (graph_.IsWeighted()) {
                int mx = (startX + endX)/2;
                int my = (startY + endY)/2;
                std::wstring weightStr = std::to_wstring(w);
                DrawTextCentered(hdc, mx, my-10, weightStr);
            }

            // Если граф направленный, рисуем стрелку
            if (directed_) {
                DrawArrow(hdc, startX, startY, endX, endY);
            }
        }
    }

    SelectObject(hdc, oldPen);
    DeleteObject(edgePen);

    // Рисуем вершины
    HBRUSH vertexBrush = CreateSolidBrush(RGB(255,255,255));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, vertexBrush);

    for (size_t v = 0; v < V; ++v) {
        int x = vertexPositions_[v].pos.x;
        int y = vertexPositions_[v].pos.y;
        Ellipse(hdc, x-15, y-15, x+15, y+15);

        // Отображаем номер вершины
        std::wstring vertexText = std::to_wstring(v);
        DrawTextCentered(hdc, x, y, vertexText);

        // Если взвешенный граф, отображаем вес вершины над ней
        if (graph_.IsWeighted()) {
            const int vw = graph_.GetVertexWeight(v);
            std::wstring weightText = std::to_wstring(vw);
            DrawTextCentered(hdc, x, y - 25, weightText);
        }
    }

    SelectObject(hdc, oldBrush);
    DeleteObject(vertexBrush);
}

void GraphVisualizer::DrawArrow(HDC hdc, int x1, int y1, int x2, int y2) {
    double angle = atan2(y2 - y1, x2 - x1);
    double length = 10.0;
    POINT arrow[3];
    arrow[0].x = x2;
    arrow[0].y = y2;
    arrow[1].x = x2 - (int)(length * cos(angle - M_PI / 6));
    arrow[1].y = y2 - (int)(length * sin(angle - M_PI / 6));
    arrow[2].x = x2 - (int)(length * cos(angle + M_PI / 6));
    arrow[2].y = y2 - (int)(length * sin(angle + M_PI / 6));

    Polygon(hdc, arrow, 3);
}

void GraphVisualizer::DrawTextCentered(HDC hdc, int x, int y, const std::wstring &text) {
    SIZE sz;
    // Используем W-версии функций для работы с wchar_t
    GetTextExtentPoint32W(hdc, text.c_str(), (int)text.size(), &sz);
    SetBkMode(hdc, TRANSPARENT);
    TextOutW(hdc, x - sz.cx/2, y - sz.cy/2, text.c_str(), (int)text.size());
}

std::optional<size_t> GraphVisualizer::HitTestVertex(int x, int y) const {
    for (size_t i = 0; i < vertexPositions_.size(); ++i) {
        int vx = vertexPositions_[i].pos.x;
        int vy = vertexPositions_[i].pos.y;
        int dx = x - vx;
        int dy = y - vy;
        if (dx*dx + dy*dy <= 15*15) {
            return i;
        }
    }
    return std::nullopt;
}

void GraphVisualizer::OnLButtonDown(int x, int y) {
    auto hit = HitTestVertex(x, y);
    if (hit.has_value()) {
        selectedVertex_ = (int)*hit;
        dragging_ = true;
        dragOffset_.x = x - vertexPositions_[selectedVertex_].pos.x;
        dragOffset_.y = y - vertexPositions_[selectedVertex_].pos.y;
    }
}

void GraphVisualizer::OnLButtonUp(int x, int y) {
    dragging_ = false;
    selectedVertex_ = -1;
}

void GraphVisualizer::OnMouseMove(int x, int y) {
    if (dragging_ && selectedVertex_ >= 0 && (size_t)selectedVertex_ < vertexPositions_.size()) {
        vertexPositions_[selectedVertex_].pos.x = x - dragOffset_.x;
        vertexPositions_[selectedVertex_].pos.y = y - dragOffset_.y;
    }
}
