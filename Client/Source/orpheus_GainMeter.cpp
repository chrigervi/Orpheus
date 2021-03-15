/* Copyright (c) 2021, Christian Hertel */

#pragma once
#include "orpheus_GainMeter.h"

orpheus::GainMeter::GainMeter(const juce::String& label)
{
	addAndMakeVisible(_meter);

	addAndMakeVisible(_label);
	_label.setText(label, juce::dontSendNotification);

	addAndMakeVisible(_slider);
	_slider.setRange(0, 2);
	_slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
	_slider.setValue(1);

	startTimer(ORPHEUS_REPAINT_TIME);
}

float orpheus::GainMeter::getGainSliderValue()
{
	return _slider.getValue();
}

void orpheus::GainMeter::timerCallback()
{
	float sum = 0;
	for (auto it = _cache.begin(); it != _cache.end(); it++) { sum += *it; }
	_average = sum / float(_cache.size());

	ORPH_TRACKDATA(_label.getText() + "-Gain", _average);

	repaint();
}

void orpheus::GainMeter::paint(juce::Graphics& g)
{
	g.setColour(ORPHEUS_BG_COLOR);
	g.fillRect(_meter.getBounds());

	float relHeight = juce::mapFromLog10<float>(_average, 0.1, 1);
	if (_average <= 0.0f || _average > 1.0f)
	{
		relHeight = _average;
	}

	unsigned int displayHeight = _meter.getHeight() * _average;
	unsigned int startY = (1 - _average) * _meter.getHeight();
	g.setColour(ORPHEUS_BG_DARK_COLOR);
	g.fillRect(_meter.getX(), startY, _meter.getWidth(), displayHeight);
}

void orpheus::GainMeter::resized()
{
	const int sliderWidth = 30;
	const int labelHeight = 20;

	setBounds(getX(), getY(), sliderWidth * 2, getHeight());

	_slider.setBounds(0, 0, sliderWidth, getHeight() - labelHeight);
	_meter.setBounds(sliderWidth, 0, sliderWidth, getHeight() - labelHeight);

	_label.setBounds(0, getHeight() - labelHeight, getWidth(), labelHeight);

}

void orpheus::GainMeter::inputGain(float newValue)
{
	_cache[_cacheIndex] = newValue;
	++_cacheIndex %= _cache.size();
}
