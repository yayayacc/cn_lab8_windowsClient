#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include <filesystem>
#include <iostream>
#include <memory>
#include <tchar.h>
#include <thread>

#include "CTP/ThreadPool.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"

#include "app_data.h"
#include "client.h"

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#endif

// https://gcc.gnu.org/onlinedocs/cpp/Stringizing.html
#define XSTR(s) STR(s)
#define STR(s) #s

inline std::wstring AnsiToWString(const std::string& str) {
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return std::wstring(buffer);
}

struct FrameContext {
    ID3D12CommandAllocator* CommandAllocator;
    UINT64                  FenceValue;
};

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT WINAPI MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class GUI {
    GUI() = default;

    enum class STATE : uint8_t {
        NOT_LOG_IN,
        HAS_LOG_IN,
    };

public:
    AppDataPack                       app_data;
    Client                            client;
    std::unique_ptr<Chen::ThreadPool> threadPool;

    bool is_quit{false};

    STATE state{STATE::NOT_LOG_IN};

public:
    static GUI& getInstance() {
        static GUI instance;
        return instance;
    }

    void init();
    void shutdown();

    void drawGUI();
    void loginGUI();
    void commonGUI();

    void drawList();
    void drawInput();
    void drawText();

    void run() {
        WNDCLASSEXW wc = {sizeof(wc), CS_CLASSDC, MainWndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"MainWnd", NULL};
        ::RegisterClassExW(&wc);
        HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"App", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

        if (!CreateDeviceD3D(hwnd)) {
            CleanupDeviceD3D();
            ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
            return;
        }

        ::ShowWindow(hwnd, SW_SHOWDEFAULT);
        ::UpdateWindow(hwnd);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows

        ImGui::StyleColorsLight();

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding              = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT,
                            DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
                            g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
                            g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

        auto font_path =
            std::filesystem::path(XSTR(ROOT_DIR)) / "asset/font/JetBrainsMono-Light.ttf";
        io.Fonts->AddFontFromFileTTF(font_path.string().c_str(), 22.0f);

        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        std::thread readMsg_thread([gui_ptr = this]() {
            while (!gui_ptr->is_quit) {
                if (gui_ptr->state == STATE::HAS_LOG_IN) {
                    auto msg_info = gui_ptr->client.readMsg();
                    gui_ptr->app_data.msg_record[msg_info.account].append(msg_info.account + ":" + msg_info.msg + "\n");
                }
            }
        });

        std::thread recvFile_thread([gui_ptr = this]() {
            while (!gui_ptr->is_quit) {
                if (gui_ptr->state == STATE::HAS_LOG_IN) {
                    gui_ptr->client.recvFile();
                }
            }
        });

        while (!is_quit) {
            std::wstring window_text =
                L"App - " + AnsiToWString(client.myName);
            // SetWindowText(FindWindow(L"MainWnd", NULL), window_text.c_str());

            MSG msg;
            while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                if (msg.message == WM_QUIT)
                    is_quit = true;
            }
            if (is_quit)
                break;

            ImGui_ImplDX12_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            bool m_dockspace_enable = true;

            if (m_dockspace_enable) {
                static bool               dockspaceOpen             = true;
                static bool               opt_fullscreen_persistant = true;
                bool                      opt_fullscreen            = opt_fullscreen_persistant;
                static ImGuiDockNodeFlags dockspace_flags           = ImGuiDockNodeFlags_None;

                ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
                if (opt_fullscreen) {
                    ImGuiViewport* viewport = ImGui::GetMainViewport();
                    ImGui::SetNextWindowPos(viewport->Pos);
                    ImGui::SetNextWindowSize(viewport->Size);
                    ImGui::SetNextWindowViewport(viewport->ID);
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
                }

                if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                    window_flags |= ImGuiWindowFlags_NoBackground;

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
                ImGui::PopStyleVar();

                if (opt_fullscreen)
                    ImGui::PopStyleVar(2);

                // DockSpace
                ImGuiIO& io = ImGui::GetIO();
                if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
                    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
                }

                if (ImGui::BeginMenuBar()) {
                    if (ImGui::BeginMenu("Menu")) {
                        if (ImGui::MenuItem("Exit")) return;
                        ImGui::EndMenu();
                    }

                    ImGui::EndMenuBar();
                }

                // Here
                drawGUI();
            }

            ImGui::End();

            // Rendering
            ImGui::Render();

            FrameContext* frameCtx      = WaitForNextFrameResources();
            UINT          backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
            frameCtx->CommandAllocator->Reset();

            D3D12_RESOURCE_BARRIER barrier = {};
            barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource   = g_mainRenderTargetResource[backBufferIdx];
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
            barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
            g_pd3dCommandList->Reset(frameCtx->CommandAllocator, NULL);
            g_pd3dCommandList->ResourceBarrier(1, &barrier);

            // Render Dear ImGui graphics
            const float clear_color_with_alpha[4] = {clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w};
            g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, NULL);
            g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, NULL);
            g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
            ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
            g_pd3dCommandList->ResourceBarrier(1, &barrier);
            g_pd3dCommandList->Close();

            g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&g_pd3dCommandList);

            // Update and Render additional Platform Windows
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault(NULL, (void*)g_pd3dCommandList);
            }

            g_pSwapChain->Present(1, 0); // Present with vsync
            // g_pSwapChain->Present(0, 0); // Present without vsync

            UINT64 fenceValue = g_fenceLastSignaledValue + 1;
            g_pd3dCommandQueue->Signal(g_fence, fenceValue);
            g_fenceLastSignaledValue = fenceValue;
            frameCtx->FenceValue     = fenceValue;
        }

        WaitForLastSubmittedFrame();

        // Cleanup
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        CleanupDeviceD3D();
        ::DestroyWindow(hwnd);
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

        readMsg_thread.join();
        recvFile_thread.join();
    }

    bool CreateDeviceD3D(HWND hWnd) {
        // Setup swap chain
        DXGI_SWAP_CHAIN_DESC1 sd;
        {
            ZeroMemory(&sd, sizeof(sd));
            sd.BufferCount        = NUM_BACK_BUFFERS;
            sd.Width              = 0;
            sd.Height             = 0;
            sd.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.Flags              = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
            sd.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.SampleDesc.Count   = 1;
            sd.SampleDesc.Quality = 0;
            sd.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            sd.AlphaMode          = DXGI_ALPHA_MODE_UNSPECIFIED;
            sd.Scaling            = DXGI_SCALING_STRETCH;
            sd.Stereo             = FALSE;
        }

        // [DEBUG] Enable debug interface
#ifdef DX12_ENABLE_DEBUG_LAYER
        ID3D12Debug* pdx12Debug = NULL;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
            pdx12Debug->EnableDebugLayer();
#endif

        // Create device
        D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
        if (D3D12CreateDevice(NULL, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK)
            return false;

            // [DEBUG] Setup debug interface to break on any warnings/errors
#ifdef DX12_ENABLE_DEBUG_LAYER
        if (pdx12Debug != NULL) {
            ID3D12InfoQueue* pInfoQueue = NULL;
            g_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
            pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
            pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
            pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
            pInfoQueue->Release();
            pdx12Debug->Release();
        }
#endif

        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            desc.NumDescriptors             = NUM_BACK_BUFFERS;
            desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            desc.NodeMask                   = 1;
            if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
                return false;

            SIZE_T                      rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle         = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
            for (UINT i = 0; i < NUM_BACK_BUFFERS; i++) {
                g_mainRenderTargetDescriptor[i] = rtvHandle;
                rtvHandle.ptr += rtvDescriptorSize;
            }
        }

        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            desc.NumDescriptors             = 1;
            desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
                return false;
        }

        {
            D3D12_COMMAND_QUEUE_DESC desc = {};
            desc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;
            desc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
            desc.NodeMask                 = 1;
            if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
                return false;
        }

        for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
            if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
                return false;

        if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator, NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK || g_pd3dCommandList->Close() != S_OK)
            return false;

        if (g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK)
            return false;

        g_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (g_fenceEvent == NULL)
            return false;

        {
            IDXGIFactory4*   dxgiFactory = NULL;
            IDXGISwapChain1* swapChain1  = NULL;
            if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK)
                return false;
            if (dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, NULL, NULL, &swapChain1) != S_OK)
                return false;
            if (swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
                return false;
            swapChain1->Release();
            dxgiFactory->Release();
            g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
            g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
        }

        CreateRenderTarget();
        return true;
    }

    void CleanupDeviceD3D() {
        CleanupRenderTarget();
        if (g_pSwapChain) {
            g_pSwapChain->SetFullscreenState(false, NULL);
            g_pSwapChain->Release();
            g_pSwapChain = NULL;
        }
        if (g_hSwapChainWaitableObject != NULL) { CloseHandle(g_hSwapChainWaitableObject); }
        for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
            if (g_frameContext[i].CommandAllocator) {
                g_frameContext[i].CommandAllocator->Release();
                g_frameContext[i].CommandAllocator = NULL;
            }
        if (g_pd3dCommandQueue) {
            g_pd3dCommandQueue->Release();
            g_pd3dCommandQueue = NULL;
        }
        if (g_pd3dCommandList) {
            g_pd3dCommandList->Release();
            g_pd3dCommandList = NULL;
        }
        if (g_pd3dRtvDescHeap) {
            g_pd3dRtvDescHeap->Release();
            g_pd3dRtvDescHeap = NULL;
        }
        if (g_pd3dSrvDescHeap) {
            g_pd3dSrvDescHeap->Release();
            g_pd3dSrvDescHeap = NULL;
        }
        if (g_fence) {
            g_fence->Release();
            g_fence = NULL;
        }
        if (g_fenceEvent) {
            CloseHandle(g_fenceEvent);
            g_fenceEvent = NULL;
        }
        if (g_pd3dDevice) {
            g_pd3dDevice->Release();
            g_pd3dDevice = NULL;
        }

#ifdef DX12_ENABLE_DEBUG_LAYER
        IDXGIDebug1* pDebug = NULL;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug)))) {
            pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
            pDebug->Release();
        }
