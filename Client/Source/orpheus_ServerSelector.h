#pragma once
#include "orpheus_config.h"
#include "orpheus_GuiComponents.h"

namespace orpheus {
	
	class NetworkServerSelector
		: public juce::Component
	{
	public:
		NetworkServerSelector();
		~NetworkServerSelector() {}

		void paint(juce::Graphics& g);
		void resized();

	private:
		juce::LookAndFeel_V4 appearence;

		juce::Label labelServerIP;
		juce::TextEditor inputServerIP;

		orpheus::Button inputContinue{ "Continue" };
	};

}