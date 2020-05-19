#include "panels.h"

#include <esp_log.h>
#include <assert.h>

PanelManager::PanelManager(u8g2_t* u8g2)
{
	this->u8g2 = u8g2;
}

PanelManager::PanelManager(PanelManager& arg)
{
	ESP_LOGE("ErrorError", "it is calling the copy constructor!!!!");
	assert(false);
}

PanelManager::~PanelManager()
{
	while(!panelList.empty())
	{
		delete panelList.back();
		panelList.pop_back();
	}
}

void PanelManager::switchIn(Panel* panel)
{
	if(!panelList.empty() && getPanel(0)==panel)
		return;

	panelList.push_back(panel);
}

void PanelManager::switchOut()
{
	if(!panelList.empty())
	{
		delete panelList.back();
		panelList.pop_back();
	}
}

int PanelManager::getPanelCount()
{
	return panelList.size();
}

bool PanelManager::isEmpty()
{
	return panelList.empty();
}

int PanelManager::getFPSLimit()
{
	if(!isEmpty())
	{
		Panel* topPanel = getPanel(0);

		if(topPanel !=nullptr)
		{
			return topPanel->fpsLimit;
		}
	}
	
	return 0;
}

Panel* PanelManager::getPanel(int layer)
{
	return panelList.at(layer);
}

void PanelManager::drawFrame()
{
	if(!active) return;
	if(isEmpty()) return;
	
	Panel* panel = panelList.back();

	if(panel->releaseFlag)
	{
		map<string, string> results = panel->results;
		delete panel;
		panelList.pop_back();
		if(!panelList.empty())
		{
			panelList.back()->results = results;
			panelList.back()->onResult();
			drawFrame();
		}
	}else{
		panel->onDraw();
	}
}

void PanelManager::onUpdate(TickType_t deltaTime)
{
	if(!active) return;

	if(!isEmpty())
	{
		for (auto panel : panelList)
		{
			if(!(panel->releaseFlag))
			{
				panel->onUpdate(deltaTime);
			}
		}
	}
}

void PanelManager::setActive(bool active)
{
	this->active = active;
}

void PanelManager::input(Args parameters)
{
	if(!panelList.empty())
	{
		panelList.back()->onInput(parameters);
	}
}

int PanelManager::getDisplayWidth()
{
	return 8 * u8g2_GetBufferTileWidth(u8g2);
}

int PanelManager::getDisplayHeight()
{
	return 8 * u8g2_GetBufferTileHeight(u8g2);
}
