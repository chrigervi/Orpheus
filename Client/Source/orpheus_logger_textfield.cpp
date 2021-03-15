/* Copyright (c) 2021, Christian Hertel */

#include "orpheus_logger_textfield.h"

orpheus::TextFieldLogger::TextFieldLogger()
{
	setReadOnly(true);
	setMultiLine(true);

	text = "";
}

orpheus::TextFieldLogger::~TextFieldLogger()
{
	ORPHLOG("dest");
}

void orpheus::TextFieldLogger::onNewLog(const std::string& msg)
{
	text += msg + "\n";

	this->setText(text);
	this->moveCaretToEnd();
}
