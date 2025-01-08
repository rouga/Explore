#pragma once

#include <string>

struct GLFWwindow;
class Engine;

class Window
{
public:
	Window(Engine* iEngine);
	~Window();

	void Initialize(int iWidth, int iHeight, std::string iTitle);
	bool ShouldClose() const;

	GLFWwindow* GetGLFWWindow() const { return mWindow;}
	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }

	void SetWidth(int iWidth)  { mWidth = iWidth; }
	void SetHeight(int iHeight) { mHeight = iHeight; }

	bool IsMinimized() const;

private:
	int mWidth;
	int mHeight;

	GLFWwindow* mWindow = nullptr;
	Engine* mEngine = nullptr;
};