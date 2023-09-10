#pragma once

namespace NCL {
	namespace CSC8503 {
		class ConfigOptions {
		public:
			static bool LoadOptions();
			static void LoadDefault();
			static void SaveOptions();

			static float volume;
			static int screenX;
			static int screenY;
			static bool fullscreen;

			static float sensitivity;
		};
	}
}