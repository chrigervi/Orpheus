/* Copyright (c) 2021, Christian Hertel */

#pragma once

#include <JuceHeader.h>

namespace orpheus {
	typedef char byte;

#define ORPHEUS_AUDIOSTREAM_BSIZE (44100 / 1)
#define ORPHEUS_RECDATA_BUFFERS (ORPHEUS_AUDIOSTREAM_BSIZE * 2)

#define ORPHEUS_AUDIOCOMPRESSION_BITS_PER_SAMPLE 32
#define ORPHEUS_AUDIOCOMPRESSION_JUCE_CODEC juce::OggVorbisAudioFormat
#define ORPHEUS_AUDIOCOMPRESSION_STD_QUALITY_INDEX 4 //Ogg: 4 = 128kbps
#define ORPHEUS_COMPRESSION_LEVEL 7

//network protocol
/*#define ORPHEUS_PREFIX_STARTSTREAM		6868927612724466255
#define ORPHEUS_PREFIX_STREAMPACKET		6368927612724466255
#define ORPHEUS_PREFIX_STOPSTREAM		6868927612824466255

#define ORPHEUS_CONNECTION_TYPE_HOST	6868944412724466256
#define ORPHEUS_CONNECTION_TYPE_GUEST	6868944412724466257*/
#define ORPHEUS_CONNECTION_APPROVED		6868927612724466250
#define ORPHEUS_CONNECTION_REFUSED		6868927612724466251
#define ORPHEUS_CONNECTION_TYPE_GUEST	6868927612724466252
#define ORPHEUS_CONNECTION_TYPE_HOST	6868927612724466253
#define ORPHEUS_START_STREAM			6868927612724466254
#define ORPHEUS_STOP_STREAM				6868927612724466255
#define ORPHEUS_STREAM_PACKET			6868927612724466256


//STYLE
#define ORPHEUS_REPAINT_TIME 100

#define ORPHEUS_EDITOR_ASPECT_RATIO (4.0f / 4.0f)
#define ORPHEUS_EDITOR_WIDTH 500

//purple theme
#define ORPHEUS_BG_COLOR (juce::Colour(79, 81, 140))
#define ORPHEUS_BG_DARK_COLOR (juce::Colour(44, 42, 74))
#define ORPHEUS_BGSUB_COLOR (juce::Colour(93, 96, 162))

//custom theme
/*#define ORPHEUS_BG_COLOR (juce::Colour(112, 162, 136))
#define ORPHEUS_BG_DARK_COLOR (juce::Colour(3, 29, 68))
#define ORPHEUS_BGSUB_COLOR (juce::Colour(9, 129, 215))*/

// Global functions
// Shortcut for logging a message

inline void orpheus_init() {}

#define ORPH_globallock() juce::ScopedLock orp_globallock(::orpheus::getGlobalLockSection())
//#define ORPH_globallock() ORPHLOG("lock")
	inline juce::CriticalSection& getGlobalLockSection() 
	{
		static juce::CriticalSection section = juce::CriticalSection();

		return section;
	}
}
