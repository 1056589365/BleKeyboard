#pragma once
#include <iwgl.h>


class Menu: public Panel
{
public:
	Menu(PM* pm);
	~Menu();

	void onDraw();
	void onUpdate(TickType_t deltaTime) ;
	void onInput(Args parameters);
};

Menu::Menu(PM* pm) : Panel(pm)
{
	addElement("title",   new Label("ab", 63, 1, 120, 13, u8g2_font_wqy12_t_gb2312, Label::CENTER, Label::H_FLIP));
}

Menu::~Menu()
{

}

void Menu::onDraw()
{
	//((Label*)getElement("title"))->text.assign("");

	Panel::onDraw();
}

void Menu::onUpdate(TickType_t deltaTime)
{

}

void Menu::onInput(Args parameters)
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