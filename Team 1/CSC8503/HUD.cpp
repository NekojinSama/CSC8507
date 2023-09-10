#include "HUD.h"
#include "MainMenu.h"
#include "GameTechRenderer.h"

#include "TutorialGame.h"
#include "CoopGame.h"


#include "imgui/imgui.h"
#include <Win32Window.h>
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl3.h"
#include <Debug.cpp>


using namespace std;
using namespace NCL;
using namespace CSC8503;

HUD::HUD(TutorialGame* g) {
	game = g;
	renderer = NULL;
}

void HUD::GameHUD(GameTechRenderer* r) {
	if (game->GetLocalPlayer() != nullptr && game->GetLocalPlayer()->IsBlind()) {
		renderer->RenderBlindingSplat();
	}
	
	renderer = r;
	float timer = game->GetTimer();
	ImGuiIO& io = ImGui::GetIO();
	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImVec2 windowSize = ImVec2(renderer->GetWindowWidth(), renderer->GetWindowHeigth());

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoScrollWithMouse /* | ImGuiWindowFlags_NoBackground*/;

	ImGui::SetNextWindowBgAlpha(0.5);
	ImGui::GetIO().FontGlobalScale = 1.0f;
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 255);
	ImGui::Begin("Timer HUD", 0, windowFlags);
	ImGui::SetWindowSize(ImVec2(200, 80));
	ImGui::SetWindowPos(ImVec2(windowSize.x/2-100, 0));

	std::string Timer = std::format("{:0>2}", abs(int(timer / 60))) + ':' + std::format("{:0>2.0f}", abs(timer - (60 * int(timer / 60))));
	auto windowWidth = ImGui::GetWindowSize().x;
	auto windowHeight = ImGui::GetWindowSize().y;
	auto textWidth = ImGui::CalcTextSize(Timer.c_str()).x;

	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::SetCursorPosY(10);
	ImGui::Text(Timer.c_str());
	ImGui::End();

	ImGui::SetNextWindowBgAlpha(0.5);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(255, 0, 0, 255);
	ImGui::Begin("Team A HUD", 0, windowFlags);
	ImGui::SetWindowSize(ImVec2(80, 70));
	ImGui::SetWindowPos(ImVec2(windowSize.x / 2 - 180, 0));
	std::string TeamA = std::to_string(game->GetScore(0));
	auto windowWidthA = ImGui::GetWindowSize().x;
	auto windowHeightA = ImGui::GetWindowSize().y;
	auto textWidthA = ImGui::CalcTextSize(TeamA.c_str()).x;

	ImGui::SetCursorPosX((windowWidthA - textWidthA) * 0.5f);
	ImGui::SetCursorPosY(5);
	ImGui::Text(TeamA.c_str());
	ImGui::End();

	ImGui::SetNextWindowBgAlpha(0.5);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 255, 255);
	ImGui::Begin("Team B HUD", 0, windowFlags);
	ImGui::SetWindowSize(ImVec2(80, 70));
	ImGui::SetWindowPos(ImVec2(windowSize.x/2 + 100, 0));
	std::string TeamB = std::to_string(game->GetScore(1));
	auto windowWidthB = ImGui::GetWindowSize().x;
	auto windowHeightB = ImGui::GetWindowSize().y;
	auto textWidthB = ImGui::CalcTextSize(TeamB.c_str()).x;

	ImGui::SetCursorPosX((windowWidthB - textWidthB) * 0.5f);
	ImGui::SetCursorPosY(5);
	ImGui::Text(TeamB.c_str());
	ImGui::End();

	/*ImGui::SetNextWindowBgAlpha(1.0);
	ImGui::Begin("Crosshair", 0, windowFlags);
	ImGui::SetWindowSize(ImVec2(10, 10));
	ImGui::SetWindowPos(ImVec2(x/2, y/2));
	ImGui::Text("+");
	ImGui::End();*/

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	Debug::Print("+", Vector2(50, 50), Debug::CYAN);
}

