#include "Logger.h"


Logger& Logger::Get()
{
	static Logger sInstance;
	std::atexit([]() { sInstance; });
	return sInstance;
}

void Logger::Initialize(bool iWithUI)
{
	mConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();;
	mConsoleSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
	if (iWithUI)
	{
		mImguiSink = std::make_shared<ImGuiSink>();
		mImguiSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
	}

	mSelectiveSink = std::make_shared<SelectiveSink>(mConsoleSink, mImguiSink);
	mLogger = std::make_shared<spdlog::logger>("Logger", mSelectiveSink);
	mLogger->set_level(spdlog::level::debug);
	spdlog::register_logger(mLogger);
}