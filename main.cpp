#define UNICODE
#include <windows.h>
#include <windowsx.h>
#include <memory> // для std::unique_ptr
#include "DirectedGraph.h"
#include "UndirectedGraph.h"
#include "GraphVisualizer.h"
#include "Resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ParamDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

static DirectedGraph g_directedGraph(true);
static UndirectedGraph g_undirectedGraph(true);
static Graph* g_currentGraph = &g_directedGraph;
static std::unique_ptr<GraphVisualizer> g_visualizer; // используем умный указатель вместо raw

struct GraphParams {
    size_t minV = 5, maxV = 10;
    size_t minE = 5, maxE = 20;
    bool directed = true;
    bool weighted = true;
} g_params;

HINSTANCE g_hInst = nullptr;
HWND g_hWnd = nullptr;

void GenerateNewGraph(HWND hWnd) {
    // Переключаемся на нужный тип графа
    g_currentGraph = g_params.directed ?
        static_cast<Graph *>(&g_directedGraph) : static_cast<Graph *>(&g_undirectedGraph);

    g_currentGraph->GenerateRandom(
        g_params.minV, g_params.maxV,
        g_params.minE, g_params.maxE,
        1, 10,
        g_params.weighted
    );

    // Каждый раз пересоздаём GraphVisualizer
    g_visualizer = std::make_unique<GraphVisualizer>(*g_currentGraph, g_params.directed);

    RECT rc;
    GetClientRect(hWnd, &rc);
    g_visualizer->LayoutVertices(rc);

    InvalidateRect(hWnd, nullptr, TRUE);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    g_hInst = hInstance;
    constexpr TCHAR szAppName[] = TEXT("RandomGraphApp");

    WNDCLASS wc = {0};
    wc.style = CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(nullptr,IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr,IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    wc.lpszClassName = szAppName;

    if(!RegisterClass(&wc)) {
        MessageBox(nullptr,TEXT("This program requires Windows NT!"), szAppName,MB_ICONERROR);
        return 0;
    }

    g_hWnd = CreateWindow(szAppName, TEXT("Random Graph Generator"),
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          800, 600,
                          nullptr, nullptr, hInstance,nullptr);

    HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDM_MAIN_MENU));
    SetMenu(g_hWnd, hMenu);

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    // Изначально генерируем граф
    g_currentGraph = g_params.directed ?
        static_cast<Graph *>(&g_directedGraph) : static_cast<Graph *>(&g_undirectedGraph);
    g_currentGraph->GenerateRandom(
        g_params.minV, g_params.maxV,
        g_params.minE, g_params.maxE,
        1, 10,
        g_params.weighted
    );

    g_visualizer = std::make_unique<GraphVisualizer>(*g_currentGraph, g_params.directed);
    {
        RECT rc;
        GetClientRect(g_hWnd, &rc);
        g_visualizer->LayoutVertices(rc);
    }

    MSG msg;
    while(GetMessage(&msg, nullptr,0,0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return static_cast<int>(msg.wParam);
}

INT_PTR CALLBACK ParamDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message) {
    case WM_INITDIALOG:
        SetDlgItemInt(hDlg, IDC_EDIT_MINV, static_cast<UINT>(g_params.minV), FALSE);
        SetDlgItemInt(hDlg, IDC_EDIT_MAXV, static_cast<UINT>(g_params.maxV), FALSE);
        SetDlgItemInt(hDlg, IDC_EDIT_MINE, static_cast<UINT>(g_params.minE), FALSE);
        SetDlgItemInt(hDlg, IDC_EDIT_MAXE, static_cast<UINT>(g_params.maxE), FALSE);
        CheckDlgButton(hDlg, IDC_CHECK_DIRECTED, g_params.directed ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(hDlg, IDC_CHECK_WEIGHTED, g_params.weighted ? BST_CHECKED : BST_UNCHECKED);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        switch(LOWORD(wParam)) {
        case IDC_BUTTON_OK:
        {
            BOOL success;
            g_params.minV = GetDlgItemInt(hDlg, IDC_EDIT_MINV, &success, FALSE);
            g_params.maxV = GetDlgItemInt(hDlg, IDC_EDIT_MAXV, &success, FALSE);
            g_params.minE = GetDlgItemInt(hDlg, IDC_EDIT_MINE, &success, FALSE);
            g_params.maxE = GetDlgItemInt(hDlg, IDC_EDIT_MAXE, &success, FALSE);
            g_params.directed = (IsDlgButtonChecked(hDlg, IDC_CHECK_DIRECTED) == BST_CHECKED);
            g_params.weighted = (IsDlgButtonChecked(hDlg, IDC_CHECK_WEIGHTED) == BST_CHECKED);

            // Можно также добавить проверку корректных значений minV <= maxV, minE <= maxE, но мы уже делаем swap в GenerateRandom
            EndDialog(hDlg, IDOK);
        }
        return (INT_PTR)TRUE;
        case IDC_BUTTON_CANCEL:
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
    default: break;
        }
        break;
    default: break;
    }
    return (INT_PTR)FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message) {
    case WM_CREATE:
        return 0;

    case WM_SIZE:
        if(g_visualizer) {
            RECT rc;
            GetClientRect(hWnd, &rc);
            g_visualizer->LayoutVertices(rc);
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        return 0;

    case WM_COMMAND:
        switch(LOWORD(wParam)) {
        case IDM_GENERATE_DIALOG:
            if (DialogBox(g_hInst, MAKEINTRESOURCE(IDD_PARAM_DIALOG), hWnd, ParamDlgProc) == IDOK) {
                // Параметры обновились, сгенерируем граф заново
                GenerateNewGraph(hWnd);
            }
            break;
        case IDM_GENERATE_GRAPH:
            GenerateNewGraph(hWnd);
            break;
        case IDM_EXIT:
            PostMessage(hWnd, WM_CLOSE, 0,0);
            break;
        default: break;
        }
        return 0;

    case WM_LBUTTONDOWN:
        if (g_visualizer) {
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);
            g_visualizer->OnLButtonDown(x, y);
            SetCapture(hWnd);
        }
        return 0;

    case WM_LBUTTONUP:
        if (g_visualizer) {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            g_visualizer->OnLButtonUp(x, y);
            ReleaseCapture();
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        return 0;

    case WM_MOUSEMOVE:
        if (g_visualizer && (wParam & MK_LBUTTON)) {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            g_visualizer->OnMouseMove(x, y);
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd,&ps);
        if(g_visualizer) {
            g_visualizer->Draw(hdc);
        }
        EndPaint(hWnd,&ps);
    }
    return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default: break;
    }
    return DefWindowProc(hWnd,message,wParam,lParam);
}
