#include <JuceHeader.h>

#include "orpheus_Server.h"

//==============================================================================
int main (int argc, char* argv[])
{
    juce::MessageManager::getInstance();

    orpheus::Server server(17995);

    juce::MessageManager::getInstance()->runDispatchLoop();

    std::cout << "Shutting down the server!" << std::endl;

    return 0;
}
