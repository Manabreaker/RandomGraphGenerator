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
    const size_t V = graph_.GetVertexCount();
    if (V == 0) return;

    // Рисуем рёбра
    const auto edgePen = CreatePen(PS_SOLID, 2, RGB(0,0,0));
    const auto oldPen = static_cast<HPEN>(SelectObject(hdc, edgePen));

    for (size_t v = 0; v < V; ++v) {
        const auto& edges = graph_[v];
        for (const auto& [to, w] : edges) {
            const int x1 = vertexPositions_[v].pos.x;
            const int y1 = vertexPositions_[v].pos.y;
            const int x2 = vertexPositions_[to].pos.x;
            const int y2 = vertexPositions_[to].pos.y;

            // Учитываем смещение для стрелок (если направленный граф)
            const double angle = atan2(y2 - y1, x2 - x1);
            constexpr double offset = 15.0;
            const int startX = x1 + static_cast<int>(offset * cos(angle));
            const int startY = y1 + static_cast<int>(offset * sin(angle));
            const int endX = x2 - static_cast<int>(offset * cos(angle));
            const int endY = y2 - static_cast<int>(offset * sin(angle));

            MoveToEx(hdc, startX, startY, nullptr);
            LineTo(hdc, endX, endY);

            // Если взвешенный граф, показываем вес ребра
            if (graph_.IsWeighted()) {
                int mx = (startX + endX) / 2;
                int my = (startY + endY) / 2;
                std::wstring weightStr = std::to_wstring(w);

                // Проверка на наличие рёбер в обе стороны
                bool hasBidirectionalEdge = false;
                const auto& reverseEdges = graph_[to];
                for (const auto& reverseEdge : reverseEdges) {
                    if (reverseEdge.first == v) {
                        hasBidirectionalEdge = true;
                        break;
                    }
                }

                // Если есть рёбра в обе стороны, смещаем вес только одного ребра
                if (hasBidirectionalEdge) {
                    // Определяем, в какую сторону смещать
                    int offsetX = 0, offsetY = 0;

                    // Смещаем в противоположную сторону от стрелки
                    if (startX < endX) { // Если направление стрелки из v в to
                        offsetX = -15; // Сместим текст влево (если стрелка направлена вправо)
                    } else {
                        offsetX = 15; // Сместим текст вправо (если стрелка направлена влево)
                    }

                    // Если ребро вертикальное (X1 == X2), смещаем вертикально
                    if (x1 == x2) {
                        if (y1 < y2) {
                            offsetX = -15; // Смещаем вверх
                        } else {
                            offsetX = 15; // Смещаем вниз
                        }
                    }

                    // Если ребро горизонтальное (Y1 == Y2), смещаем вертикально
                    if (y1 - 25 <= y2 and y1 + 25 >= y2) {
                        if (x1 < x2) {
                            offsetY = -15; // Смещаем вверх
                        } else {
                            offsetY = 15; // Смещаем вниз
                        }
                    }
                    // Отображаем вес рёбер со смещением
                    DrawTextCentered(hdc, mx + offsetX, my + offsetY, weightStr);
                } else {
                    // Если рёбер в обе стороны нет, отображаем вес рёбер в стандартном положении
                    DrawTextCentered(hdc, mx, my - 10, weightStr);
                }
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
    const auto vertexBrush = CreateSolidBrush(RGB(255,255,255));
    const auto oldBrush = static_cast<HBRUSH>(SelectObject(hdc, vertexBrush));

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

void GraphVisualizer::DrawArrow(HDC hdc, const int x1, const int y1, const int x2, const int y2) {
    double angle = atan2(y2 - y1, x2 - x1);
    double length = 10.0;
    POINT arrow[3];
    arrow[0].x = x2;
    arrow[0].y = y2;
    arrow[1].x = x2 - static_cast<int>(length * cos(angle - M_PI / 6));
    arrow[1].y = y2 - static_cast<int>(length * sin(angle - M_PI / 6));
    arrow[2].x = x2 - static_cast<int>(length * cos(angle + M_PI / 6));
    arrow[2].y = y2 - static_cast<int>(length * sin(angle + M_PI / 6));

    Polygon(hdc, arrow, 3);
}

void GraphVisualizer::DrawTextCentered(HDC hdc, const int x, const int y, const std::wstring &text) {
    SIZE sz;
    // Используем W-версии функций для работы с wchar_t
    GetTextExtentPoint32W(hdc, text.c_str(), static_cast<int>(text.size()), &sz);
    SetBkMode(hdc, TRANSPARENT);
    TextOutW(hdc, x - sz.cx/2, y - sz.cy/2, text.c_str(), static_cast<int>(text.size()));
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
        selectedVertex_ = static_cast<int>(*hit);
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
    if (dragging_ && selectedVertex_ >= 0 && static_cast<size_t>(selectedVertex_) < vertexPositions_.size()) {
        vertexPositions_[selectedVertex_].pos.x = x - dragOffset_.x;
        vertexPositions_[selectedVertex_].pos.y = y - dragOffset_.y;
    }
}
