/** (c) Christian Hertel 2022 */

/***/

#include <JuceHeader.h>
#include "orpheus_Server.h"

//==============================================================================
int main (int argc, char* argv[])
{
    // Initiliasing the JUCE message thread
    juce::MessageManager::getInstance();

    // Initiliasing the Orpheus server on port 17995
    orpheus::Server server(17995);

    // Start up the JUCE message thread
    juce::MessageManager::getInstance()->runDispatchLoop();

    std::cout << "Shutting down the server!" << std::endl;

    return 0;
}
