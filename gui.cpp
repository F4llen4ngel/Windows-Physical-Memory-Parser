#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Создание окна Win32
    HWND hwnd;
    {
        WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
        RegisterClassEx(&wc);
        hwnd = CreateWindow(wc.lpszClassName, _T("ImGui Example"), WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, NULL, NULL, wc.hInstance, NULL);
    }

    // Создание DirectX 12 устройства и контекста
    ID3D12Device* pDevice;
    ID3D12CommandQueue* pCommandQueue;
    {
        // Создание устройства и командной очереди DirectX 12

        // ...

        // Инициализация ImGui для DirectX 12
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX12_Init(pDevice, 3, DXGI_FORMAT_R8G8B8A8_UNORM, NULL, NULL);
    }

    // Запуск главного цикла приложения
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        // Начало нового кадра ImGui
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Создание меню
        ImGui::Begin("Меню");

        static char file_path[256] = ""; // Переменная для хранения пути к файлу

        ImGui::InputText("Путь к файлу", file_path, sizeof(file_path));

        if (ImGui::Button("Анализ"))
        {
            // Здесь можно выполнить код для обработки анализа файла
        }

        ImGui::End();

        // Отрисовка кадра ImGui
        ImGui::Render();
        ID3D12CommandAllocator* pCommandAllocator;
        ID3D12GraphicsCommandList* pCommandList;
        pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandAllocator));
        pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator, NULL, IID_PPV_ARGS(&pCommandList));
        pCommandList->Reset(pCommandAllocator, NULL);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pCommandList);
        pCommandList->Close();

        //
          // Выполнение команд и отображение рендеринга
        ID3D12CommandList* ppCommandLists[] = { pCommandList };
        pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

        // Здесь также можно выполнить другие команды DirectX 12, если необходимо

        // Переключение буферов
        pCommandQueue->Signal(pFence, pFenceValue);
        pSwapChain->Present(1, 0); // 1 означает использование вертикальной синхронизации (VSync)

        // Ожидание завершения отображения кадра
        const UINT64 fence = pFenceValue;
        pCommandQueue->Signal(pFence, fence);
        pFenceValue++;

        if (pFence->GetCompletedValue() < fence)
        {
            pFence->SetEventOnCompletion(fence, fenceEvent);
            WaitForSingleObject(fenceEvent, INFINITE);
        }
    }

    // Очистка ресурсов ImGui и DirectX 12
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // Освобождение ресурсов DirectX 12
    // ...

    // Удаление окна Win32
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            // Обновление размеров буфера отрисовки DirectX 12
            // ...
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Отключение ALT-клавиши
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
