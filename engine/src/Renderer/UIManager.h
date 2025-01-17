#pragma once

#include <functional>
#include <unordered_map>
#include <string>

class Window;

class UIManager
{
public:
	using UIElementCallback = std::function<void()>;

	UIManager(Window* iWindow);
	~UIManager();

	void BeginFrame();
	void Execute();

	void AddUIElement(const std::string& iName, const UIElementCallback& iCallback);
	void RemoveUIElement(const std::string& iName);
	void ClearUIElements();

private:
	std::unordered_map<std::string, UIElementCallback> mUIElements;
};