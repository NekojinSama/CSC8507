#include "MainMenu.h"
#include "TutorialGame.h"
#include "NetworkedGame.h"
#include "CoopGame.h"
#include "GameTechRenderer.h"

#include "Assets.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include <Win32Window.h>
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl3.h"

#include <fstream>
#include <vector>




MainMenu::MainMenu(GameWorld* _world) {
	netGame = nullptr;
	singleGame = nullptr;
	coopGame = nullptr;
	inMenu = true;
	isQuit = false;
	world = _world;
	renderer = nullptr;
	isPaused = false;
}

MainMenu::~MainMenu() {
	delete netGame;
	delete singleGame;
	delete coopGame;
}

void MainMenu::StartCoopGame(GameWorld* world, GameTechRenderer* renderer) {
	coopGame = new CoopGame(world, renderer);
	coopGame->StartCoop(2);
	inMenu = false;
}

void MainMenu::StartSinglePlayer(GameWorld* world, GameTechRenderer* renderer) {
	singleGame = new TutorialGame(world, renderer);
	singleGame->StartSolo();
	inMenu = false;
}

void MainMenu::StartServer(GameWorld* world, GameTechRenderer* renderer) {
	netGame = new NetworkedGame(world, renderer);
	netGame->StartAsServer();
	inMenu = false;
}

void MainMenu::StartClient(GameWorld* world, GameTechRenderer* renderer) {
	netGame = new NetworkedGame(world, renderer);
	netGame->StartAsClient(127, 0, 0, 1);
	inMenu = false;
}

void MainMenu::BackToMainMenu() {
	if (coopGame) {
		coopGame->Unpause(true);
		world->RemoveExtraCameras();
		delete coopGame;
		coopGame = nullptr;
	}
	else if (singleGame) {
		singleGame->Unpause(true);
		delete singleGame;
		singleGame = nullptr;
	}
	inMenu = true;
}

