/*
 * home.cpp
 *
 *  Created on: 2018年11月6日
 *      Author: shengYang
 */
#include <cstring>
#include <cmath>
#include "esp_bt_main.h"
#include "esp_bt.h"
#include "esp_bt_device.h"
#include "freertos/FreeRTOS.h"
#include <time.h>

#include "cbui.h"
#include "cbble.h"
#include "app.h"

using namespace cbble;
using namespace cbui::cbpanel;

#define AF_ITEM [](string& text, int index, list<panel_list_item>& lisT, panelManager* pm, cbui::cbpanel::panel_list* panel_list)->bool
#define AF_PANEL_ONUPDATE [](string& title, int index, list<panel_list_item>& lisT, panelManager* pm, cbui::cbpanel::panel_list* panel_list)
#define switchin(pla) pm->switchedIn((panel*)new cbui::cbpanel::panel_list(pla))
#define RT_FALSE return false;
#define RT_TRUE return true;


namespace app
{
	panel_home::panel_home(panel_home& arg) : panel(arg)
	{
		u8g2 = arg.u8g2;

		cg = new c_componentGroup(0, 0, -1, -1);
		cg->sub_components.insert(arg.cg->sub_components.begin(), arg.cg->sub_components.end());
	}

	panel_home::panel_home() : panel()
	{
		u8g2 = NULL;
		cg = new c_componentGroup(0, 0, -1, -1);
		setFramePerSecond(10);

//		frame.addComponent("voltage", (ui_component*)new c_text("", 1, 1, 70, u8g2_font_wqy12_t_gb2312, TEXT_ALIGNMENT_LEFT, SCROLL_DIRECTION_HORIZONTAL_FLIP));
//		frame.addComponent("btstate", (ui_component*)new c_text("", 1, 14, 90, 13, u8g2_font_wqy12_t_gb2312, TEXT_ALIGNMENT_LEFT, SCROLL_DIRECTION_HORIZONTAL_FLIP));
		cg->addComponent("notify", (ui_component*)new c_text("", 70, 35, 90, 13, u8g2_font_wqy12_t_gb2312, TEXT_ALIGNMENT_LEFT, SCROLL_DIRECTION_HORIZONTAL_FLIP));
		cg->addComponent("recting", (ui_component*)new c_line(52, 63, 75, 63));
//		frame.addComponent("frame", (ui_component*)new c_line(0, 48, 127, 48));
//		frame.addComponent("desc", (ui_component*)new c_text("Home地", 63, 30, 32, 13, u8g2_font_wqy12_t_gb2312, TEXT_ALIGNMENT_CENTER, SCROLL_DIRECTION_VERTIAL_LOOP));
//		frame.addComponent("desc", (ui_component*)new c_text("↑天气 ○菜单/锁屏 ↓通知", 63, 50, 120, u8g2_font_wqy12_t_gb2312, TEXT_ALIGNMENT_CENTER, SCROLL_DIRECTION_HORIZONTAL_LOOP));
	}

	void panel_home::onResult(u8g2_t* u8g2, int result, int requestCode, panelManager* pm)
	{

	}

