#include <iwgl.h>
#include <Menu.h>

extern BleKeyboard bleKeyboard;

class Home : public Panel
{
public:
    Home(PM* pm) : Panel(pm)
    {
        qadc_config_io(ADC1_GPIO34_CHANNEL);
        addElement("blestate", new Label("blestate", 63, 1, 120, 13, u8g2_font_wqy12_t_gb2312, Label::CENTER));
        addElement("powervoltage", new Label("powervoltage", 63, 20, 120, 13, u8g2_font_wqy12_t_gb2312, Label::CENTER));

        addElement("center-option", new Label("菜单", 63, 63-13, 100, 13, u8g2_font_wqy12_t_gb2312, Label::CENTER));
    }

    void onDraw()
    {
        ((Label*)getElement("powervoltage"))->text = "电池电压: "+to_string((int)(qadc_get_voltage(ADC1_GPIO34_CHANNEL, 2)/0.161538461538))+"mV";
        ((Label*)getElement("blestate"))->text = bleKeyboard.isConnected()? "已连接":"等待.";

        

        Panel::onDraw();
    }

	void onInput(Args parameters)
    {
        switch (parameters)
        {
        case KeyCode::UP:
            (new Menu(pm))->switchIn();
            break;
        
        default:
            break;
        }
    }

};