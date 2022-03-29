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

//��Ҫ���þ�̬IP������֪�����ӵ�WIFI������
//#define ESP32_STATIC_IP   //�Ƿ�ʹ�þ�̬IP

#ifdef ESP32_STATIC_IP 
//IP��ַ��
#define DEVICE_IP "192.168.1.199"
//���ص�ַ
#define DEVICE_GW "192.168.1.1"
//����
#define DEVICE_NETMASK "255.255.255.0"
#endif



#define  DEFAULT_SSID "TP_LINK"        //��Ҫ���ӵ�WIFI����
#define  DEFAULT_PWD "12345678"   //wifi��Ӧ������

static ip4_addr_t s_ip_addr,s_gw_addr,s_netmask_addr;


//����s��0����ʼ������1�������С���2��������
void lcd_display(int s)
{
  char lcd_buff[100]={0};

  Gui_DrawFont_GBK24(15,0,RED,WHITE,(u8 *)"��Ϊ����");
  LCD_P6x8Str(20,24,WHITE,BLACK,(u8 *)"taobao.com");
  Gui_DrawFont_GBK16(16,34,VIOLET,WHITE,(u8 *)"������Ϊ����");
  Gui_DrawFont_GBK16(32,50,BLUE,WHITE,(u8 *)"��ӭ��");

  //��ʾ���ӵ�wifi��Ϣ
  LCD_P6x8Str(0,70,BLACK,WHITE,(u8 *)"mode:STA");
  sprintf(lcd_buff, "ssid:%s",DEFAULT_SSID);
  LCD_P6x8Str(0,80,BLACK,WHITE,(u8 *)lcd_buff);
  sprintf(lcd_buff, "psw:%s",DEFAULT_PWD);
  LCD_P6x8Str(0,90,BLACK,WHITE,(u8 *)lcd_buff);

  if(2==s)
  {
    //2��������
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

// wifi�¼�������
// ctx     :��ʾ������¼����Ͷ�Ӧ��Я���Ĳ���
// event   :��ʾ������¼�����
// ESP_OK  : succeed
// ����    :ʧ�� 
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
        case SYSTEM_EVENT_STA_START://STA����
            printf("sta start.\r\n");
            //�޸��豸������
            ESP_ERROR_CHECK(tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, "sz-yy.taobao.com"));
            esp_wifi_connect();//��ʼ����wifi
            lcd_display(1);
            break;
        case SYSTEM_EVENT_STA_CONNECTED://STA��������
          printf("SYSTEM_EVENT_STA_CONNECTED.\r\n");
          break;
        case SYSTEM_EVENT_STA_GOT_IP://STAȡ��IP
            //ͨ��LOG���IP��Ϣ
            printf("\r\nip:%s.\r\n",ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
            printf("gw:%s.\r\n",ip4addr_ntoa(&event->event_info.got_ip.ip_info.gw));
            printf("netmask:%s.\r\n",ip4addr_ntoa(&event->event_info.got_ip.ip_info.netmask));
            //����IP��Ϣ����LCD��ʾ
            s_ip_addr = event->event_info.got_ip.ip_info.ip;//����IP
            s_gw_addr = event->event_info.got_ip.ip_info.gw;//��������
            s_netmask_addr = event->event_info.got_ip.ip_info.netmask;//��������
            //LCD��ʾ
            lcd_display(2);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED://STA�Ͽ�����
            esp_wifi_connect();//��ʼ����wifi
            lcd_display(1);
            break;
        default:
            break;
    }
    return ESP_OK;
}

//����WIFI��STA
void wifi_init_sta()
{
    tcpip_adapter_init();//tcp/IP����
#ifdef ESP32_STATIC_IP 
    //��̬IP����
    tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA);
    tcpip_adapter_ip_info_t ipInfo;
     
    inet_pton(AF_INET,DEVICE_IP,&ipInfo.ip);
    inet_pton(AF_INET,DEVICE_GW,&ipInfo.gw);
    inet_pton(AF_INET,DEVICE_NETMASK,&ipInfo.netmask);
    tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA,&ipInfo);
#endif

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));//����wifi�¼��ص�����

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));//wifiĬ�ϳ�ʼ��    
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));//NVS ���ڴ洢 WiFi ����.
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = DEFAULT_SSID,//���ӵ��ȵ�����
            .password = DEFAULT_PWD,//���ӵ��ȵ�����
            .scan_method = WIFI_ALL_CHANNEL_SCAN,
            .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
            .threshold.rssi = -127,
        },
    };    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));//����wifi����ģʽΪSTA
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));//����AP����    
    ESP_ERROR_CHECK(esp_wifi_start());    //ʹ��wifi
}

//�û�������ڣ��൱��main����
void app_main()
{
    ESP_ERROR_CHECK( nvs_flash_init() );//��ʼ��NVS

    //��ʾ����ʼ�����Լ�������ʾ
    Lcd_Init();
    lcd_display(0);

    //����WIFI��STA
    wifi_init_sta();
}





