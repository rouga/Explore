#pragma once

#include "Core/Engine.h"
#include "Core/Window.h"
#include "Renderer/UIManager.h"

class Renderer;

class Editor
{
public:
	Editor();

	void Initialize();

	void Run();

	void Shutdown();
	
private:
	void SetupUI();
	std::unique_ptr<Window> mWindow = nullptr;
	std::unique_ptr<UIManager> mUIManager = nullptr;
	Renderer* mRenderer = nullptr;

	float mEngineCPU = 0;
	float mRenderCPU = 0;
	float mCPUFrameTime = 0;
};