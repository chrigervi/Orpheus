/* Copyright (c) 2021, Christian Hertel */

#include "orpheus_log.h"

std::vector<std::weak_ptr<orpheus::LogDisplayer>> orpheus::Log::displayers = std::vector<std::weak_ptr<orpheus::LogDisplayer>>();
juce::CriticalSection orpheus::Log::mutex = juce::CriticalSection();

orpheus::LogDisplayer::LogDisplayer()
{
}

orpheus::LogDisplayer::~LogDisplayer() 
{
	//delete this from displayers
	juce::ScopedLock lock(Log::mutex);
	for (auto it = Log::displayers.begin(); it != Log::displayers.end();)
	{
		if ((*it).lock().get() == this)
		{
			Log::displayers.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void orpheus::Log::log(const std::string& message)
{
	juce::MessageManagerLock mmlock;
	juce::ScopedLock lock(Log::mutex);
	for (int i = 0; i < displayers.size(); i++)
	{
		if (displayers[i].expired())
		{
			displayers.erase(displayers.begin() + i);
			i--;
			continue;
		}

		displayers[i].lock()->onNewLog(message);
	}
}

void orpheus::Log::addDisplay(std::shared_ptr<LogDisplayer> display)
{
	juce::ScopedLock lock(Log::mutex);
	displayers.push_back(display);
}

void orpheus::print(const std::string& message, const std::string& instance)
{
	std::string rmsg;
	if (instance.empty() == false)
	{
		rmsg = "[" + instance + "] " + message;
	}
	else rmsg = message;

	juce::Logger::writeToLog(rmsg);
	orpheus::Log::log(rmsg);
}

void orpheus::println(const std::string& message, const std::string& instance)
{
	//print(message + '\n', instance);
	print(message, instance);
}

void orpheus::dialogText(const std::string& title, const std::string& message)
{
	juce::MessageManagerLock mlock;

	juce::Label* content = new juce::Label("orpheus_dialog_content", message);
	const unsigned int width = 220, height = 50;
	content->setBounds(0, 0, width, height);
	content->setFont(17.0f);
	content->setRepaintsOnMouseActivity(false);
	content->setColour(juce::Label::backgroundColourId, juce::Colours::white);
	content->setColour(juce::Label::textColourId, juce::Colours::black);
	content->setJustificationType(juce::Justification::centred);

	juce::DialogWindow::LaunchOptions l_options;
	l_options.dialogTitle = title;
	l_options.content.set(content, true);
	l_options.escapeKeyTriggersCloseButton = true;
	l_options.resizable = false;

	l_options.launchAsync();

	//log
	println(message, title);
}

void orpheus::dialogError(const std::string& message)
{
	dialogText("Orpheus - Error", message);
}
