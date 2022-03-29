#include <stdio.h>
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "Lcd.h"
#include <lwip/sockets.h>

//需要设置静态IP，必须知道连接的WIFI的网段
//#define ESP32_STATIC_IP   //是否使用静态IP

#ifdef ESP32_STATIC_IP 
//IP地址。
#define DEVICE_IP "192.168.1.199"
//网关地址
#define DEVICE_GW "192.168.1.1"
//掩码
#define DEVICE_NETMASK "255.255.255.0"
#endif



#define  DEFAULT_SSID "TP_LINK"        //需要连接的WIFI名称
#define  DEFAULT_PWD "12345678"   //wifi对应的密码

static ip4_addr_t s_ip_addr,s_gw_addr,s_netmask_addr;


//参数s　0：初始化，　1：连接中　　2：已连接
void lcd_display(int s)
{
  char lcd_buff[100]={0};

  Gui_DrawFont_GBK24(15,0,RED,WHITE,(u8 *)"悦为电子");
  LCD_P6x8Str(20,24,WHITE,BLACK,(u8 *)"taobao.com");
  Gui_DrawFont_GBK16(16,34,VIOLET,WHITE,(u8 *)"深圳悦为电子");
  Gui_DrawFont_GBK16(32,50,BLUE,WHITE,(u8 *)"欢迎您");

  //显示连接的wifi信息
  LCD_P6x8Str(0,70,BLACK,WHITE,(u8 *)"mode:STA");
  sprintf(lcd_buff, "ssid:%s",DEFAULT_SSID);
  LCD_P6x8Str(0,80,BLACK,WHITE,(u8 *)lcd_buff);
  sprintf(lcd_buff, "psw:%s",DEFAULT_PWD);
  LCD_P6x8Str(0,90,BLACK,WHITE,(u8 *)lcd_buff);

  if(2==s)
  {
    //2：已连接
    sprintf(lcd_buff, "ip:%s      ",ip4addr_ntoa(&s_ip_addr));
    LCD_P6x8Str(0,100,BLUE,WHITE,(u8 *)lcd_buff);        
    sprintf(lcd_buff, "gw:%s",ip4addr_ntoa(&s_gw_addr));
    LCD_P6x8Str(0,110,BLUE,WHITE,(u8 *)lcd_buff);        
    sprintf(lcd_buff, "mask:%s",ip4addr_ntoa(&s_netmask_addr));
    LCD_P6x8Str(0,120,BLUE,WHITE,(u8 *)lcd_buff);        
  }
  else
  {
    LCD_P6x8Str(0,100,RED,WHITE,(u8 *)"wifi connecting......");
  }
  
}

// wifi事件处理函数
// ctx     :表示传入的事件类型对应所携带的参数
// event   :表示传入的事件类型
// ESP_OK  : succeed
// 其他    :失败 
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
        case SYSTEM_EVENT_STA_START://STA启动
            printf("sta start.\r\n");
            //修改设备的名字
            ESP_ERROR_CHECK(tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, "sz-yy.taobao.com"));
            esp_wifi_connect();//开始连接wifi
            lcd_display(1);
            break;
        case SYSTEM_EVENT_STA_CONNECTED://STA已连接上
          printf("SYSTEM_EVENT_STA_CONNECTED.\r\n");
          break;
        case SYSTEM_EVENT_STA_GOT_IP://STA取得IP
            //通过LOG输出IP信息
            printf("\r\nip:%s.\r\n",ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
            printf("gw:%s.\r\n",ip4addr_ntoa(&event->event_info.got_ip.ip_info.gw));
            printf("netmask:%s.\r\n",ip4addr_ntoa(&event->event_info.got_ip.ip_info.netmask));
            //保存IP信息用于LCD显示
            s_ip_addr = event->event_info.got_ip.ip_info.ip;//保存IP
            s_gw_addr = event->event_info.got_ip.ip_info.gw;//保存网关
            s_netmask_addr = event->event_info.got_ip.ip_info.netmask;//保存掩码
            //LCD显示
            lcd_display(2);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED://STA断开连接
            esp_wifi_connect();//开始连接wifi
            lcd_display(1);
            break;
        default:
            break;
    }
    return ESP_OK;
}

//启动WIFI的STA
void wifi_init_sta()
{
    tcpip_adapter_init();//tcp/IP配置
#ifdef ESP32_STATIC_IP 
    //静态IP部分
    tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA);
    tcpip_adapter_ip_info_t ipInfo;
     
    inet_pton(AF_INET,DEVICE_IP,&ipInfo.ip);
    inet_pton(AF_INET,DEVICE_GW,&ipInfo.gw);
    inet_pton(AF_INET,DEVICE_NETMASK,&ipInfo.netmask);
    tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA,&ipInfo);
#endif

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));//设置wifi事件回调函数

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));//wifi默认初始化    
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));//NVS 用于存储 WiFi 数据.
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = DEFAULT_SSID,//连接的热点名称
            .password = DEFAULT_PWD,//连接的热点密码
            .scan_method = WIFI_ALL_CHANNEL_SCAN,
            .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
            .threshold.rssi = -127,
        },
    };    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));//设置wifi工作模式为STA
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));//配置AP参数    
    ESP_ERROR_CHECK(esp_wifi_start());    //使能wifi
}

//用户函数入口，相当于main函数
void app_main()
{
    ESP_ERROR_CHECK( nvs_flash_init() );//初始化NVS

    //显示屏初始化，以及界面提示
    Lcd_Init();
    lcd_display(0);

    //启动WIFI的STA
    wifi_init_sta();
}





