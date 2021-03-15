#pragma once

#include "orpheus_log.h"

namespace orpheus {

	class TextFieldLogger : public juce::TextEditor, public LogDisplayer
	{
	public:
		TextFieldLogger()
		{
			setReadOnly(true);
		}

		void update()
		{
			this->setText(text);
			this->moveCaretToEnd();
		}

		void onNewLog(const std::string& msg)
		{
			text += msg + "\n";
		}

	private:
		std::string text;
	};

}