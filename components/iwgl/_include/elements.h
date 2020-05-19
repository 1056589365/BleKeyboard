#pragma once

#include <string>
#include <vector>
#include "u8g2.h"

using namespace std;

class Element
{
public:
	bool active;

	Element();
	virtual ~Element();

	void virtual onElementDraw(u8g2_t* u8g2) = 0;
};

class Label : public Element
{
public:
	enum Direction { NO_SCROLL, V_LOOP, H_FLIP, H_LOOP };
	enum Alignment { LEFT, CENTER, RIGHT };

	int x;
	int y;
	string text;
	string lastText;
	const uint8_t* font;
	int width;
	int height;
	Direction scrolling;
	Alignment aliment;
	bool flipColor = false;
	bool mask = false;

	int rect_x1;
	int rect_y1;
	int rect_x2;
	int rect_y2;

	int hl_offX;
	bool hf_dsing;
	int hf_scrollOffset;
	vector<string> displayTexts;
	int vl_offY;
	int vl_backPoint;

	Label(string text, int x, int y, int width, int height,const uint8_t* font, Alignment aliment=LEFT, Direction scrolling=NO_SCROLL, bool mask=false);
	Label();
	~Label();

	void onElementDraw(u8g2_t* u8g2);
	


private:
	uint8_t* buf = nullptr;

	void restore_buf  (uint8_t* u8g2ptr, uint16_t _vb, uint16_t _b, uint16_t _h);
	void scroll_no    (u8g2_t* u8g2, uint16_t maxCharHeight, uint16_t stringWidth);
	void scroll_v_loop(u8g2_t* u8g2, uint16_t maxCharHeight, uint16_t stringWidth);
	void scroll_h_flip(u8g2_t* u8g2, uint16_t maxCharHeight, uint16_t stringWidth);
	void scroll_h_loop(u8g2_t* u8g2, uint16_t maxCharHeight, uint16_t stringWidth);
};


class Line : public Element
{
public:
	int x1;
	int y1;
	int x2;
	int y2;

	Line(int x1, int y1, int x2, int y2);
	~Line();

	void onElementDraw(u8g2_t* u8g2);
};

class Rectangle : public Element
{
public:
	int x;
	int y;
	int w;
	int h;
	int radius;

	Rectangle(int x, int y, int w, int h, int radius);
	~Rectangle();

	void onElementDraw(u8g2_t* u8g2);
};