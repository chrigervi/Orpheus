/* Copyright (c) 2021, Christian Hertel */

#include "orpheus_GuiComponents.h"

#include "orpheus_config.h"

orpheus::Button::Button(juce::String label)
	: juce::TextButton(label)
{
	setColour(juce::TextButton::ColourIds::buttonColourId, ORPHEUS_BGSUB_COLOR);
	setColour(juce::TextButton::ColourIds::buttonOnColourId, ORPHEUS_BG_DARK_COLOR);
}

orpheus::Button::~Button()
{
}