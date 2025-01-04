#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>
#include <utility>

class Input {
public:
	// Get the singleton instance
	static Input& Get();

	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

	void Initialize(GLFWwindow* iWindow);

	// SHould be called every frame, before GLFW Poll
	void Setup();

	// Check if key was just pressed (Continuous Poll)
	bool IsKeyPressed(int iKey);

	// Check if key was just pressed (One time Poll)
	bool IsKeyDown(int iKey);

	bool IsMouseButtonPressed(int button);

	std::pair<double, double> GetMousePosition() const;

	std::pair<double, double> GetMouseDelta() const;

	std::pair<double, double> GetScrollOffset() const;

private:
	// Private constructor for Singleton
	Input() = default;

	GLFWwindow* mWindow = nullptr;

	std::unordered_map<int, bool> mCurrentKeyStates;
	std::unordered_map<int, bool> mPreviousKeyStates;

	std::unordered_map<int, bool> mMouseButtonStates;

	double mLastMouseX = 0.0;
	double mLastMouseY = 0.0;
	double mCurrentMouseX = 0.0;
	double mCurrentMouseY = 0.0;

	double mScrollOffsetX = 0.0;
	double mScrollOffsetY = 0.0;

	void ResetScrollOffset();

	// Callbacks
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
	static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};