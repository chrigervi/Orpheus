/* Copyright (c) 2021, Christian Hertel */

#pragma once

#include "JuceHeader.h"
#include "orpheus_config.h"
#include <array>
#include "orpheus_DataTracker.h"

namespace orpheus
{
	class GainMeter : public juce::Component, public juce::Timer
	{
	private:
		std::array<float, 10> _cache{0};
		unsigned int _cacheIndex = 0;

		float _average = 0;

		juce::Component _meter;
		juce::Slider _slider;
		juce::Label _label;
	public:
		GainMeter(const juce::String& label);

		float getGainSliderValue();

		void timerCallback() override;

		void paint(juce::Graphics& g);

		void resized();

		void inputGain(float newValue);
	};
}