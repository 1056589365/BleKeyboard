#include "elements.h"

Line::Line(int x1, int y1, int x2, int y2)
{
	this->x1 = x1;
	this->y1 = y1;
	this->x2 = x2;
	this->y2 = y2;
}

Line::~Line()
{

}

void Line::onElementDraw(u8g2_t* u8g2)
{
	u8g2_DrawLine(u8g2, x1, y1, x2, y2);
}

