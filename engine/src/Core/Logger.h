#pragma once

#include <memory>

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/base_sink.h>

class ImGuiSink : public spdlog::sinks::base_sink<std::mutex>
{
public:
	ImGuiSink() = default;

	// Expose your messages if you want them to be accessed externally
	const std::vector<std::string>& get_messages() const
	{
		return mMgs;
	}

protected:
	void sink_it_(const spdlog::details::log_msg& msg) override
	{
		// Use the formatter to prepare the log message
		spdlog::memory_buf_t formatted;
		base_sink<std::mutex>::formatter_->format(msg, formatted);

		// Store the formatted message (std::string) in our vector
		mMgs.emplace_back(fmt::to_string(formatted));

		// Limit the buffer size to avoid excessive memory usage
		const size_t max_messages = 1000;
		if (mMgs.size() > max_messages)
		{
			mMgs.erase(mMgs.begin());
		}
	}

	void flush_() override
	{
		// No specific flushing needed in this sink
	}

private:
	std::vector<std::string> mMgs;
};

// Aggregator sink for selective logging
class SelectiveSink : public spdlog::sinks::base_sink<std::mutex>
{
public:
	SelectiveSink(std::shared_ptr<spdlog::sinks::sink> console_sink,
		std::shared_ptr<ImGuiSink> iImguiSink)
		: mConsoleSink(console_sink), mImguiSink(iImguiSink) {}

protected:
	void sink_it_(const spdlog::details::log_msg& iMsg) override
	{
		// Routing logic: Console for info and below, UI for warnings and above
		if (iMsg.level <= spdlog::level::info)
		{
			mConsoleSink->log(iMsg);
		}
		if(mImguiSink && iMsg.level >= spdlog::level::info)
		{
			mImguiSink->log(iMsg);
		}
	}

	void flush_() override
	{
		mConsoleSink->flush();
		if(mImguiSink)
		{
			mImguiSink->flush();
		}

	}

private:
	std::shared_ptr<spdlog::sinks::sink> mConsoleSink = nullptr;
	std::shared_ptr<ImGuiSink> mImguiSink = nullptr;
};

class Logger
{
public:
	// Get the singleton instance
	static Logger& Get();

	void Initialize(bool iWithUI);

	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;


	std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> mConsoleSink = nullptr;
	std::shared_ptr<ImGuiSink> mImguiSink = nullptr;
	std::shared_ptr<SelectiveSink> mSelectiveSink = nullptr;
	std::shared_ptr<spdlog::logger> mLogger = nullptr;
private:
	// Private constructor for Singleton
	Logger() = default;

};