/* Copyright (c) 2021, Christian Hertel */

#include "orpheus_ConsoleWindow.h"

orpheus::ConsoleWindow::ConsoleWindow()
: juce::DocumentWindow("Orpheus Console", juce::Colours::black, juce::DocumentWindow::TitleBarButtons::minimiseButton | TitleBarButtons::maximiseButton, false)
{
    _consoleLogger = std::make_shared<TextFieldLogger>();
    orpheus::Log::addDisplay(_consoleLogger);
    
    setSize(300, 170);
    setResizable(true, true);
    setVisible(true);
    addToDesktop();
    
    setContentOwned(_consoleLogger.get(), false);
}

orpheus::ConsoleWindow::~ConsoleWindow()
{
    
}
