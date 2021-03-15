#include "orpheus_ServerSelector.h"

orpheus::NetworkServerSelector::NetworkServerSelector()
{
	addAndMakeVisible(inputContinue);
}

void orpheus::NetworkServerSelector::paint(juce::Graphics& g)
{
#define relw(w) (w * this->getWidth())
#define relh(h) (h * this->getHeight())

	g.setColour(ORPHEUS_BGSUB_COLOR);
	g.fillRect(this->getBounds());

	getLookAndFeel().drawSpinningWaitAnimation(g, ORPHEUS_BG_DARK_COLOR, 0, 0, this->getWidth(), this->getHeight());
}

void orpheus::NetworkServerSelector::resized()
{
	inputContinue.setBounds(0, 0, 100, 40);
}
