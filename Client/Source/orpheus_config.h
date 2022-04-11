/* Copyright (c) 2021, Christian Hertel */

/**
* This file is part of "orpheus".
*
* Within this file, basic constants and parameters for the Orpheus Application are defined.
* Changing these constants will manipulate Orpheus' appearence and / or functionality.
*/

#pragma once

#include <JuceHeader.h>

namespace orpheus {
/// In C++, the char type is defined es exactly 8 bits in memory size --> Synonym to a byte
typedef char byte;

/** Constants used in audio streaming and compression */
/// Size of the audio packets which are sent via the Orpheus network protocol (in SAMPLES)
#define ORPHEUS_AUDIOSTREAM_BSIZE 44100

/// Size of the audio buffers for recieved audio data
#define ORPHEUS_RECDATA_BUFFERS (ORPHEUS_AUDIOSTREAM_BSIZE * 2)

/// Determines how many bits correspond with one audio sample
#define ORPHEUS_AUDIOCOMPRESSION_BITS_PER_SAMPLE 32

// Determines which JUCE-supported audio codec is used by Orpheus
#define ORPHEUS_AUDIOCOMPRESSION_JUCE_CODEC juce::OggVorbisAudioFormat

// Determines the compression quality (so.. bitrate of the compression)
#define ORPHEUS_AUDIOCOMPRESSION_STD_QUALITY_INDEX 4 //Ogg: 4 = 128kbps

/// Determines the strength of the audio compression
#define ORPHEUS_COMPRESSION_LEVEL 7
/* ============================================================= */



/** Constants used within the Oprheus network protocol */
/// Package starter for approving incoming client connections (Server --> CLient)
#define ORPHEUS_CONNECTION_APPROVED 6868927612724466250

/// Package starter for refusing incoming client connections (Server --> Client)
#define ORPHEUS_CONNECTION_REFUSED 6868927612724466251

/// Package starter to determine the client's role as a GUEST within a Oprheus session (Client --> Server)
#define ORPHEUS_CONNECTION_TYPE_GUEST 6868927612724466252

/// Package starter to determine the client's role as a HOST within a Oprheus session (Client --> Server)
#define ORPHEUS_CONNECTION_TYPE_HOST 6868927612724466253

/// Package starter to initiate the audio stream (Server --> Client)
#define ORPHEUS_START_STREAM 6868927612724466254

/// Package starter to stop the audio stream (Server --> Client)
#define ORPHEUS_STOP_STREAM 6868927612724466255

/// Package starter to mark a packet containing streamed audio data (Client --> Server --> CLient)
#define ORPHEUS_STREAM_PACKET 6868927612724466256
/* ============================================================= */



/** Constants defining the Orpheus GUI style and appearence */
/// Time between redrawing the GUI (in ms)
#define ORPHEUS_REPAINT_TIME 100

/// Aspect ratio of the Orpheus interface window (width / height)
#define ORPHEUS_EDITOR_ASPECT_RATIO (4.0f / 4.0f)

/// Width of the Orpheus interface window
#define ORPHEUS_EDITOR_WIDTH 500

/// Background color of the Orpheus GUI
#define ORPHEUS_BG_COLOR (juce::Colour(79, 81, 140))

/// Contrast Color for the Orpheus GUI
#define ORPHEUS_BG_DARK_COLOR (juce::Colour(44, 42, 74))

/// Light Color for the Orpheus GUI
#define ORPHEUS_BGSUB_COLOR (juce::Colour(93, 96, 162))
/* ============================================================= */


/** FOLLOWING CODE IS OUTDATED */


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
