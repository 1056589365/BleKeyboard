/*
 * notification.cpp
 *
 *  Created on: 2018年11月6日
 *      Author: shengYang
 */
#include "cbui.h"
#include "app.h"
#include "cbble.h"

using namespace cbble;

namespace app
{
	panel_notifiction::panel_notifiction(panel_notifiction& arg) : panel(arg)
	{
		un = arg.un;
		point = arg.point;

		cg = new c_componentGroup(0, 0, -1, -1);
		cg->sub_components.insert(arg.cg->sub_components.begin(), arg.cg->sub_components.end());
	}

	panel_notifiction::panel_notifiction() : panel()
	{
		un = &unreadNotifictions;

		cg = new c_componentGroup(0, 0, -1, -1);

		setFramePerSecond(8);

		cg->addComponent("title", (ui_component*)new c_text("", 63, 5, 124, 13, u8g2_font_wqy12_t_gb2312, TEXT_ALIGNMENT_CENTER, SCROLL_DIRECTION_HORIZONTAL_LOOP));
		cg->addComponent("spline", (ui_component*)new c_line(7, 20, 120, 20));
		cg->addComponent("content", (ui_component*)new c_text("", 63, 23, 124, 13*3, u8g2_font_wqy12_t_gb2312, TEXT_ALIGNMENT_CENTER, SCROLL_DIRECTION_VERTIAL_LOOP));
		cg->addComponent("tip", (ui_component*)new c_text("", 122, 50, 31, 13, u8g2_font_wqy12_t_gb2312, TEXT_ALIGNMENT_RIGHT, SCROLL_DIRECTION_HORIZONTAL_LOOP));
		cg->addComponent("recting", (ui_component*)new c_rectangle(0, 0, 128, 64, 3));

//		(cg->getComponent("tip"))->setActive(false);
	}

	void panel_notifiction::onResult(u8g2_t* u8g2, int result, int requestCode, panelManager* pm)
	{
		//finish();
	}

	void panel_notifiction::onInput(u8g2_t* u8g2, panel_input parameters, panelManager* pm)
	{
		switch(parameters)
		{
			case PANEL_INPUT_UP:
				point--;
				if(point<0) point = 0;
				break;
			case PANEL_INPUT_DOWN:
				point++;
				if(point>=un->size()) point = un->size()-1;
				break;
			case PANEL_INPUT__ENTER:
			{
				list<app::notification>::iterator it = un->begin();
				for(int c=0;c<point;c++) it++;
				un->erase(it);
				break;
			}
			case PANEL_INPUT_LEFT:
			case PANEL_INPUT_ENTER:
				finish();
				break;
			default:
				break;
		}

	}

	void panel_notifiction::drawFrame(u8g2_t* u8g2, panelManager* pm)
	{
		if(un->size()==0)
		{
//			panel_dialog_argument pda("已经没有更多通知", "任意键退出");
//			pm->switchedIn((panel*)new panel_dialog(pda));
			finish();
		}else{

			if(hasNewNotification && point==un->size()-2)
			{
				point++;
			}

			if(point<0) point = 0;
			if(point>=un->size()) point = un->size()-1;
//			ESP_LOGI("APP", "size:%d, point:%d", arg.unreadNotifitions->size(), point);
			list<notification>::iterator it = un->begin();
			for(int c=0;c<point;c++) it++;
			notification ntf = *it;

			((c_text*)cg->getComponent("title"))->text.assign(ntf.title);
			((c_text*)cg->getComponent("content"))->text.assign(ntf.content);

			char temp[32];
			sprintf(temp, "%d/%d", point+1, un->size());

			((c_text*)cg->getComponent("tip"))->text.assign(temp);

			((ui_component*)cg->getComponent("recting"))->active = ble_isConnected();

			cg->drawFrame(u8g2);

		}

		hasNewNotification = false;
	}

	panel_notifiction::~panel_notifiction()
	{
		delete cg;
	}

}