void MainMenu::UpdateMainMenu() {
	/*ImGuiIO& io = ImGui::GetIO();
	ImFont* myFont = io.Fonts->AddFontFromFileTTF((Assets::FONTSSDIR + "/Kanit/Kanit-Regular.ttf").c_str(), 1.0f);*/

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 255);
	style.Colors[ImGuiCol_Text] = ImVec4(255, 255, 255, 255);
	ImGui::GetIO().FontGlobalScale = 0.5f;

	ImGui::GetStyle().FrameBorderSize = 0.0f;
	style.Colors[ImGuiCol_Button] = ImVec4(0, 0, 0, 0);

	ImVec2 windowSize = ImVec2(renderer->GetWindowWidth(),renderer->GetWindowHeigth());

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoScrollWithMouse /* | ImGuiWindowFlags_NoBackground*/;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 10.0f));

	static bool showMultiPlayerMenu = false;
	static bool showCredits = false;
	static bool showOptions = false;
	static bool showNotification = false;

	ImGui::Begin("Main Menu", (bool *)0, windowFlags);
	ImGui::SetWindowSize(ImVec2(windowSize.x, 80));
	ImGui::SetWindowPos(ImVec2(0, windowSize.y - 80));

	ImGui::Columns(5, 0, false);

	if (ImGui::Button("Single Player", ImVec2(windowSize.x/5, 60))) {
		StartSinglePlayer(world, renderer);
	}

	ImGui::NextColumn();
	if (ImGui::Button("MultiPlayer", ImVec2(windowSize.x / 5, 60))) {
		showMultiPlayerMenu = !showMultiPlayerMenu;
	}

	ImGui::NextColumn();
	if (ImGui::Button("Options", ImVec2(windowSize.x/5, 60))) {
		showOptions = !showOptions;
	}

	ImGui::NextColumn();
	if (ImGui::Button("Credits ", ImVec2(windowSize.x/5, 60))) {
		showCredits = !showCredits;
	}

	ImGui::NextColumn();
	if (ImGui::Button("Exit", ImVec2(windowSize.x/5, 60))) {
		isQuit = true;
	}
	ImGui::End();

	if (showCredits) {
		ImGui::Begin("Credits", &showCredits, windowFlags);
		ImGui::Text("Jack Berry 190013367");
		ImGui::Text("Ashwin Bhanware 220478278");
		ImGui::Text("Paraic Bradley 180165038");
		ImGui::Text("Matthew Holdsworth 190302995");
		ImGui::Text("Zhaolong Ma 200947242");
		ImGui::Text("Santi Mujica 190380133");
		ImGui::Text("Mark Nicholson 190312879");
		ImGui::Text("Simone Noble 190282930");
			
		ImGui::End();
	}

	if (showOptions) {
		ImGui::Begin("Options", &showOptions, windowFlags);
		ImGui::SetWindowSize(ImVec2((windowSize.x / 2) - 20, 260));
		ImGui::SetWindowPos(ImVec2(windowSize.x/2, (windowSize.y / 2)));

		ImGui::SliderFloat("Master Volume", &NCL::CSC8503::ConfigOptions::volume, 0.0, 1.0);
		SoundSystem::GetSoundSystem()->SetMasterVolume(NCL::CSC8503::ConfigOptions::volume);

		if (ImGui::Selectable("Fullscreen", &NCL::CSC8503::ConfigOptions::fullscreen))
			showNotification = true;
		ImGui::SliderFloat("Sensitivity", &NCL::CSC8503::ConfigOptions::sensitivity, 0.1, 3.0);

		if (ImGui::Button("DefaultOptions"))
			ConfigOptions::LoadDefault();
		ImGui::End();
	}

	if (showMultiPlayerMenu) {
		ImGui::Begin("MultiPlayer Menu", &showMultiPlayerMenu, windowFlags);
		ImGui::SetWindowSize(ImVec2(windowSize.x/5 + 20, 210));
		ImGui::SetWindowPos(ImVec2(windowSize.x/5 - 5, windowSize.y - 290));

		if (ImGui::Button("Server", ImVec2(windowSize.x/5, 60))) {
			StartServer(world, renderer);
		}
		if (ImGui::Button("Client", ImVec2(windowSize.x/5, 60))) {
			StartClient(world, renderer);
		}
		if (ImGui::Button("Local", ImVec2(windowSize.x/5, 60))) {
			StartCoopGame(world, renderer);
		}
		
		ImGui::End();
	}

	if (showNotification) {
		ImGui::Begin("Notification", &showNotification, windowFlags);
		ImGui::SetWindowSize(ImVec2(windowSize.x / 3 , 70));
		ImGui::SetWindowPos(ImVec2(windowSize.x /3, (windowSize.y /3)));

		if (ImGui::Button("This change wont take place until restarted, OK", ImVec2(windowSize.x / 3, 70))) {
			showNotification = false;
		}

		ImGui::End();
	}

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MainMenu::UpdatePauseMenu() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	static bool showOptions = false;
	static bool showNotification = false;

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 255);
	style.Colors[ImGuiCol_Text] = ImVec4(255, 255, 255, 255);
	ImGui::GetIO().FontGlobalScale = 1.0f;

	ImGui::GetStyle().FrameBorderSize = 0.0f;
	style.Colors[ImGuiCol_Button] = ImVec4(0, 0, 0, 0);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 10.0f));

	ImVec2 windowSize = ImVec2(renderer->GetWindowWidth(), renderer->GetWindowHeigth());

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoScrollWithMouse /* | ImGuiWindowFlags_NoBackground*/;

	ImGui::SetNextWindowBgAlpha(0.95);
	
	ImGui::Begin("Pause Menu", 0, windowFlags);
	ImGui::SetWindowSize(ImVec2(windowSize.x/5, windowSize.y));
	ImGui::SetWindowPos(ImVec2(0, 0));

	ImGui::Text("Game Paused");
	ImGui::AlignTextToFramePadding();
	ImGui::GetIO().FontGlobalScale = 0.5f;

	if (ImGui::Button("Resume", ImVec2(windowSize.x/5, windowSize.y/6))) {
		if (singleGame)
			singleGame->Unpause(false);
		else
			coopGame->Unpause(false);
	}
	if (ImGui::Button("Options", ImVec2(windowSize.x / 5, windowSize.y / 6))) {
		showOptions = !showOptions;
	}
	if (ImGui::Button("Restart Level", ImVec2(windowSize.x / 5, windowSize.y / 6))) {
	}
	if (ImGui::Button("Back to Main Menu", ImVec2(windowSize.x / 5, windowSize.y / 6))) {
		BackToMainMenu();
	}
	if (ImGui::Button("Exit", ImVec2(windowSize.x / 5, windowSize.y / 6))) {
		isQuit = true;
	}
	ImGui::End();

	if (showOptions) {
		ImGui::Begin("Options", &showOptions, windowFlags);
		ImGui::SetWindowSize(ImVec2((windowSize.x / 2) - 20, 260));
		ImGui::SetWindowPos(ImVec2(windowSize.x / 2, (windowSize.y / 2)));

		ImGui::SliderFloat("Master Volume", &NCL::CSC8503::ConfigOptions::volume, 0.0, 1.0);
		SoundSystem::GetSoundSystem()->SetMasterVolume(NCL::CSC8503::ConfigOptions::volume);

		if (ImGui::Selectable("Fullscreen", &NCL::CSC8503::ConfigOptions::fullscreen))
			showNotification = true;
		ImGui::SliderFloat("Sensitivity", &NCL::CSC8503::ConfigOptions::sensitivity, 0.1, 3.0);

		if (ImGui::Button("DefaultOptions")) 
			ConfigOptions::LoadDefault();
		

		ImGui::End();
	}

	if (showNotification) {
		ImGui::Begin("Notification", &showNotification, windowFlags);
		ImGui::SetWindowSize(ImVec2(windowSize.x / 3, 70));
		ImGui::SetWindowPos(ImVec2(windowSize.x / 3, (windowSize.y / 3)));

		if (ImGui::Button("This change wont take place until restarted, OK", ImVec2(windowSize.x / 3, 70))) {
			showNotification = false;
		}

		ImGui::End();
	}

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MainMenu::UpdateEndScreen() {
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RETURN)) {
		inMenu = true;
	}
	int winner = 0;
	if (singleGame != nullptr) {
		winner = singleGame->GetWinner();
	}
	else if (coopGame != nullptr) {
		winner = coopGame->GetWinner();
	}
	else if (netGame != nullptr) {
		winner = netGame->GetWinner();
	}
	switch (winner)
	{
	case 0:
		Debug::Print("DRAW", Vector2(35,45));
		break;
	case 1:
		Debug::Print("Red Team Wins", Vector2(35, 45));
		break;
	case 2:
		Debug::Print("Blue Team Wins", Vector2(35, 45));
		break;
	}
}

void MainMenu::UpdateGame(float dt) {
	if (netGame != nullptr) {
		isPaused = false;
		netGame->UpdateGame(dt);
	}
	else if (coopGame != nullptr) {
		isPaused = coopGame->IsPaused();
		coopGame->UpdateGame(dt);
	}
	else if (singleGame != nullptr) {
		isPaused = singleGame->IsPaused();
		singleGame->UpdateGame(dt);
	}
}