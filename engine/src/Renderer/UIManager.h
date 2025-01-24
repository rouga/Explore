#pragma once

#include <functional>
#include <unordered_map>
#include <string>

class Window;
class Entity;
enum class EntityType;

class UIManager
{
public:
	using UIElementCallback = std::function<void()>;
	using UIPropertiesCallback = std::function<void(void*)>;

	UIManager(Window* iWindow);
	~UIManager();

	void BeginFrame();
	void Execute();

	void AddUIElement(const std::string& iName, const UIElementCallback& iCallback);
	void RemoveUIElement(const std::string& iName);
	void ClearUIElements();
	void DrawUIProperties(Entity* iEntity);

private:
	void SetupCameraDetailsPanel();
	void SetupLightDetailsPanel();
	std::unordered_map<std::string, UIElementCallback> mUIElements;
	std::unordered_map<EntityType, UIPropertiesCallback> mPropertiesUI;
};