/* Copyright (c) 2021, Christian Hertel */

#pragma once

#include "orpheus_DataTrackerTable.h"

orpheus::DataTrackerTable::TableColumn::TableColumn()
{
    addAndMakeVisible(name);
    addAndMakeVisible(value);
}

orpheus::DataTrackerTable::TableColumn::TableColumn(const TableColumn& other)
    : TableColumn()
{
    name.setText(other.name.getText(), juce::NotificationType::dontSendNotification);
    value.setText(other.value.getText(), juce::NotificationType::dontSendNotification);

    setBounds(other.getBounds());
}

 void orpheus::DataTrackerTable::TableColumn::set(const DataTracker::DataSet& data)
{
    name.setText(data.name, juce::NotificationType::dontSendNotification);
    value.setText(juce::String(data.value), juce::NotificationType::dontSendNotification);
}

void orpheus::DataTrackerTable::TableColumn::paint(juce::Graphics& g)
{
    g.setColour(ORPHEUS_BGSUB_COLOR);
    g.drawLine(name.getRight(), name.getY(), name.getRight(), name.getBottom(), 1);
}

void orpheus::DataTrackerTable::TableColumn::resized()
{
    //int nameWidth = getLookAndFeel().getLabelFont(name).getStringWidth(name.getText());
    int width = 150;

    name.setBounds(0, 0, width, getHeight());
    value.setBounds(width + 10, 0, 200, getHeight());
}

orpheus::DataTrackerTable::DataTrackerTable(DataTracker *tracker)
{
    _Tracker = tracker;
    
    for (int i = 0; i < tracker->numData(); i++)
    {
        updateColumn(i);
    }
    
    _Tracker->addActionListener(this);
}

orpheus::DataTrackerTable::~DataTrackerTable()
{
    juce::ScopedLock lock(mutex);
    
    _Tracker->removeActionListener(this);
    
    for (auto it = _Table.begin(); it != _Table.end(); it++)
    {
        delete *it;
    }
}

void orpheus::DataTrackerTable::updateColumn(unsigned int index)
{
    if (index > _Table.size() - 1 || _Table.empty())
    {
        TableColumn* tc = new TableColumn();
        addAndMakeVisible(tc);
        _Table.resize(index + 1);
        _Table[index] = tc;
    }

    auto dataSet = _Tracker->getData(index);
    _Table[index]->set(*dataSet);
    
    resized();
}

void orpheus::DataTrackerTable::paint(juce::Graphics& g)
{
   for (auto it = _Table.begin(); it != _Table.end(); it++)
   {
       auto column = **it;
       unsigned int index = std::distance(_Table.begin(), it);

       if (index % 2 == 0)
       {
           g.setColour(ORPHEUS_BG_DARK_COLOR.withAlpha(0.5f));
       }
       else
       {
           g.setColour(ORPHEUS_BG_DARK_COLOR.withAlpha(0.2f));
       }

       g.fillRect(column.getBounds());
   }
}

void orpheus::DataTrackerTable::resized()
{
    //juce::Component::resized();

    auto height = 20;
    auto border = height * 0.1f * 0;
   
    auto width = getWidth();

    for (int i = 0; i < _Table.size(); i++)
    {
        _Table[i]->setBounds(0, border*0.5 + height * i, width, height - border * 0.5f);
    }

    setSize(getWidth(), height * _Table.size());
    //setBounds(0, 0, getWidth(), height * _Table.size());
}

void orpheus::DataTrackerTable::actionListenerCallback(const juce::String& message)
{
    juce::ScopedLock lock(mutex);
    
    for (int i = 0; i < _Tracker->numData(); i++)
    {
        if (_Tracker->columnNeedsUpdate(i)) updateColumn(i);
    }
}
