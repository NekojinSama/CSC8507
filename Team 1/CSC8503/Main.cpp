#include "Window.h"
#include "Assets.h"
#include "MainMenu.h"
#include "AssetManager.h"
#include "SoundsManager.h"
#include "SoundSystem.h"
#include "ConfigOptions.h"


using namespace NCL;
using namespace CSC8503;


int main() {
	if (!ConfigOptions::LoadOptions())
		ConfigOptions::LoadDefault();

#pragma region Intialise Window
	
	Window* w = Window::CreateGameWindow("Splat Genius",(ConfigOptions::screenX < 1280 ? 1280 : ConfigOptions::screenX),(ConfigOptions::screenY< 720 ? 720 : ConfigOptions::screenX), ConfigOptions::fullscreen);

	if (!w->HasInitialised()) {
		return -1;
	}

#pragma endregion

#pragma region Intialise SoundSystem

	SoundSystem::Initialise();

	SoundsManager::AddSound((Assets::SOUNDSDIR + "bgmusic.wav").c_str());
	Sounds* bgMusic = SoundsManager::GetSound((Assets::SOUNDSDIR + "bgmusic.wav").c_str());
	SoundSource* bgMus = new SoundSource();
	bgMus->SetSound(bgMusic);
	bgMus->SetLooping(true);
	SoundSystem::GetSoundSystem()->AddSoundEmitter(bgMus);
	SoundSystem::GetSoundSystem()->SetMasterVolume(ConfigOptions::volume);

#pragma endregion

#pragma region Intialise World, Menu, Renderer and Assets

	GameWorld* world = new GameWorld();
	MainMenu* mainMenu = new MainMenu(world);
	GameTechRenderer* renderer = new GameTechRenderer(*world, mainMenu);
	renderer->SetVerticalSync(VerticalSyncState::VSync_OFF);
	mainMenu->renderer = renderer;
	auto startTime = std::chrono::steady_clock::now();
	AssetManager::InitialiseAssets(renderer);
	auto endTime = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed = endTime - startTime;
	

#pragma endregion

	w->GetTimer()->GetTimeDeltaSeconds();

	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE) && !mainMenu->isQuit) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		
		SoundSystem::GetSoundSystem()->Update(dt);

		renderer->Update(dt);

		if (mainMenu->inMenu) {
			renderer->Render(1);
			w->LockMouseToWindow(false);
			w->ShowOSPointer(true);
		}
		else {
			mainMenu->UpdateGame(dt);
			w->LockMouseToWindow(!mainMenu->isPaused);
			w->ShowOSPointer(mainMenu->isPaused);
		}
	}

#pragma region Free memory

	delete renderer;
	delete mainMenu;
	delete world;
	ConfigOptions::screenX = w->GetScreenSize().x;
	ConfigOptions::screenY = w->GetScreenSize().y;
	ConfigOptions::SaveOptions();
	SoundSystem::Destroy();
	Window::DestroyGameWindow();
	AssetManager::FreeAssets();

#pragma endregion
	
}