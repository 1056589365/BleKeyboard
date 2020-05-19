#include "panels.h"
#include "esp_log.h"

Dialog::Dialog(string title, string content, PM* pm, long duration) : Panel(pm)
{
	this->title = title;
	this->content = content;
	this->duration = duration;

	addElement("title",   new Label("", 63, 1, 120, 13, u8g2_font_wqy12_t_gb2312,     Label::CENTER, Label::H_FLIP));
	addElement("Line2",   new Line(20, 15, 107, 15));
	addElement("content", new Label("", 63, 18, 120, 340, u8g2_font_wqy12_t_gb2312, Label::CENTER, Label::V_LOOP));
}


Dialog::~Dialog()
{
	
}

void Dialog::onDraw()
{
	if(elapse==-1)
		elapse = 0;

	((Label*)getElement("title"))->text.assign(title);

	((Label*)getElement("content"))->text.assign(content);

	Panel::onDraw();
}

void Dialog::onUpdate(TickType_t deltaTime)
{
	if(duration>0 && elapse!=-1)
	{
		elapse += deltaTime;

		//ESP_LOGI("display", "elapse: %ld, dt: %ld", elapse, (long)deltaTime);

		if(elapse>=duration)
		{
			setResult("reason", 0);
			exit();
		}
	}
}

void Dialog::onInput(Args parameters)
{
	switch(parameters)
	{
		case KeyCode::ENTER:
			setResult("reason", "1");
			break;
		default:
			setResult("reason", "0");
			break;
	}

	exit();
}

void Dialog::onResult()
{

}