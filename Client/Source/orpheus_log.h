/* Copyright (c) 2021, Christian Hertel */

#pragma once

#include "orpheus_config.h"
#include <string>
#include <vector>
#include <JuceHeader.h>

#define ORPHLOG(msg) ::orpheus::println(msg, typeid(*this).name());

namespace orpheus {

	class LogDisplayer
	{
	public:
		LogDisplayer();
		virtual ~LogDisplayer();

		virtual void onNewLog(const std::string& log) = 0;
	};

	class Log
	{
	public:
		friend LogDisplayer;

		static void log(const std::string& message);
		static void addDisplay(std::shared_ptr<LogDisplayer> display);

	private:

		static std::vector<std::weak_ptr<LogDisplayer>> displayers;
		static juce::CriticalSection mutex;
	};

	extern void print(const std::string& message, const std::string& instance = std::string());
	extern void println(const std::string& message, const std::string& instance = std::string());
	extern void dialogText(const std::string& title, const std::string& message);
	extern void dialogError(const std::string& message);
}