	void panel_home::onInput(u8g2_t* u8g2, panel_input parameters, panelManager* pm)
	{
		if(!active)
		{
			if(parameters==PANEL_INPUT_ENTER)
				setActive(true);
			return;
		}

		switch(parameters)
		{
			case PANEL_INPUT_LEFT:
				ESP_LOGI(tagx, "LEFT down!");
				break;
			case PANEL_INPUT_RIGHT:
				ESP_LOGI(tagx, "RIGHT down!");
				break;
			case PANEL_INPUT_DOWN:
			{
				panel_notifiction* pn = new panel_notifiction();
				pm->switchedIn((panel*)pn);
				break;
			}
			case PANEL_INPUT_ENTER:
			{

				plar pla;
				pla.title = "菜单";
				pla.onPanelUpdate = AF_PANEL_ONUPDATE{
					static char t[10];
					sprintf(t, "%d kb", esp_get_free_heap_size()/1024);
					title.assign("菜单 ");
					title.append(t);
				};
				pla.lisT.push_back(plitem("后退", AF_ITEM{RT_FALSE}, NULL));

				pla.lisT.push_back(plitem("TouchPad测试", AF_ITEM
				{
					plar pla;
					pla.title = "TouchPad测试";

					pla.lisT.push_back(plitem("后退", AF_ITEM{RT_FALSE}, NULL));

					pla.lisT.push_back(plitem("Touch3", AF_ITEM{
						panel_datachart* pdc = new panel_datachart(32, []()->int{
							static uint16_t touch_value;
							touch_pad_read(TOUCH_PAD_NUM3, &touch_value);
							return touch_value;
						});
						((c_text*) ((panel*)pdc)->cg->getComponent("title"))->text.assign("Touch3");

						pm->switchedIn((panel*)pdc);
						RT_TRUE
					}, AF_ITEM{
						text.assign("Touch3 ");
						static char t[10];
						static uint16_t touch_value;
						touch_pad_read(TOUCH_PAD_NUM3, &touch_value);
						sprintf(t, "%d", touch_value);
						text.append(t);
						RT_TRUE
					}));
					pla.lisT.push_back(plitem("Touch4", AF_ITEM{
						panel_datachart* pdc = new panel_datachart(32, []()->int{
							static uint16_t touch_value;
							touch_pad_read(TOUCH_PAD_NUM4, &touch_value);
							return touch_value;
						});
						((c_text*) ((panel*)pdc)->cg->getComponent("title"))->text.assign("Touch4");

						pm->switchedIn((panel*)pdc);
						RT_TRUE
					}, AF_ITEM{
						text.assign("Touch4 ");
						static char t[10];
						static uint16_t touch_value;
						touch_pad_read(TOUCH_PAD_NUM4, &touch_value);
						sprintf(t, "%d", touch_value);
						text.append(t);
						RT_TRUE
					}));
					pla.lisT.push_back(plitem("Touch5", AF_ITEM{
						panel_datachart* pdc = new panel_datachart(32, []()->int{
							static uint16_t touch_value;
							touch_pad_read(TOUCH_PAD_NUM5, &touch_value);
							return touch_value;
						});
						((c_text*) ((panel*)pdc)->cg->getComponent("title"))->text.assign("Touch5");

						pm->switchedIn((panel*)pdc);
						RT_TRUE
					}, AF_ITEM{
						text.assign("Touch5 ");
						static char t[10];
						static uint16_t touch_value;
						touch_pad_read(TOUCH_PAD_NUM5, &touch_value);
						sprintf(t, "%d", touch_value);
						text.append(t);
						RT_TRUE
					}));
					pla.lisT.push_back(plitem("Touch6", AF_ITEM{
											panel_datachart* pdc = new panel_datachart(32, []()->int{
												static uint16_t touch_value;
												touch_pad_read(TOUCH_PAD_NUM6, &touch_value);
												return touch_value;
											});
											((c_text*) ((panel*)pdc)->cg->getComponent("title"))->text.assign("Touch6");

											pm->switchedIn((panel*)pdc);
											RT_TRUE
										}, AF_ITEM{
											text.assign("Touch6 ");
											static char t[10];
											static uint16_t touch_value;
											touch_pad_read(TOUCH_PAD_NUM6, &touch_value);
											sprintf(t, "%d", touch_value);
											text.append(t);
											RT_TRUE
										}));
					pla.lisT.push_back(plitem("Touch7", AF_ITEM{
											panel_datachart* pdc = new panel_datachart(32, []()->int{
												static uint16_t touch_value;
												touch_pad_read(TOUCH_PAD_NUM7, &touch_value);
												return touch_value;
											});
											((c_text*) ((panel*)pdc)->cg->getComponent("title"))->text.assign("Touch7");

											pm->switchedIn((panel*)pdc);
											RT_TRUE
										}, AF_ITEM{
											text.assign("Touch7 ");
											static char t[10];
											static uint16_t touch_value;
											touch_pad_read(TOUCH_PAD_NUM7, &touch_value);
											sprintf(t, "%d", touch_value);
											text.append(t);
											RT_TRUE
										}));

					panel_list->setItemPointPosition(0);
					switchin(pla);

					RT_TRUE
				}, NULL));


				pla.lisT.push_back(plitem("蓝牙控制器", AF_ITEM
				{
					plar pla;
					pla.onPanelUpdate = AF_PANEL_ONUPDATE{title.assign("蓝牙控制器");title.append(ble_isConnected()?"#":"×");};
					pla.lisT.push_back(plitem("后退", AF_ITEM{RT_FALSE}, NULL));
					pla.lisT.push_back(plitem("启用/禁用控制器", AF_ITEM
					{
						esp_bluedroid_status_t status = esp_bluedroid_get_status();
						if(status==ESP_BLUEDROID_STATUS_INITIALIZED)ble_enable();
						else if(status==ESP_BLUEDROID_STATUS_ENABLED)ble_disable();
						RT_TRUE
					}, AF_ITEM{
						esp_bluedroid_status_t status=esp_bluedroid_get_status();
						if(status==ESP_BLUEDROID_STATUS_UNINITIALIZED)text.assign("控制器未初始化");
						else if(status==ESP_BLUEDROID_STATUS_INITIALIZED)text.assign("启用控制器");
						else if(status==ESP_BLUEDROID_STATUS_ENABLED)text.assign("禁用控制器");
						else text.assign("未知控制器状态");
						RT_TRUE
					}));

					pla.lisT.push_back(plitem("Mac Address", NULL, AF_ITEM{
						static char t[20];

						esp_bluedroid_status_t status = esp_bluedroid_get_status();
						if(status==ESP_BLUEDROID_STATUS_ENABLED)
						{
							const uint8_t* t2 = esp_bt_dev_get_address();
							if(t2!=NULL)
							{
								sprintf(t,"%X-%X-%X-%X-%X-%X", t2[0], t2[1], t2[2], t2[3], t2[4], t2[5]);
								text.assign(t);
							}else{
								text.assign("未启用蓝牙堆栈");
							}
						}else{
							text.assign("bluedroid未启用");
						}
						RT_TRUE
					}));

					switchin(pla);
					RT_TRUE
				}, NULL));


				pla.lisT.push_back(plitem("背光", AF_ITEM
				{
					plar pla;
					pla.title = "背光";
					pla.onPanelUpdate = AF_PANEL_ONUPDATE{
						title.assign("背光 - ");
						static char t[10];
						sprintf(t,"%d", backgroundLight*100/265);
						title.append(t);
					};
					pla.lisT.push_back(plitem("后退", AF_ITEM{RT_FALSE}, NULL));
					pla.lisT.push_back(plitem("to 0%",  AF_ITEM{backgroundLight = 1;RT_TRUE}, NULL));
					pla.lisT.push_back(plitem("to 25%", AF_ITEM{backgroundLight = 64;RT_TRUE}, NULL));
					pla.lisT.push_back(plitem("to 50%", AF_ITEM{backgroundLight = 128;RT_TRUE}, NULL));
					pla.lisT.push_back(plitem("to 75%", AF_ITEM{backgroundLight = 192;RT_TRUE}, NULL));
					pla.lisT.push_back(plitem("to 100%", AF_ITEM{backgroundLight = 255;RT_TRUE}, NULL));
					switchin(pla);
					RT_TRUE
				}, AF_ITEM{
					text.assign("背光设置");
					static char t[10];
					sprintf(t,"[%d]", backgroundLight*100/265);
					text.append(t);
					RT_TRUE
				}));

				pla.lisT.push_back(plitem("通知", AF_ITEM
				{
					plar pla;
					pla.title = "通知";
					pla.onPanelUpdate = AF_PANEL_ONUPDATE{
						title.assign("通知 - ");
						static char t[10];
						sprintf(t,"%d", unreadNotifictions.size());
						title.append(t);
					};;
					pla.lisT.push_back(plitem("后退", AF_ITEM{RT_FALSE}, NULL));
					pla.lisT.push_back(plitem("清理所有通知", AF_ITEM{unreadNotifictions.clear();RT_TRUE}, NULL));
					pla.lisT.push_back(plitem("主界面自动弹出通知", AF_ITEM{
						autoShowNotification = !autoShowNotification;
						saveASN(autoShowNotification);

						RT_TRUE
					}, AF_ITEM{
						text.assign("自动弹出通知");
						static char t[10];
						sprintf(t,"[%s]", autoShowNotification?"启用":"禁用");
						text.append(t);
						RT_TRUE}));
					switchin(pla);
					RT_TRUE
				}, AF_ITEM{
					text.assign("通知");
					static char t[10];
					sprintf(t,"[%d]", unreadNotifictions.size());
					text.append(t);
					RT_TRUE
				}));



				/*
				pla.lisT.push_back(plitem("WIFI_STA", AF_ITEM
				{
					plar pla;
					pla.title = "WIFI菜单";
					pla.lisT.push_back(plitem("后退", AF_ITEM{RT_FALSE}, NULL));
					pla.lisT.push_back(plitem("扫描", AF_ITEM{
						static wifi_scan_config_t config;
						config.show_hidden = true;
						config.scan_type = WIFI_SCAN_TYPE_ACTIVE;
						config.scan_time.active.min = 10;
						config.scan_time.active.max = 600;
						wifi_scaning = true;

						ESP_ERROR_CHECK(esp_wifi_scan_start(&config, false));
						RT_TRUE
					}, AF_ITEM{
						text.assign("扫描");
						static bool temp;
						if(wifi_scaning){
							text.append(temp?"/":"\\");
							temp = !temp;
						}
						RT_TRUE
					}));
					pla.lisT.push_back(plitem("AP列表", AF_ITEM{
						plar pla;
						pla.onPanelUpdate = OUD_AF{
							title.assign(wifi_scaning?"正在扫描..":"AP列表");
							lisT.clear();
							lisT.push_back(plitem("后退", AF_ITEM{RT_FALSE}, NULL));
							if(!wifi_scaning)
							{
								for(list<wifi_ap_record_t>::iterator it=aps.begin();it!=aps.end();it++)
								{
									wifi_ap_record_t api = *it;
									char t[128];
									char auth_mode[10];

									switch(api.authmode)
									{
										case WIFI_AUTH_OPEN:
											memcpy(auth_mode, "Open", 5);
											break;
										case WIFI_AUTH_WEP:
											memcpy(auth_mode, "WEP ", 5);
											break;
										case WIFI_AUTH_WPA_PSK:
											memcpy(auth_mode, "WPAP", 5);
											break;
										case WIFI_AUTH_WPA2_PSK:
											memcpy(auth_mode, "WP2P", 5);
											break;
										case WIFI_AUTH_WPA_WPA2_PSK:
											memcpy(auth_mode, "WW2P", 5);
											break;
										case WIFI_AUTH_WPA2_ENTERPRISE:
											memcpy(auth_mode, "Ente", 5);
											break;
										default:
											memcpy(auth_mode, "Unkn", 5);
											break;
									}
									sprintf(t, "%s|%2d|%16s|", auth_mode, api.rssi, api.ssid);
									lisT.push_back(plitem(string(t), AF_ITEM{RT_TRUE}, NULL));
								}
							}//!wifi_scaning
						};//onupdate
						switchin(pla);

						RT_TRUE
					}, NULL));

					ui->switchedIn((panel*)new panel_list(pla));
					RT_TRUE
				}, AF_ITEM{
					text.assign("WIFI");
					wifi_mode_t mode = WIFI_MODE_NULL;
					esp_err_t err = esp_wifi_get_mode(&mode);
					if(err==ESP_OK) {
						if(mode==WIFI_MODE_STA) text.append("(STA)");
						else if(mode==WIFI_MODE_AP) text.append("(AP)");
						else if(mode==WIFI_MODE_APSTA) text.append("(APSTA)");
						else text.append("(UkonwMode)");
					} else if(err==ESP_ERR_WIFI_NOT_INIT) text.append("(NotInit)");
					else if(err==ESP_ERR_INVALID_ARG) text.append("(ErrArg)");
					else text.append("(Unknow)");
					RT_TRUE
				}));

*/

//				pla.lisT.push_back(plitem("example", AF_ITEM
//				{
//					plar pla;
//					pla.onPanelUpdate = OUD_AF{;};
//					pla.lisT.push_back(plitem("后退", AF_ITEM{RT_FALSE}, AF_ITEM{RT_TRUE}));
//					ui->switchedIn((panel*)new panel_list(pla));
//					RT_TRUE
//				}, AF_ITEM{RT_TRUE}));




//				pla.list.push_back("电源设置");
//				pla.list.push_back("参数设置");
//				pla.list.push_back("系统信息");
//				pla.list.push_back("高级功能");
				panel_list* pl = new panel_list(pla);
				((panel*)pl)->argumentCode = 90;
				pm->switchedIn((panel*)pl);

				break;
			}
			case PANEL_INPUT__ENTER:
				setActive(false);
				break;
			default:
				break;
		}

	}

