#include "panels.h"

extern "C" {

	Panel::Panel(PM* pm)
	{
		this->pm = pm;
	}

	Panel::~Panel()
	{
		removeAllElement();
	}

	void Panel::exit()
	{
		releaseFlag = true;
	}

	void Panel::switchIn()
	{
		pm->switchIn(this);
	}

	void Panel::setResult(string key, int value)
	{
		setResult(key, to_string(value));
	}

	void Panel::setResult(string key, string value)
	{
		results.insert(pair<string, string>(key, value));
	}

	void Panel::setArgument(string key, int value)
	{
		setArgument(key, to_string(value));
	}

	void Panel::setArgument(string key, string value)
	{
		arguments.insert(pair<string, string>(key, value));
	}

	int  Panel::getArgumentAsInt(string key)
	{
		if(arguments.find(key)!=arguments.end())
		{
			return strtol(arguments[key].data(), nullptr, 10);
		}

		return -1;
	}

	string Panel::getArgumentAsString(string key)
	{
		if(arguments.find(key)!=arguments.end())
		{
			return arguments[key];
		}

		return "";
	}

	int  Panel::getResultAsInt(string key)
	{
		if(results.find(key)!=results.end())
		{
			return strtol(results[key].data(), nullptr, 10);
		}
		return -1;
	}

	string Panel::getResultAsString(string key)
	{
		if(results.find(key)!=results.end())
		{
			return results[key];
		}

		return "";
	}

	bool Panel::checkArgument(string key)
	{
		return arguments.find(key)!=arguments.end();
	}

	bool Panel::checkResult(string key)
	{
		return results.find(key)!=results.end();
	}


	void Panel::onDraw()
	{
		for(auto el : elements)
		{
			el.second->onElementDraw(pm->u8g2);
		}
	}

	void Panel::onUpdate(TickType_t deltaTime)
	{

	}

	void Panel::onResult()
	{

	}

	void Panel::onInput(Args parameters)
	{

	}

	Element* Panel::addElement(string name, Element* element)
	{
		if(getElement(name)==nullptr)
		{
			elements.insert(std::pair<string, Element*>(name, element));
		}

		return element;
	}

	Element* Panel::getElement(string name)
	{
		auto it = elements.find(name);

		if(it != elements.end())
		{
			return it->second;
		}

		return nullptr;
	}

	void Panel::removeElement(string name)
	{
		auto it = elements.find(name);

		if(it != elements.end())
		{
			delete it->second;

			elements.erase(it);
		}
	}

	void Panel::removeAllElement()
	{
		for(auto el : elements)
		{
			delete el.second;
			elements.erase(el.first);
			return;
		}
	}

}
