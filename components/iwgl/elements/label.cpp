#include <cmath>
#include <cstring>
#include "elements.h"

#define TAG __FILE__

extern "C" {

	bool containNumber(int* array, int len, int value);

	void splitWord(const string& word, vector<string>& chars);

	void copyU8G2Buffer(uint8_t* source, uint8_t* dist, uint16_t _vb, uint16_t _b, uint16_t _h, int rect_x1, int rect_x2, int rect_y1, int rect_y2);

	int getCalStringWidth(u8g2_t* u8g2, string& text);


	void Label::scroll_no(u8g2_t* u8g2, uint16_t maxCharHeight, uint16_t stringWidth)
	{
		switch(aliment)
		{
			case LEFT:
				u8g2_DrawUTF8(u8g2, 1+rect_x1, y+maxCharHeight, text.data());
				break;
			case CENTER:
				u8g2_DrawUTF8(u8g2, 1+(rect_x1+width/2)-stringWidth/2, y+maxCharHeight, text.data());
				break;
			case RIGHT:
				u8g2_DrawUTF8(u8g2, rect_x2-stringWidth, y+maxCharHeight, text.data());
				break;
		}
	}

	void Label::scroll_v_loop(u8g2_t* u8g2, uint16_t maxCharHeight, uint16_t stringWidth)
	{
		if(text.compare(lastText)!=0)
		{
			string temp;
			vector<string> words;
			displayTexts.clear();
			splitWord(text, words);

			int charWidth;
			int currWidth;

			for(int i=0;i<= words.size()-1;)
			{
				string word = words[i];

				charWidth = u8g2_GetUTF8Width(u8g2, word.data());
				currWidth = u8g2_GetUTF8Width(u8g2, temp.data());
				if(charWidth + currWidth < width-1){
					temp.append(word);
					i++;
				}else{
					displayTexts.push_back(temp);
					temp.clear();
				}
			}
			displayTexts.push_back(temp);
			vl_backPoint = -(displayTexts.size() * u8g2_GetMaxCharHeight(u8g2));
			vl_offY = abs(u8g2_GetDisplayHeight(u8g2)*0.8);

		}


		if(displayTexts.size() * u8g2_GetMaxCharHeight(u8g2) > height)
		{
			vl_offY -= 2;
		}else{
			vl_offY = 0;
		}

		if(vl_offY< vl_backPoint)
		{
			vl_offY = height;
		}

		int px, py;
		for(int c=0;c<displayTexts.size();c++)
		{
			px = 2 + rect_x1;
			py = c*12 + vl_offY + y+maxCharHeight;

			if(py>= 0 && py<= 128)
			{
				u8g2_DrawUTF8(u8g2, px, py, displayTexts[c].data());
			}
		}
	}

	void Label::scroll_h_flip(u8g2_t* u8g2, uint16_t maxCharHeight, uint16_t stringWidth)
	{
		uint16_t range = stringWidth - width + 1;

		if(text.compare(lastText)!=0)
			hl_offX = width*0.3;

		if(hf_dsing)
		{
			if(hf_scrollOffset>=range)
			{
				hf_dsing = !hf_dsing;
			}else{
				hf_scrollOffset += 1;
			}
		}else{
			if(hf_scrollOffset<0)
			{
				hf_dsing = !hf_dsing;
			}else{
				hf_scrollOffset -= 3;
			}
		}
		
		int xx = (-hf_scrollOffset) + rect_x1;
		int yy = y+maxCharHeight;

		u8g2_DrawUTF8(u8g2, xx, yy, text.data());
	}

	void Label::scroll_h_loop(u8g2_t* u8g2, uint16_t maxCharHeight, uint16_t stringWidth)
	{
		int backPotin = -stringWidth;

		if(text.compare(lastText)!=0)
			hl_offX = width*0.3;

		hl_offX -= 3;

		if(hl_offX< backPotin)
			hl_offX = width;

		int xx = hl_offX + rect_x1;
		int yy = y+maxCharHeight;
		
		u8g2_DrawUTF8(u8g2, xx, yy, text.data());
	}

	Label::Label(string text, int x, int y, int width, int height, const uint8_t* font, Alignment aliment, Direction scrolling, bool mask)
	{
		this->x = x;
		this->y = y;
		this->text = text;
		this->scrolling = scrolling;
		this->aliment = aliment;
		this->font = font;

		this->width = width;
		this->height = height;
		
		this->mask = mask;

		rect_x1 = 0;
		rect_y1 = 0;
		rect_x2 = 0;
		rect_y2 = 0;

		hl_offX = 0;
		hf_dsing = true;
		hf_scrollOffset = 0;
		vl_offY = 0;
		vl_backPoint = 0;
	}

	Label::Label()
	{
		
	}

	Label::~Label()
	{
		delete buf;
	}

	/*
	*
	*                 +------------------------------------------- tile width * 8 --------------------------------------------+                                                                              .
	*                 |                                                                                                       |                          .
	*                                                                                                                                                    .
	*                                                                                                                                                    .
	* tile height*8   +-------------------------------------------------------------------------------------------------------+                        .
	*   +-            |      0     1     2     3     4     5     6     7     8     9     10    11    12    13   ...   128     |                        .
	*   |             |    +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+    |           +---+          .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |           | t |         .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |           | i |        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |           | l |        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |           | e |        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |           |   |         .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |           |   |        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |           |   |        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |           |   |       .
	*   |             |    +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+    |           +---+       .
	*   |             |                                                                                                       |                           .
	*   |             |    +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+    |                       .
	*   |             |                                                                                                       |                          .                      .
	*   |             |    +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+    |                       .
	*   |             |                                                                                                       |                           .
	*   |             |    +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # | | # |    |                        .
	*   |             |    +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+    |                       .
	*   +             |                                                                                                       |                          .
	*                 +-------------------------------------------------------------------------------------------------------+                        .
	*                                                                                    .
	*                                                                                    .
	*     vb 8                                                                           .
	*                                                                                    .
	*                                                                                    .
	*/

	void Label::onElementDraw(u8g2_t* u8g2)
	{
		if(text.empty()) return;

		uint8_t* u8g2ptr;
		uint16_t h, b, vb, bufLen;

		h = u8g2_GetBufferTileWidth(u8g2);
		b = u8g2_GetBufferTileHeight(u8g2);
		vb = 8;
		bufLen = h*b*vb;
		u8g2ptr = u8g2_GetBufferPtr(u8g2);

		if(buf==nullptr) buf = new uint8_t[bufLen];


		if(mask)
		{
			memcpy(buf, u8g2ptr, bufLen);
			u8g2_ClearBuffer(u8g2);
		}
		
		u8g2_SetFont(u8g2, font);
		u8g2_SetFontPosBottom(u8g2);

		int maxCharHeight = u8g2_GetMaxCharHeight(u8g2);
		int stringWidth = u8g2_GetUTF8Width(u8g2, text.data());

		int calWidth = getCalStringWidth(u8g2, text);

		if(abs(calWidth-stringWidth) >13)
			stringWidth = calWidth;

		int x1_offset = 0;

		switch(aliment)
		{
			case LEFT:
				x1_offset = 0;
				break;
			case CENTER:
				x1_offset = (-width/2);
				break;
			case RIGHT:
				x1_offset = (-width);
				break;
		}
		rect_x1 = x + x1_offset;
		rect_y1 = y;
		rect_x2 = rect_x1 + width;
		rect_y2 = rect_y1 + height;

		
		if(flipColor)
		{
			u8g2_DrawRFrame(u8g2, rect_x1+1, rect_y1+1, width-1, height-1, 3);
		}

		if(flipColor)
		{
			//u8g2_SetDrawColor(u8g2, (u8g2->draw_color==1? 0:1));
		}


		if(stringWidth > width-1) // out range
		{
			switch(scrolling)
			{
				case NO_SCROLL:
					scroll_no(u8g2, maxCharHeight, stringWidth);
					break;
				case V_LOOP:
					scroll_v_loop(u8g2, maxCharHeight, stringWidth);
					break;
				case H_FLIP:
					scroll_h_flip(u8g2, maxCharHeight, stringWidth);
					break;
				case H_LOOP:
					scroll_h_loop(u8g2, maxCharHeight, stringWidth);
					break;
			}
		}else{
			scroll_no(u8g2, maxCharHeight, stringWidth);
		}

		if(flipColor)
		{
			//u8g2_SetDrawColor(u8g2, (u8g2->draw_color==1? 0:1));
		}

		if(mask)
		{

			int display_width = 8 * u8g2_GetBufferTileWidth(u8g2);
			int display_height = 8 * u8g2_GetBufferTileHeight(u8g2);
			
			int rotated_x1 = 0;
			int rotated_y1 = 0;
			int rotated_x2 = 0;
			int rotated_y2 = 0;

			if(u8g2->cb == U8G2_R0){
				rotated_x1 = rect_x1;
				rotated_y1 = rect_y1;
				rotated_x2 = rect_x2;
				rotated_y2 = rect_y2;
				//printf("u8g2 R0\n");
			}else if(u8g2->cb == U8G2_R1)
			{
				rotated_x1 = display_width-rect_y1-height;
				rotated_y1 = rect_x1;
				rotated_x2 = rotated_x1+height;
				rotated_y2 = rotated_y1+width;
				//printf("u8g2 R1\n");
			}else if(u8g2->cb == U8G2_R2)
			{
				rotated_x1 = rect_x1;
				rotated_y1 = display_height-rect_y2;
				rotated_x2 = rect_x2;
				rotated_y2 = display_height-rect_y1;
				//printf("u8g2 R2\n");
			}else if(u8g2->cb == U8G2_R3)
			{
				rotated_x1 = rect_y1;
				rotated_y1 = display_height-rect_x1-width;
				rotated_x2 = rotated_x1+height;
				rotated_y2 = rotated_y1+width;
				//printf("u8g2 R3\n");
			}else if(u8g2->cb == U8G2_MIRROR)
			{
				rotated_x1 = display_width-rect_x1;
				rotated_y1 = rect_y1;
				rotated_x2 = display_width-rect_x2;
				rotated_y2 = rect_y2;
				//printf("u8g2 MIRROR\n");
			}
			/*
			int W = (rect_x2-rect_x1);
			int H = (rect_y2-rect_y1);
			*/
		
			copyU8G2Buffer(buf, u8g2ptr, vb, b, h, rotated_x1, rotated_y1, rotated_x2, rotated_y2);
		}

		
		
		/*
		u8g2_DrawFrame(u8g2, rect_x1, rect_y1, W, H); // for debug
		printf("w: %d, h: %d\n", display_width, display_height);
		printf("u8g2 Rpoint1: %d:%d\n", rotated_x1, rotated_y1);
		printf("u8g2 Rpoint2: %d:%d\n", rotated_x2, rotated_y2);
		printf("u8g2 Spoint1: %d:%d\n", rect_x1, rect_y1);
		printf("u8g2 Spoint2: %d:%d\n", rect_x2, rect_y2);
		printf("u8g2 w / h: W: %d, H: %d\n", W, H);
		*/
		
		if(text.compare(lastText)!=0)
		{
			lastText.assign(text);
		}

		return;
	}


	bool containNumber(int* array, int len, int value)
	{
		for(int c=0;c<len;c++)
		{
			if(array[c]==value) return true;
		}
		return false;
	}

	void splitWord(const string& word, vector<string>& chars)
	{
		int num = word.size();
		int i = 0;
		while(i < num)
		{
			int size = 1;
			if(word[i] & 0x80)// is unicode char
			{
				char head = word[i];
				head <<= 1;
				do{
					head <<= 1;
					++size;
				}while(head & 0x80);
			}
			string subWord;
			subWord = word.substr(i, size);
			chars.push_back(subWord);
			i += size;
		}
	}

	inline void copyBit(uint8_t* source, uint8_t* dist, uint16_t index, uint8_t bit)
	{
		static uint8_t dot;
		dot = source[index]&(0x1<<bit)?1:0;
		if(dot)
		{
			dist[index] |= 0x00|(0x1<<bit);
		}else{
			dist[index] &= 0xFF-(0x1<<bit);
		}
	}

	void copyU8G2Buffer(uint8_t* source, uint8_t* dist, uint16_t _vb, uint16_t _b, uint16_t _h, int _rect_x1, int _rect_y1, int rect_x2, int _rect_y2)
	{
		int xx;
		int yy;
		
		if(_rect_x1 > rect_x2)
		{
			_rect_x1 += rect_x2;
			rect_x2 = _rect_x1 - rect_x2;
			_rect_x1 = _rect_x1 - rect_x2;
		}
		
		if(_rect_y1 > _rect_y2)
		{
			_rect_y1 += _rect_y2;
			_rect_y2 = _rect_y1 - _rect_y2;
			_rect_y1 = _rect_y1 - _rect_y2;
		}

		for(int vb=0;vb<_vb;vb++)
		for(int b=0;b<_b;b++)
		for(int h=0;h<_h*8;h++)
		{
			xx = h;
			yy = vb*8 + b;

			if(!(_rect_x1<xx && xx<rect_x2 && _rect_y1<yy && yy<_rect_y2))
			{
				//copyBit(source, dist, vb*128 + h, b);
				///*
				if(source[(vb*128 + h)]&(0x1<<b))
				{
					dist[(vb*128 + h)] |= 0x00|(0x1<<b);
				}else{
					dist[(vb*128 + h)] &= 0xFF-(0x1<<b);
				}
				//*/
			}
		}
	}

	int getCalStringWidth(u8g2_t* u8g2, string& text)
	{
		static vector<string> words;
		static int swidth;

		words.clear();
		swidth = 0;
		splitWord(text, words);

		for(int i=0;i<= words.size()-1;i++)
		{
			int dx = u8g2_GetUTF8Width(u8g2, words[i].data());
			if(dx!=0)
				swidth += dx;
			swidth++;
		}
		swidth--;

		return swidth;
	}

}
