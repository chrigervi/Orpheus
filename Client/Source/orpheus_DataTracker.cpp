/* Copyright (c) 2021, Christian Hertel */

#include "orpheus_DataTracker.h"

//STATIC VARS
orpheus::DataTracker* orpheus::DataTracker::_globalTracker = nullptr;


void orpheus::DataTracker::setGlobalTracker(DataTracker* tracker)
{
	_globalTracker = tracker;
}

orpheus::DataTracker* orpheus::DataTracker::getGlobalTracker()
{
	//jassert(_globalTracker == nullptr);
	return _globalTracker;
}

orpheus::DataTracker::DataTracker()
{
	startTimer(ORPHEUS_REPAINT_TIME);
}

orpheus::DataTracker::~DataTracker() {}

void orpheus::DataTracker::setData(const juce::String& name, float newValue)
{
    juce::ScopedLock lock(mutex);
	for (auto it = _data.begin(); it != _data.end(); it++)
	{
		if (it->name.equalsIgnoreCase(name))
		{
			it->value = newValue;
			_columsToUpdate.push_back(std::distance(_data.begin(), it));
			return;
		}
	}

	DataSet element;
	element.name = name;
	element.value = newValue;
	_data.push_back(element);

	_columsToUpdate.push_back(_data.size() - 1);
}

const orpheus::DataTracker::DataSet* orpheus::DataTracker::getData(unsigned int index)
{
    juce::ScopedLock lock(mutex);
    
    if (index < 0 || index >= _data.size()) return nullptr;
    
    return &_data[index];
}

const orpheus::DataTracker::DataSet* orpheus::DataTracker::getData(juce::String& name)
{
    juce::ScopedLock lock(mutex);
    for (auto it = _data.begin(); it != _data.end(); it++)
    {
        if (it->name.equalsIgnoreCase(name))
        {
            return &*it;
        }
    }
    
    return nullptr;
}

unsigned int orpheus::DataTracker::numData()
{
    return _data.size();
}

bool orpheus::DataTracker::columnNeedsUpdate(unsigned int index)
{
    juce::ScopedLock lock(mutex);
    for (int i = 0; i < _columsToUpdate.size(); i++)
    {
        if (_columsToUpdate[i] == index) return true;
    }
    
    return false;
}

void orpheus::DataTracker::timerCallback()
{
    sendActionMessage("");
}
