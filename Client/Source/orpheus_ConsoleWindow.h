/* Copyright (c) 2021, Christian Hertel */

#ifndef orpheus_ConsoleWindow_h
#define orpheus_ConsoleWindow_h

#include "JuceHeader.h"
#include "orpheus_logger_textfield.h"
#include <memory>

namespace orpheus {

class ConsoleWindow : public juce::DocumentWindow
{
private:
    std::shared_ptr<orpheus::TextFieldLogger> _consoleLogger;
    
protected:
    
public:
    ConsoleWindow();
    ~ConsoleWindow();
};

}

#endif /* orpheus_ConsoleWindow_h */