#endif
    }

    void CreateRenderTarget() {
        for (UINT i = 0; i < NUM_BACK_BUFFERS; i++) {
            ID3D12Resource* pBackBuffer = NULL;
            g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
            g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, g_mainRenderTargetDescriptor[i]);
            g_mainRenderTargetResource[i] = pBackBuffer;
        }
    }

    void CleanupRenderTarget() {
        WaitForLastSubmittedFrame();

        for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
            if (g_mainRenderTargetResource[i]) {
                g_mainRenderTargetResource[i]->Release();
                g_mainRenderTargetResource[i] = NULL;
            }
    }

    void WaitForLastSubmittedFrame() {
        FrameContext* frameCtx = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];

        UINT64 fenceValue = frameCtx->FenceValue;
        if (fenceValue == 0)
            return; // No fence was signaled

        frameCtx->FenceValue = 0;
        if (g_fence->GetCompletedValue() >= fenceValue)
            return;

        g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
        WaitForSingleObject(g_fenceEvent, INFINITE);
    }

    FrameContext* WaitForNextFrameResources() {
        UINT nextFrameIndex = g_frameIndex + 1;
        g_frameIndex        = nextFrameIndex;

        HANDLE waitableObjects[]  = {g_hSwapChainWaitableObject, NULL};
        DWORD  numWaitableObjects = 1;

        FrameContext* frameCtx   = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
        UINT64        fenceValue = frameCtx->FenceValue;
        if (fenceValue != 0) // means no fence was signaled
        {
            frameCtx->FenceValue = 0;
            g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
            waitableObjects[1] = g_fenceEvent;
            numWaitableObjects = 2;
        }

        WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

        return frameCtx;
    }

    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg) {
            case WM_SIZE:
                if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
                    WaitForLastSubmittedFrame();
                    CleanupRenderTarget();
                    HRESULT result = g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
                    assert(SUCCEEDED(result) && "Failed to resize swapchain.");
                    CreateRenderTarget();
                }
                return 0;
            case WM_SYSCOMMAND:
                if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                    return 0;
                break;
            case WM_DESTROY:
                ::PostQuitMessage(0);
                return 0;
        }
        return ::DefWindowProc(hWnd, msg, wParam, lParam);
    }

    // Data
    int const    NUM_FRAMES_IN_FLIGHT = 3;
    FrameContext g_frameContext[3]    = {};
    UINT         g_frameIndex         = 0;

    int const                   NUM_BACK_BUFFERS                = 3;
    ID3D12Device*               g_pd3dDevice                    = NULL;
    ID3D12DescriptorHeap*       g_pd3dRtvDescHeap               = NULL;
    ID3D12DescriptorHeap*       g_pd3dSrvDescHeap               = NULL;
    ID3D12CommandQueue*         g_pd3dCommandQueue              = NULL;
    ID3D12GraphicsCommandList*  g_pd3dCommandList               = NULL;
    ID3D12Fence*                g_fence                         = NULL;
    HANDLE                      g_fenceEvent                    = NULL;
    UINT64                      g_fenceLastSignaledValue        = 0;
    IDXGISwapChain3*            g_pSwapChain                    = NULL;
    HANDLE                      g_hSwapChainWaitableObject      = NULL;
    ID3D12Resource*             g_mainRenderTargetResource[3]   = {};
    D3D12_CPU_DESCRIPTOR_HANDLE g_mainRenderTargetDescriptor[3] = {};
};

LRESULT WINAPI MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return GUI::getInstance().WndProc(hWnd, msg, wParam, lParam);
}
