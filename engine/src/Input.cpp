#include "Input.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

// Get the singleton instance
Input& Input::Get() {
	static Input sInstance;
	return sInstance;
}

void Input::Initialize(GLFWwindow* iWindow)
{
	mWindow = iWindow;

	// Set GLFW callbacks
	glfwSetKeyCallback(mWindow, KeyCallback);
	glfwSetMouseButtonCallback(mWindow, MouseButtonCallback);
	glfwSetCursorPosCallback(mWindow, CursorPositionCallback);
	glfwSetScrollCallback(mWindow, ScrollCallback);

	// Initialize mouse position
	glfwGetCursorPos(mWindow, &mLastMouseX, &mLastMouseY);

	spdlog::info("Input System Initialized");
}

void Input::Setup()
{
	// Update previous key state before the next frame
	mPreviousKeyStates = mCurrentKeyStates;

	// Update mouse position deltas
	mLastMouseX = mCurrentMouseX;
	mLastMouseY = mCurrentMouseY;

	ResetScrollOffset();
}

bool Input::IsKeyPressed(int iKey)
{
	return mCurrentKeyStates[iKey];
}

bool Input::IsKeyDown(int iKey)
{
	return mCurrentKeyStates[iKey] && !mPreviousKeyStates[iKey];
}

bool Input::IsMouseButtonPressed(int iButton)
{
	return mMouseButtonStates[iButton];
}

std::pair<double, double> Input::GetMousePosition() const
{
	return { mCurrentMouseX, mCurrentMouseY };
}

std::pair<double, double> Input::GetMouseDelta() const
{
	return { mCurrentMouseX - mLastMouseX, mCurrentMouseY - mLastMouseY };
}

std::pair<double, double> Input::GetScrollOffset() const
{
	return { mScrollOffsetX, mScrollOffsetY };
}

void Input::ResetScrollOffset()
{
	mScrollOffsetX = 0.0;
	mScrollOffsetY = 0.0;
}

// Callbacks
void Input::KeyCallback(GLFWwindow* iWindow, int iKey, int iScancode, int iAction, int iMods)
{
	if (iAction == GLFW_PRESS)
	{
		Input::Get().mCurrentKeyStates[iKey] = true;
	}
	else if (iAction == GLFW_RELEASE)
	{
		Input::Get().mCurrentKeyStates[iKey] = false;
	}
}

void Input::MouseButtonCallback(GLFWwindow* iWindow, int iButton, int iAction, int iMods)
{
	if (iAction == GLFW_PRESS)
	{
		Input::Get().mMouseButtonStates[iButton] = true;
	}
	else if (iAction == GLFW_RELEASE)
	{
		Input::Get().mMouseButtonStates[iButton] = false;
	}
}

void Input::CursorPositionCallback(GLFWwindow* iWindow, double iXPos, double iYPos)
{
	Input::Get().mCurrentMouseX = iXPos;
	Input::Get().mCurrentMouseY = iYPos;
}

void Input::ScrollCallback(GLFWwindow* iWindow, double iXOffset, double iYOffset)
{
	Input::Get().mScrollOffsetX += iXOffset;
	Input::Get().mScrollOffsetY += iYOffset;
}