void NCL::CSC8503::HUD::WeaponHUD(GameTechRenderer* r, Player* p)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	renderer = r;
	ImGui::SetNextWindowBgAlpha(1);
	ImGuiWindowFlags hudFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground;

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 255, 255);
	ImVec2 windowSize = ImVec2(renderer->GetWindowWidth(), renderer->GetWindowHeigth());

	ImVec4 color = ImVec4(255, 0, 0, 255);
	std::string buf;
	static float progress = 0.0f, progress_dir = 1.0f;

	ImGui::Begin("Weapon HUD", 0, hudFlags);
	if (game->GetLocalPlayer() != nullptr) {
		Values PlayerInfo =	GetPlayerInfo(game->GetLocalPlayer());
		ImGui::SetWindowSize(ImVec2(windowSize.x / 2, windowSize.y / 8));
		ImGui::SetWindowPos(ImVec2(5, windowSize.y - 125));
		ImGui::SetWindowPos(ImVec2(5, windowSize.y - 125));

		ImGui::Columns(2, 0, false);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Image(((OGLTexture*)renderer->getHUDtex()), ImVec2(100, 100), ImVec2(0, 0), ImVec2(1, 1), PlayerInfo.color);

		ImGui::NextColumn();
		ImGui::SetColumnWidth(1, 270.0f);

		progress = PlayerInfo.progress;
		progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
		if (progress >= +1.0f) {
			progress = +1.1f; progress_dir *= -1.0f;
		}
		if (progress <= -0.1f) {
			progress = -0.1f; progress_dir *= -1.0f;
		}

		ImGui::SetCursorPosY(50);
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, PlayerInfo.color);
		ImGui::SetWindowFontScale(0.3);
		ImGui::ProgressBar(progress * 10, ImVec2((windowSize.x / 15) * 3, 20), PlayerInfo.buf.c_str());
		ImGui::PopStyleColor();
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void NCL::CSC8503::HUD::CoopGameWeaponHUD(GameTechRenderer* r)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	renderer = r;
	ImGui::SetNextWindowBgAlpha(1);
	ImGuiWindowFlags hudFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground;

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 255, 255);
	ImVec2 windowSize = ImVec2(renderer->GetWindowWidth(), renderer->GetWindowHeigth());

	ImVec4 color = ImVec4(255, 0, 0, 255);
	std::string buf;
	static float progress = 0.0f, progress1 = 0.0f, progress_dir = 1.0f, progress_dir1 = 1.0f;
	vector<Values> PlayerInfo;
	int playerNumber = 0;

	if (getplayerList().size() >> 0) {
		for (auto& i : getplayerList()) {
			PlayerInfo.push_back(GetPlayerInfo(i));
		}

		ImGui::Begin("Weapon HUD Player1", 0, hudFlags);
		ImGui::SetWindowSize(ImVec2(windowSize.x / 2, windowSize.y / 8));
		ImGui::SetWindowPos(ImVec2(5, windowSize.y - 125));
		ImGui::SetWindowPos(ImVec2(5, windowSize.y / 2 - 125));
		ImGui::Image(((OGLTexture*)renderer->getHUDtex()), ImVec2(100, 100), ImVec2(0, 0), ImVec2(1, 1), PlayerInfo[1].color);
		ImGui::Columns(2, 0, false);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Image(((OGLTexture*)renderer->getHUDtex()), ImVec2(100, 100), ImVec2(0, 0), ImVec2(1, 1), PlayerInfo[1].color);

		ImGui::NextColumn();
		ImGui::SetColumnWidth(1, 270.0f);

		progress1 = PlayerInfo[1].progress;
		progress1 += progress_dir1 * 0.4f * ImGui::GetIO().DeltaTime;
		if (progress1 >= +1.0f) {
			progress1 = +1.1f; progress_dir1 *= -1.0f;
		}
		if (progress1 <= -0.1f) {
			progress1 = -0.1f; progress_dir1 *= -1.0f;
		}

		ImGui::SetCursorPosY(50);
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, PlayerInfo[1].color);
		ImGui::SetWindowFontScale(0.3);
		ImGui::ProgressBar(progress1 * 10, ImVec2((windowSize.x / 15) * 3, 20), PlayerInfo[1].buf.c_str());
		ImGui::PopStyleColor();
		ImGui::End();

		ImGui::Begin("Weapon HUD Player2", 0, hudFlags);
		ImGui::SetWindowSize(ImVec2(windowSize.x / 2, windowSize.y / 8));
		ImGui::SetWindowPos(ImVec2(5, windowSize.y - 125));
		ImGui::SetWindowPos(ImVec2(5, windowSize.y - 125));
		ImGui::Image(((OGLTexture*)renderer->getHUDtex()), ImVec2(100, 100), ImVec2(0, 0), ImVec2(1, 1), PlayerInfo[0].color);
		ImGui::Columns(2, 0, false);
		ImGui::SetColumnWidth(0, 100.0f);
		ImGui::Image(((OGLTexture*)renderer->getHUDtex()), ImVec2(100, 100), ImVec2(0, 0), ImVec2(1, 1), PlayerInfo[0].color);

		ImGui::NextColumn();
		ImGui::SetColumnWidth(1, 270.0f);

		progress = PlayerInfo[0].progress;
		progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
		if (progress >= +1.0f) {
			progress = +1.1f; progress_dir *= -1.0f;
		}
		if (progress <= -0.1f) {
			progress = -0.1f; progress_dir *= -1.0f;
		}

		ImGui::SetCursorPosY(50);
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, PlayerInfo[0].color);
		ImGui::SetWindowFontScale(0.3);
		ImGui::ProgressBar(progress * 10, ImVec2((windowSize.x / 15) * 3, 20), PlayerInfo[0].buf.c_str());
		ImGui::PopStyleColor();
		ImGui::End();
	}
	
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void HUD::DebugHUD(GameTechRenderer* r) {
	renderer = r;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImVec2 windowSize = ImVec2(renderer->GetWindowWidth(), renderer->GetWindowHeigth());

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoScrollWithMouse /* | ImGuiWindowFlags_NoBackground*/;
	ImGui::SetNextWindowBgAlpha(0.5);
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 255);

	ImGui::Begin("Debug", 0, windowFlags);
	ImGui::SetWindowSize(ImVec2(windowSize.x/4, 240));
	ImGui::SetWindowPos(ImVec2((2*windowSize.x)/3 + 100, 20));
	std::string FPS = "FPS: " + std::to_string(1 / game->averageFrameRate);
	std::string PhyMemUse = "Physical Memory Usage: " + std::to_string(game->RAMUsage / 1000000.0f) + " mb";
	std::string VirMemUse = "Virtual Memory Usage: " + std::to_string(game->VirtualMemUsage / 1000000.0f) + " mb";
	std::string CPUusage = "CPU Usage: " + std::to_string(game->CPUUsage) + "%";
	std::string lastSplat = "Last paint splat took " + std::to_string(game->applyPaintTimer * 1000) + " ms";
	std::string detectTime = "Last paint splat took " + std::to_string(game->applyPaintTimer * 1000) + " ms";
	std::string assetLoadTime = "It took " + std::to_string(game->assetsTimer * 1000) + " ms to initialise assets";
	std::string phyLoadTime = "It takes " + std::to_string(game->physicsTimer * 1000) + " ms to render physics";
	
	ImGui::SetWindowFontScale(0.4);
	ImGui::Text(FPS.c_str());
	ImGui::Text(PhyMemUse.c_str());
	ImGui::Text(VirMemUse.c_str());
	ImGui::Text(CPUusage.c_str());
	ImGui::Text(lastSplat.c_str());
	ImGui::Text(detectTime.c_str());
	ImGui::Text(assetLoadTime.c_str());
	ImGui::Text(phyLoadTime.c_str());
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	/*Debug::Print("FPS: " + std::to_string(1 / game->averageFrameRate), Vector2(1, 10), Vector4(0,0,0,1));
	Debug::Print("Physical Memory Usage: " + std::to_string(game->RAMUsage/1000000.0f) + " mb", Vector2(1, 20), Vector4(0, 0, 0, 1));
	Debug::Print("Virtual Memory Usage: " + std::to_string(game->VirtualMemUsage/1000000.0f) + " mb", Vector2(1, 30), Vector4(0, 0, 0, 1));
	Debug::Print("CPU Usage: " + std::to_string(game->CPUUsage) + "%", Vector2(1, 40), Vector4(0, 0, 0, 1));
	Debug::Print("Last paint splat took " + std::to_string(game->applyPaintTimer * 1000) + " ms", Vector2(1, 50), Vector4(0, 0, 0, 1));
	Debug::Print("Paint detection took " + std::to_string(game->detectPaintTimer * 1000) + " ms", Vector2(1, 60), Vector4(0, 0, 0, 1));
	Debug::Print("It took " + std::to_string(game->assetsTimer * 1000) + " ms to initialise assets", Vector2(1, 70), Vector4(0, 0, 0, 1));
	Debug::Print("It takes " + std::to_string(game->physicsTimer * 1000) + " ms to render physics", Vector2(1, 80), Vector4(0, 0, 0, 1));*/
}

Values NCL::CSC8503::HUD::GetPlayerInfo(const Player* p)
{
	Values v;
	switch (p->GetCurrentItem()) {
	case 1:
		v.color = ImVec4(255, 0, 0, 255);
		v.buf = "Speed";
		break;
	case 2:
		v.color = ImVec4(0, 0, 255, 255);
		v.buf = "Slow";
		break;
	case 3:
		v.color = ImVec4(0, 255, 0, 255);
		v.buf = "Bounce";
		break;
	case 4:
		v.color = ImVec4(255, 0, 255, 255);
		v.buf = "Confusion";
		break;
	case 5:
		v.color = ImVec4(0, 255, 255, 255);
		v.buf = "Splat";
		break;
	}
	v.progress = (p->GetPaintTimer()) / 100.0f;
	return v;
}
