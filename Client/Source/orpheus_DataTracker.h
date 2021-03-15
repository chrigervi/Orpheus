/* Copyright (c) 2021, Christian Hertel */

#pragma once

#include "JuceHeader.h"

#include <vector>

#include "orpheus_config.h"

#define ORPH_TRACKDATA(name, value) if (orpheus::DataTracker::getGlobalTracker() != nullptr) { orpheus::DataTracker::getGlobalTracker()->setData(name, value); }

namespace orpheus
{
	class DataTracker : public juce::ActionBroadcaster, public juce::Timer
	{
    public:
        struct DataSet
        {
            juce::String name;
            float value;
        };
        
	private:

		std::vector<DataSet> _data;
		juce::CriticalSection mutex;
		std::vector<unsigned int> _columsToUpdate;

		static DataTracker* _globalTracker;

	public:
		static void setGlobalTracker(DataTracker* tracker);
		static DataTracker* getGlobalTracker();

		DataTracker();
		~DataTracker();

		void setData(const juce::String& name, float newValue);
        const DataSet* getData(unsigned int index);
        const DataSet* getData(juce::String& name);
        unsigned int numData();
        bool columnNeedsUpdate(unsigned int index);

		void timerCallback();
	};
}
