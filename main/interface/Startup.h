#include <vector>
#include <cmath>

#include <iwgl.h>

class Startup: public Panel
{
public:
	vector<string> texts;

	Startup(PM* pm);
	~Startup();

	void onDraw();

	void println(string text);
	void clear();
};

Startup::Startup(PM* pm) : Panel(pm)
{
	addElement("4",   new Label("", 0, 2,  120, 13, u8g2_font_wqy12_t_gb2312));
	addElement("3",   new Label("", 0, 14, 120, 13, u8g2_font_wqy12_t_gb2312));
	addElement("2",   new Label("", 0, 26, 120, 13, u8g2_font_wqy12_t_gb2312));
	addElement("1",   new Label("", 0, 38, 120, 13, u8g2_font_wqy12_t_gb2312));
	addElement("0",   new Label("", 0, 50, 120, 13, u8g2_font_wqy12_t_gb2312));
}

Startup::~Startup()
{

}

void Startup::onDraw()
{
	for(int i=0;i<min(5, (int)texts.size());i++)
	{
		((Label*)getElement(to_string(i)))->text = texts[i];
	}

	Panel::onDraw();
}

void Startup::println(string text)
{
	texts.insert(texts.begin(), text);

	if(texts.size()>5)
	{
		texts.pop_back();
	}
}

void Startup::clear()
{
	texts.clear();
}