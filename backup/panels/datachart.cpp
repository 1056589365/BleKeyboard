/*
 * datachart.cpp
 *
 *  Created on: 2019年1月20日
 *      Author: Administrator
 */
#include <string>

#include "cbui.h"
#include "app.h"

#include "esp_system.h"
#include <cmath>

using namespace std;


int mapx(int x, int in_min, int in_max, int out_min, int out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

namespace app
{
	panel_datachart::panel_datachart(panel_datachart& arg) : panel(arg)
	{
		pt = arg.pt;
		dataLen = arg.dataLen;
		data = new int[dataLen];
		memcpy(data, arg.data, dataLen);
		cg = new c_componentGroup(0, 0, -1, -1);
		cg->sub_components.insert(arg.cg->sub_components.begin(), arg.cg->sub_components.end());
	}

	panel_datachart::panel_datachart(int dataSize, int (*pt)()): panel()
	{
		cg = new c_componentGroup(0, 0, -1, -1);
		dataLen = dataSize<0? 8:dataSize;
		data = new int[dataLen];
		this->pt = pt;

		for(int i=0;i<dataLen;i++)
		{
			data[i] = 1;
		}

		setFramePerSecond(20);

		cg->addComponent("title", (ui_component*)new c_text("title", 63, 5, 124, 13, u8g2_font_wqy12_t_gb2312, TEXT_ALIGNMENT_CENTER, SCROLL_DIRECTION_HORIZONTAL_LOOP));
		cg->addComponent("spline", (ui_component*)new c_line(7, 20, 120, 20));
		cg->addComponent("recting", (ui_component*)new c_rectangle(0, 0, 128, 64, 3));

		char temp[10];
		int sp = 126/(dataLen-1);//4 31

		for(int c=0;c<dataLen;c++)
		{
			sprintf(temp, "l%d", c);
			cg->addComponent(string(temp), (ui_component*)new c_line(127-4-sp*c, 63-3, 127-4-sp*c, 63-5-2*c));
		}

	}

	void panel_datachart::pushData(int pdata)
	{
		int swapcount = dataLen-1;
		for(int c=0;c<swapcount;c++)
		{
//			int a = data[swapcount-c];
//			int b = data[swapcount-c-1];
			data[swapcount-c]^=data[swapcount-c-1]^=data[swapcount-c]^=data[swapcount-c-1];
		}
		data[0] = pdata;
	}

	int panel_datachart::getSize()
	{
		return dataLen;
	}

	void panel_datachart::onResult(u8g2_t* u8g2, int result, int requestCode, panelManager* pm)
	{

	}

	void panel_datachart::onInput(u8g2_t* u8g2, panel_input parameters, panelManager* pm)
	{
		switch(parameters)
		{
			case PANEL_INPUT_RIGHT:
				finish();
				break;
			default:
				break;
		}
	}

	void panel_datachart::drawFrame(u8g2_t* u8g2, panelManager* pm)
	{
		if(pt!=NULL)
		{
			pushData(mapx(pt(), 0, 1200, 1, 42));
		}

		char temp[10];
		for(int c=0;c<dataLen;c++)
		{
			sprintf(temp, "l%d", c);
			((c_line*)cg->getComponent(string(temp)))->y2 = 63-5-data[c];
		}

		cg->drawFrame(u8g2);
	}

	panel_datachart::~panel_datachart()
	{
		delete cg;
	}
}
