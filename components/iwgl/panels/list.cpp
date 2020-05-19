#include "panels.h"
#include <stdio.h>
#include "esp_log.h"

ListMenu::ListMenu(string title, PM* pm, bool longLeftExit) : Panel(pm)
{
	this->title = title;

	addElement("title", new Label("title", 63, 0, 120, 13,    u8g2_font_wqy12_t_gb2312, Label::CENTER, Label::H_FLIP));
	addElement("item0", new Label("item0", 1+63, 14, 120, 13, u8g2_font_wqy12_t_gb2312, Label::CENTER, Label::H_LOOP));
	addElement("item1", new Label("item1", 1+63, 26, 120, 13, u8g2_font_wqy12_t_gb2312, Label::CENTER, Label::H_LOOP));
	addElement("item2", new Label("item2", 1+63, 38, 120, 13, u8g2_font_wqy12_t_gb2312, Label::CENTER, Label::H_LOOP));
	addElement("item3", new Label("item3", 1+63, 50, 120, 13, u8g2_font_wqy12_t_gb2312, Label::CENTER, Label::H_LOOP));
}

ListMenu::~ListMenu()
{

}

void ListMenu::movePoint(int position)
{
	itemPoint = position;
}

void ListMenu::addItem(ListMenu::Item item)
{
	items.push_back(item);
}

vector<ListMenu::Item> ListMenu::getItems()
{
	return items;
}

int ListMenu::getPointerPosition()
{
	return itemPoint;
}

void ListMenu::onInput(Args parameters)
{
	switch(parameters)
	{
		case KeyCode::UP:
			itemPoint--;
			if(itemPoint<0) itemPoint = 0;
			break;
		case KeyCode::DOWN:
			itemPoint++;
			if(itemPoint>=items.size()) itemPoint = items.size()-1;
			break;
		case KeyCode::RIGHT:
		case KeyCode::ENTER:
		{
			items[itemPoint].onItemClick(*this);

			break;
		}
		case KeyCode::LEFT:
			if(longClickLeftExit)
				exit();
			break;
		default:
			break;
	}
}

void ListMenu::onResult()
{

}

void ListMenu::onDraw()
{
	// 限制item point的位置
	checkPointPosition();
	
	//为可能的返回做好准备
	setResult("pointPosition", itemPoint);

	int point_page  = itemPoint/4;
	int point_index = itemPoint%4;

	for(int i=0;i<4;i++)
	{
		auto idx = i + point_page*4;
		Label* item = (Label*)getElement("item" + std::to_string(i));
		
		if(idx<=items.size()-1)
		{
			auto it = items[idx];

			// 让屏幕上每个菜单选项都绘制一下
			it.onItemUpdate(*this);

			item->text = it.text;
		}else{
			item->text.clear();
		}
	}

	string selectedOptionName = "item" + std::to_string(point_index);

	((Label*)getElement("title"))->text = title;

	u8g2_t* u8g2 = pm->u8g2;


	// 翻转颜色
	for(auto el: elements)
	{
		if(el.first.find("item")!=string::npos)
		{
			((Label*)el.second)->flipColor = !(el.first.compare(selectedOptionName));
		}

		el.second->onElementDraw(u8g2);
	}

	// 绘制滚动条
	if(items.size()>0)
	{
		int totalLength = 49;
		int stepLength = totalLength / items.size();
		int startPoint = 15+(stepLength * itemPoint);
		u8g2_DrawVLine(u8g2, 126, startPoint, stepLength);
	}

	Panel::onDraw();
}

void ListMenu::onUpdate(TickType_t deltaTime)
{

}

void ListMenu::checkPointPosition()
{
	if(itemPoint<0) itemPoint = 0;
	if(itemPoint>=items.size()) itemPoint = items.size()-1;
}



void ListMenu::Item::onItemClick(ListMenu& panel)
{

}

void ListMenu::Item::onItemUpdate(ListMenu& panel)
{

}