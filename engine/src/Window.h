#pragma once

#include <string>

struct GLFWwindow;

class Window
{
public:
	Window();
	~Window();

	void Initialize(int iWidth, int iHeight, std::string iTitle);
	bool ShouldClose() const;

	GLFWwindow* GetGLFWWindow() const { return mWindow;}
	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }

private:
	int mWidth;
	int mHeight;

	GLFWwindow* mWindow = nullptr;
};