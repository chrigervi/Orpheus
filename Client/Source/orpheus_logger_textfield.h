/* Copyright (c) 2021, Christian Hertel */

#pragma once

#include "orpheus_log.h"

namespace orpheus {

	class TextFieldLogger : public juce::TextEditor, public LogDisplayer
	{
	public:
		TextFieldLogger();
		virtual ~TextFieldLogger();

		void update()
		{
		}

		void onNewLog(const std::string& msg);

	private:
		std::string text;
	};

}