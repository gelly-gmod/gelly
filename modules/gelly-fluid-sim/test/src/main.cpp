#include <GellyD3D.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include <d3d11.h>
#include <windows.h>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_sdl2.h"

SDL_Window *g_Window = nullptr;
int g_Width = 1366;
int g_Height = 768;
ID3D11Device *g_Device = nullptr;
ID3D11DeviceContext *g_Context = nullptr;
IDXGISwapChain *g_SwapChain = nullptr;
ID3D11RenderTargetView *g_BackBufferRTV = nullptr;

void EnsureWindowInitialized() {
	if (g_Window == nullptr) {
		g_Window = SDL_CreateWindow(
			"Gelly Fluid Sim Test",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			g_Width,
			g_Height,
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
		);
	}
}

void EnsureD3D11() {
	if (g_Device != nullptr) {
		return;
	}

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(g_Window, &info);

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = g_Width;
	swapChainDesc.BufferDesc.Height = g_Height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = info.info.win.window;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;

	DX("Failed to create the D3D11 device or swapchain",
	   D3D11CreateDeviceAndSwapChain(
		   nullptr,
		   D3D_DRIVER_TYPE_HARDWARE,
		   nullptr,
		   0,
		   &featureLevel,
		   1,
		   D3D11_SDK_VERSION,
		   &swapChainDesc,
		   &g_SwapChain,
		   &g_Device,
		   nullptr,
		   &g_Context
	   ));

	ID3D11Texture2D *backBuffer;
	DX("Failed to get the back buffer",
	   g_SwapChain->GetBuffer(
		   0, __uuidof(ID3D11Texture2D), (void **)&backBuffer
	   ));

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));

	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	DX("Failed to create the render target view",
	   g_Device->CreateRenderTargetView(backBuffer, &rtvDesc, &g_BackBufferRTV)
	);
}

void EnsureIMGUI() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplSDL2_InitForD3D(g_Window);
	ImGui_ImplDX11_Init(g_Device, g_Context);

	ImGui::StyleColorsDark();
}

void EnsureInitialized() {
	EnsureWindowInitialized();
	EnsureD3D11();
	EnsureIMGUI();
}

void Shutdown() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	if (g_BackBufferRTV) {
		g_BackBufferRTV->Release();
		g_BackBufferRTV = nullptr;
	}

	if (g_SwapChain) {
		g_SwapChain->Release();
		g_SwapChain = nullptr;
	}

	if (g_Context) {
		g_Context->Release();
		g_Context = nullptr;
	}

	if (g_Device) {
		g_Device->Release();
		g_Device = nullptr;
	}

	if (g_Window) {
		SDL_DestroyWindow(g_Window);
		g_Window = nullptr;
	}
}

void RenderFrame() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplSDL2_NewFrame(g_Window);
	ImGui::NewFrame();

	ImGui::Begin("Hello, world!");
	ImGui::End();

	ImGui::Render();
	float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	g_Context->OMSetRenderTargets(1, &g_BackBufferRTV, nullptr);
	g_Context->ClearRenderTargetView(g_BackBufferRTV, clearColor);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	g_SwapChain->Present(1, 0);
}

int main() {
	EnsureInitialized();

	bool quit = false;
	while (!quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT) {
				quit = true;
			}
		}

		RenderFrame();
	}

	Shutdown();

	return 0;
}
