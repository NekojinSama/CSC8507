# CSC8507-Team-1
The project was developed as the part of university team project which helped me understanding how a game development team might work. The aim was to create a Splatoon-like multiplayer FPS game with objectives to capture in a small arena map.
In terms of my contribution, I created the Audio system using tools like dr_wav and OpenAL. Another contribution by me is the User Interface for the game.

## Features Implemented
- [CMake](Team%201/CMakeLists.txt) for adding [OpenAL](Team%201/audioLib/AL/al.h), [dr_wav](Team%201/audioLib/dr_lib/dr_wav.h) and [ImGUI](Team%201/CSC8503CoreClasses/imgui/imgui.h)
- Audio
  - [Sound](Team%201/CSC8503/Sounds.cpp)
  - [Sound Buffer](Team%201/CSC8503/SoundBuffer.cpp)
  - [Sound Device](Team%201/CSC8503/SoundDevice.cpp)
  - [Sound Manager](Team%201/CSC8503/SoundsManager.cpp)
  - [Sound Source](Team%201/CSC8503/SoundSource.cpp)
  - [Sound System](Team%201/CSC8503/SoundSystem.cpp)
- [Game UI](Team%201/CSC8503/HUD.cpp)
- [Main Menu UI](Team%201/CSC8503/MainMenu.cpp)

## Project Highlights:
The Audio System used two tools: dr_wav and OpenAL. To implement OpenAL and dr_wav in our project we had to update the CMakeList to properly load in the library and also have the .dll files in the executable folder. We first started off with making a simple audio play in the scene. To do this we made an audio device, context, sources, listener and buffer. The device is for getting the audio playback device, context was made to specify the audio environment it will use, sources were made on the player to generate audio while moving and also the listener was attached to the player. To create an audio buffer we used “dr_wav”, we made a struct which holds the audio information. This was later updated into an audio system to utilise the 32 buffer limit by introducing audio play radius and priority.

The User Interface was created using ImGUI. It was integrated into our game through installing the relevant files from GitHub (imgui_impl_opengl3.cpp etc.) and building them into the CoreClasses project of our game through CMake.

## Future improvements:
The Audio System could've been improved by adding sound triggers, adding global sound. Another thing which could've been added would be streaming audio files which are too large to fit in memory.

As for User Interface, the improvements could've been the properly scaled UI in game. Another improvement can be adding custom textures in Menu.

## Project Showcase:
[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/IPfcGszz82g/0.jpg)](https://www.youtube.com/watch?v=IPfcGszz82g)
