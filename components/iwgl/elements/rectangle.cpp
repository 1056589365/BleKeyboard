#include "elements.h"

Rectangle::Rectangle(int x, int y, int w, int h, int radius)
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	this->radius = radius;
}

void Rectangle::onElementDraw(u8g2_t* u8g2)
{
	u8g2_DrawRFrame(u8g2, x, y, w, h, radius);
}

Rectangle::~Rectangle()
{

}
