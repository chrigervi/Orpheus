/* Copyright (c) 2021, Christian Hertel */

#ifndef orpheus_DataTrackerTable_h
#define orpheus_DataTrackerTable_h

#include "orpheus_DataTracker.h"

namespace orpheus
{

class DataTrackerTable : public juce::Component, public juce::ActionListener
{
private:
    DataTracker* _Tracker;
    
    class TableColumn : public juce::Component
    {
    private:
        juce::Label name;
        juce::Label value;

    public:
        TableColumn();

        TableColumn(const TableColumn& other);

        void set(const DataTracker::DataSet& data);

        void paint(juce::Graphics& g);

        void resized();
    };
    
    void updateColumn(unsigned int index);
    std::vector<TableColumn*> _Table;
    
    juce::CriticalSection mutex;
    
public:
    DataTrackerTable(DataTracker* tracker);
    virtual ~DataTrackerTable();
    
    void paint(juce::Graphics& g);
    void resized();
    
    void actionListenerCallback(const juce::String& message);
};

}

#endif /* orpheus_DataTrackerTable_h */
