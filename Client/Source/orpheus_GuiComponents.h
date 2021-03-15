/* Copyright (c) 2021, Christian Hertel */

#pragma once

#include "JuceHeader.h"

namespace orpheus {
	
	class Button : public juce::TextButton
	{
	public:
		Button(juce::String label);
		~Button();
	};

}