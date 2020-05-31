#pragma once
#include <iwgl.h>


class Template: public Panel
{
public:
	Template(PM* pm);
	~Template();

	void onDraw();
	void onUpdate(TickType_t deltaTime) ;
	void onResult();
	void onInput(Args parameters);
};

Template::Template(PM* pm) : Panel(pm)
{
	addElement("title",   new Label("ab", 63, 1, 120, 13, u8g2_font_wqy12_t_gb2312, Label::CENTER, Label::H_FLIP));
}

Template::~Template()
{

}

void Template::onDraw()
{
	((Label*)getElement("title"))->text.assign("");

	Panel::onDraw();
}

void Template::onUpdate(TickType_t deltaTime)
{

}

void Template::onResult()
{
	
}

void Template::onInput(Args parameters)
{
	switch(parameters)
	{
		case KeyCode::ENTER:
			setResult("value", "1");
			break;
		default:
			setResult("value", "0");
			break;
	}

	exit();
}