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

private:
	GLFWwindow* mWindow = nullptr;
};