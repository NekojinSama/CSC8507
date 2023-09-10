#include "PlayStation4Renderer.h"
#include "GameObject.h"
#include "RenderObject.h"
#include "Camera.h"
#include "TextureLoader.h"

#include "imgui/imgui.h"
#include <Win32Window.h>
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl3.h"

#include "MainMenu.h"

using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

#define SHADOWSIZE 4096

Matrix4 biasMatrix = Matrix4::Translation(Vector3(0.5f, 0.5f, 0.5f)) * Matrix4::Scale(Vector3(0.5f, 0.5f, 0.5f));

PlayStation4Renderer::PlayStation4Renderer(GameWorld& world, MainMenu* menu) : PS4RendererBase(*Window::GetWindow()), gameWorld(world) {

}

PlayStation4Renderer::~PlayStation4Renderer()	{
}

void PlayStation4Renderer::LoadSkybox() {
}

void PlayStation4Renderer::RenderFrame(int type) {

}

void PlayStation4Renderer::BuildObjectList() {

}

void PlayStation4Renderer::SortObjectList() {

}

void PlayStation4Renderer::RenderShadowMap() {

}

void PlayStation4Renderer::RenderSkybox() {

}

void PlayStation4Renderer::RenderGame() {
	BuildObjectList();
	SortObjectList();
	RenderShadowMap();
	RenderSkybox();
	RenderCamera();
}

void PlayStation4Renderer::RenderMainMenu() {

}

void PlayStation4Renderer::RenderPauseMenu() {

}

void PlayStation4Renderer::RenderCamera() {

}

MeshGeometry* PlayStation4Renderer::LoadMesh(const string& name) {

}

void PlayStation4Renderer::NewRenderLines() {

}

void PlayStation4Renderer::NewRenderText() {

}

TextureBase* PlayStation4Renderer::LoadTexture(const string& name) {

}

ShaderBase* PlayStation4Renderer::LoadShader(const string& vertex, const string& fragment) {

}

void PlayStation4Renderer::SetDebugStringBufferSizes(size_t newVertCount) {

}

void PlayStation4Renderer::SetDebugLineBufferSizes(size_t newVertCount) {

}

void PlayStation4Renderer::OnWindowResize(int w, int h){

}
