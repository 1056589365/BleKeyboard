/*
 * weather.cpp
 *
 *  Created on: 2018年11月6日
 *      Author: shengYang
 */
#include "cbui.h"
#include "app.h"

using namespace std;

namespace app
{
	panel_weather::panel_weather(panel_weather& arg) : panel(arg)
	{
		this->arg = arg.arg;
		cg = new c_componentGroup(0, 0, -1, -1);
		cg->sub_components.insert(arg.cg->sub_components.begin(), arg.cg->sub_components.end());
	}

	panel_weather::panel_weather(panel_weather_argument& arg) : panel()
	{
		this->arg = arg;
//		this->arg.we_today = &we_today;
//		this->arg.we_tomorrow = &we_tomorrow;
//		this->arg.update_time = &update_time;

		cg = new c_componentGroup(0, 0, -1, -1);

		cg->addComponent("day", (ui_component*)new c_text("", 5, 2, 55, 13, u8g2_font_wqy12_t_gb2312, TEXT_ALIGNMENT_LEFT, SCROLL_DIRECTION_HORIZONTAL_FLIP));
		cg->addComponent("night", (ui_component*)new c_text("", 5, 16, 55, 13, u8g2_font_wqy12_t_gb2312, TEXT_ALIGNMENT_LEFT, SCROLL_DIRECTION_HORIZONTAL_FLIP));
		cg->addComponent("max_temperature", (ui_component*)new c_text("", 69, 2, 55, 13, u8g2_font_wqy12_t_gb2312, TEXT_ALIGNMENT_LEFT, SCROLL_DIRECTION_HORIZONTAL_FLIP));
		cg->addComponent("min_temperature", (ui_component*)new c_text("", 69, 16, 55, 13, u8g2_font_wqy12_t_gb2312, TEXT_ALIGNMENT_LEFT, SCROLL_DIRECTION_HORIZONTAL_FLIP));
		cg->addComponent("update_time", (ui_component*)new c_text("", 63, 34, 120, 13, u8g2_font_wqy12_t_gb2312, TEXT_ALIGNMENT_CENTER, SCROLL_DIRECTION_HORIZONTAL_FLIP));
		cg->addComponent("description", (ui_component*)new c_text("↑切换到今天  ○后退  ↓切换到明天", 63, 50, 120, 13, u8g2_font_wqy12_t_gb2312, TEXT_ALIGNMENT_CENTER, SCROLL_DIRECTION_HORIZONTAL_FLIP));

		cg->addComponent("recting", (ui_component*)new c_rectangle(0, 0, 128, 64, 3));
		cg->addComponent("line", (ui_component*)new c_line(0, 48, 127, 48));
	}

	panel_weather::panel_weather(panel_weather_argument& arg, void (*onFinishWithoutClass)(int parameters)) : panel_weather(arg)
	{
		this->onFinishWithoutClass = onFinishWithoutClass;
	}

	void panel_weather::onResult(u8g2_t* u8g2, int result, int requestCode, panelManager* pm)
	{

	}

	void panel_weather::onInput(u8g2_t* u8g2, panel_input parameters, panelManager* pm)
	{
		switch(parameters)
		{
			case PANEL_INPUT_UP:
			case PANEL_INPUT__UP:
				arg.show_today = true;
				break;
			case PANEL_INPUT_DOWN:
			case PANEL_INPUT__DOWN:
				arg.show_today = false;
				break;
			case PANEL_INPUT_ENTER:
			case PANEL_INPUT__ENTER:
				finish();
				break;
			case PANEL_INPUT_NONE:
			default:
				returnCode = -1;
				break;
		}
	}

	void panel_weather::drawFrame(u8g2_t* u8g2, panelManager* pm)
	{
		weather ctw = *((arg.show_today)?(arg.we_today):(arg.we_tomorrow));

		static char buf[64];

		sprintf(buf, "白天:%s", ctw.day.data());
		((c_text*)cg->getComponent("day"))->text.assign(buf);
		sprintf(buf, "夜间:%s", ctw.night.data());
		((c_text*)cg->getComponent("night"))->text.assign(buf);

		sprintf(buf, "最高: %1.1f℃", ctw.max_temp);
		((c_text*)cg->getComponent("max_temperature"))->text.assign(buf);
		sprintf(buf, "最低: %1.1f℃", ctw.min_temp);
		((c_text*)cg->getComponent("min_temperature"))->text.assign(buf);

		time_t cstTime = *arg.update_time + 8*60*60;
		tm* tp = gmtime(&cstTime);
		strftime(buf, 32, "%m-%d %H:%M", tp);
		((c_text*)cg->getComponent("update_time"))->text.assign(buf);

		cg->drawFrame(u8g2);
	}

	panel_weather::~panel_weather()
	{
		delete cg;
	}
}