	void draw_clock(int raduis, int hour, int minute, int second, u8g2_t* u8g2);

	void panel_home::drawFrame(u8g2_t* u8g2, panelManager* pm)
	{
		static char buf[64];

		sprintf(buf, "%d", unreadNotifictions.size());
		((c_text*)cg->getComponent("notify"))->text.assign(buf);

		cg->getComponent("recting")->active = ble_isConnected();

		cg->drawFrame(u8g2);

		static int p1=0;
		static int p2=0;
		static int p3=0;

		if(p1++>59)
		{
			p1=0;
			if(p2++>59)
			{
				p2=0;
				if(p3++>23)
				{
					p1=0;
					p2=0;
					p3=0;
				}
			}
		}

		draw_clock(30, p3, p2, p1, u8g2);

		if(autoShowNotification && hasNewNotification)
		{
			panel_notifiction* pn = new panel_notifiction();
			pn->point = unreadNotifictions.size()-1;
			pm->switchedIn((panel*)pn);
		}
	}

	long map(long x, long in_min, long in_max, long out_min, long out_max)
	{
	  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

	void draw_clock(int raduis, int hour, int minute, int second, u8g2_t* u8g2)
	{
		bool flag_show_all_info = false;

		uint8_t center_x = flag_show_all_info?31:64;
		uint8_t center_y = 32;

		uint8_t pointer_length_h = raduis-11;
		uint8_t pointer_length_m = raduis-4;
		uint8_t pointer_length_s = raduis-2;

		if(!flag_show_all_info)
		{
			pointer_length_h = raduis-7;
			pointer_length_m = raduis-1;
			pointer_length_s = raduis-2;
		}

		uint8_t pointer_radian_h = map(hour*10+((float)minute/60)*10, 0, 11*10+0.9*10, 0, 360)*M_PI/180*10;
		uint8_t pointer_radian_m = map(minute, 0, 59, 0, 360)*M_PI/180*10;
		uint8_t pointer_radian_s = map(second, 0, 59, 0, 360)*M_PI/180*10;

		uint8_t ponter_h_x = sin((float)pointer_radian_h/10)*pointer_length_h;
		uint8_t ponter_h_y = sin(1.5*M_PI-(float)pointer_radian_h/10)*pointer_length_h;
		uint8_t ponter_m_x = sin((float)pointer_radian_m/10)*pointer_length_m;
		uint8_t ponter_m_y = sin(1.5*M_PI-(float)pointer_radian_m/10)*pointer_length_m;
		uint8_t ponter_s_x = sin((float)pointer_radian_s/10)*pointer_length_s;
		uint8_t ponter_s_y = sin(1.5*M_PI-(float)pointer_radian_s/10)*pointer_length_s;

		//指针线
		u8g2_DrawLine(u8g2, center_x, center_y, center_x+ponter_h_x, center_y+ponter_h_y);
		u8g2_DrawLine(u8g2, center_x, center_y, center_x+ponter_m_x, center_y+ponter_m_y);

		//OLED_COMPLETE_CLOCK
		u8g2_DrawLine(u8g2, center_x, center_y, center_x+ponter_s_x, center_y+ponter_s_y);

		//指针头小圆
		// u8g.drawCircle(center_x+ponter_h_x, center_y+ponter_h_y, 2);
		// u8g.drawCircle(center_x+ponter_m_x, center_y+ponter_m_y, 2);

		//秒针
		u8g2_DrawCircle(u8g2, center_x+ponter_s_x, center_y+ponter_s_y, 2, U8G2_DRAW_ALL);

		//大外圈
		u8g2_DrawCircle(u8g2, center_x, center_y, raduis, U8G2_DRAW_ALL);

		//转轴
		u8g2_SetDrawColor(u8g2, 0);
		u8g2_DrawDisc(u8g2, center_x, center_y, 4, U8G2_DRAW_ALL);
		u8g2_SetDrawColor(u8g2, 1);

		if(!flag_show_all_info)
		{
			u8g2_DrawPixel(u8g2, center_x, center_y);
			u8g2_DrawCircle(u8g2, center_x, center_y, 2, U8G2_DRAW_ALL);
			u8g2_DrawPixel(u8g2, center_x, center_y);
		}
	}


	panel_home::~panel_home()
	{
		delete cg;
	}
}
