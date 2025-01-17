#pragma once

#include <string>
#include <unordered_map>

enum class EntityType
{
	eModel,
	eCamera,
	eLight,
	eNone,
};

class Entity
{
public:
	Entity(EntityType iType)
		:mType(iType) {mName = TypeToString(mType); }

	EntityType GetType() const {return mType;}

	void SetName(const std::string& iName) { mName = iName; }

	const std::string& GetName() const { return mName; }

private:
	// Returns the name of the enumeration as a string
	static std::string TypeToString(EntityType iType) 
	{
		static const std::unordered_map<EntityType, std::string> TypeNames = {
				{EntityType::eModel, "Model"},
				{EntityType::eCamera, "Camera"},
				{EntityType::eLight, "Light"},
		};

		static std::unordered_map<EntityType, int> TypeCount = {
		{EntityType::eModel, 0},
		{EntityType::eCamera, 0},
		{EntityType::eLight, 0},
		};

		auto wTypeStr = TypeNames.find(iType);
		auto wTypeCount = TypeCount.find(iType);
		if (wTypeStr != TypeNames.end() && wTypeCount != TypeCount.end())
		{
			auto string = wTypeStr->second + " " + std::to_string(wTypeCount->second);
			wTypeCount->second += 1;
			return string;
		}

		return "Unknown";
	}

	EntityType mType = EntityType::eNone;
	std::string mName;
};