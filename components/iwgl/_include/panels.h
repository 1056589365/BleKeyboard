#pragma once

#include <string>
#include <map>
#include <vector>
#include <list>
#include "elements.h"
#include "freertos/FreeRTOS.h"


using namespace std;

class PanelManager;

typedef PanelManager PM;

enum class KeyCode
{
	ENTER,
	UP,
	DOWN,
	LEFT,
	RIGHT,
};

typedef KeyCode Args;

class Panel
{
public:
	map<string, Element*> elements;
	map<string, string> results;
	map<string, string> arguments;
	bool releaseFlag = false;
	int fpsLimit = 3;
	PM* pm = nullptr;

	Panel(PM* pm);
	virtual ~Panel();

	virtual void onDraw();
	virtual void onUpdate(TickType_t deltaTime);
	virtual void onResult();
	virtual void onInput(Args parameters);

	void exit();
	void switchIn();
	void setResult(string key, int value);
	void setResult(string key, string value);
	void setArgument(string key, int value);
	void setArgument(string key, string value);
	int  getArgumentAsInt(string key);
	string getArgumentAsString(string key);
	int  getResultAsInt(string key);
	string getResultAsString(string key);
	bool checkArgument(string key);
	bool checkResult(string key);
	
protected:
	Element* addElement(string name, Element* element);
	Element* getElement(string name);
	void removeElement(string name);
	void removeAllElement();
};

class PanelManager
{
public:
	vector<Panel*> panelList;
	u8g2_t* u8g2 = nullptr;
	bool active = true;

	PanelManager(u8g2_t* u8g2);
	PanelManager(PanelManager& arg);
	~PanelManager();

	void switchIn(Panel* panel);
	void switchOut();
	int getPanelCount();
	bool isEmpty();
	int getFPSLimit();
	Panel* getPanel(int layer);
	void drawFrame();
	void onUpdate(TickType_t deltaTime);
	void setActive(bool active);
	void input(Args parameters);
	int getDisplayWidth();
	int getDisplayHeight();
};


class ListMenu : public Panel
{
public:
	class Item
	{
	public:
		string text;

		Item(string text)
		{
			this->text = text;
		}
		
		virtual void onItemClick(ListMenu& panel);
		virtual void onItemUpdate(ListMenu& panel);
	};

	vector<Item> items;
	string title;
	
	int itemPoint = 0;
	bool longClickLeftExit = true;

	ListMenu(string title, PM* pm, bool longLeftExit=false);
	~ListMenu();

	void onDraw();
	void onUpdate(TickType_t deltaTime);
	void onResult();
	void onInput(Args parameters);

	void movePoint(int position);
	void addItem(Item item);
	vector<Item> getItems();
	int getPointerPosition();

private:
	void checkPointPosition();
};

class Dialog: public Panel
{
public:
	string title;
	string content;
	long duration = 0;
	long elapse = -1;

	Dialog(string title, string content, PM* pm, long duration=0);
	~Dialog();

	void onDraw();
	void onUpdate(TickType_t deltaTime);
	void onResult();
	void onInput(Args parameters);
};