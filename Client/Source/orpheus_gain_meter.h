#pragma once

#include "orpheus_config.h"
#include <JuceHeader.h>

namespace orpheus {

#define ORPHEUS_GAINMETER_VALSMOOTH 6

    struct GainMeter
    {
        GainMeter()
        {
            gain_display.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
            gain_display.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentWhite);
            //host_gain_display.setColour(juce::Slider::ColourIds::, juce::Colours::red);
            gain_display.setColour(juce::Slider::ColourIds::trackColourId, ORPHEUS_BG_DARK_COLOR);
            gain_display.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
            gain_display.setEnabled(false);
            //DECIBEL RANGE
            gain_display.setRange(-100, 0);
            //gain_display.setRange(0, 1);
            gain_display.setValue(0);
            value = 0;

            gain_slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
            gain_slider.setRange(0, 2);
            gain_slider.setValue(1);
        }

        void add(juce::Component& parent)
        {
            parent.addAndMakeVisible(gain_display);
            parent.addAndMakeVisible(gain_slider);
        }

        void setValue(float val)
        {
            val = juce::Decibels::gainToDecibels(val);
            //value = val;
            value_cache[current_index++] = val;

            current_index %= ORPHEUS_GAINMETER_VALSMOOTH;

            updateValue();
        }

        float getValue() 
        {
            return value;
        }

        void paint(juce::Graphics& g)
        {
            g.setColour(ORPHEUS_BG_COLOR);
            g.fillRect(gain_display.getBounds());
            gain_display.setValue(getValue() * gain_slider.getValue());
        }

        void resize(int posx, int posy)
        {
            gain_slider.setBounds(posx, posy, element_width, 80);
            gain_display.setBounds(gain_slider.getBounds().getRight(), posy, element_width, 80);
        }

        int getWidth() { return element_width * 2; }
        float getGainFactor() { return (float) gain_slider.getValue(); }

    private: 
        void updateValue()
        {
            float sum = 0;
            for (int i = 0; i < max_index; i++) {
                sum += value_cache[i];
            }
            //ORPHLOG(std::to_string(sum));
            value = ((float)sum / (float)max_index);
            if (max_index < ORPHEUS_GAINMETER_VALSMOOTH) max_index++;
        }

        juce::Slider gain_slider;
        juce::Slider gain_display;

        unsigned int current_index = 0;
        std::array<float, ORPHEUS_GAINMETER_VALSMOOTH> value_cache;
        float value = 0;
        int max_index = 1;

        const int element_width = 24;
    };

}