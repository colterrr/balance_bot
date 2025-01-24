/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#if CONFIG_BT_NIMBLE_ENABLED
#include "host/ble_hs.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#else
#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#endif

#if CONFIG_BT_NIMBLE_ENABLED
#include "host/ble_hs.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#define ESP_BD_ADDR_STR         "%02x:%02x:%02x:%02x:%02x:%02x"
#define ESP_BD_ADDR_HEX(addr)   addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]
#else
#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#endif

#include "esp_hidh.h"
#include "esp_hid_gap.h"
#include "esp_hidh_api.h"

#include "port_uart.h"

static const char *TAG = "ESP_HIDH_DEMO";
static const uint8_t xbox_addr[6] = {0x0c, 0x35, 0x26, 0x62, 0xd9, 0xf4};
static uint8_t is_find_flag = 0;

#pragma pack(1)
typedef struct
{
    uint16_t left_x;       ///< 左0~右65535
    uint16_t left_y;       ///< 上0~下65535
    uint16_t right_x;      ///< 左0~右65535
    uint16_t right_y;      ///< 上0~下65535
    uint16_t triger_left;  ///< 不按0~按到底1023
    uint16_t triger_right; ///< 不按0~按到底1023
    uint8_t key_dir;       ///< 1~8 垂直向上为1，顺时针旋转递增，不按为0
    struct
    {
        uint8_t key_a : 1;
        uint8_t key_b : 1;
        uint8_t  : 1;
        uint8_t key_x : 1;
        uint8_t key_y : 1;
        uint8_t : 1;
        uint8_t key_lb : 1;
        uint8_t key_rb : 1;
    };
    struct
    {
        uint8_t : 2;
        uint8_t key_page: 1;
        uint8_t key_menu : 1;
        uint8_t : 4;
    };
    uint8_t share;
} xbox_message;
#pragma pack()

static struct{
    uint8_t head;
    uint8_t online_flag;
    xbox_message xbox_sta;
    uint8_t tail;
}send_msg;

xbox_message last_msg;

static void xbox_analyse(xbox_message* msg)
{
    if (msg->left_x != last_msg.left_x || msg->left_y != last_msg.left_y) {
        last_msg.left_x = msg->left_x;
        last_msg.left_y = msg->left_y;
        printf("左拨杆位置x:%5u y:%5u  ", msg->left_x, msg->left_y);
    }
    if (msg->right_x != last_msg.right_x || msg->right_y != last_msg.right_y) {
        last_msg.right_x = msg->right_x;
        last_msg.right_y = msg->right_y;
        printf("右拨杆位置x:%5u y:%5u  ", msg->right_x, msg->right_y);
    }
    if (msg->triger_left != last_msg.triger_left) {
        last_msg.triger_left = msg->triger_left;
        printf("LT:%3u  ", msg->triger_left);
    }
    if (msg->triger_right != last_msg.triger_right) {
        last_msg.triger_right = msg->triger_right;
        printf("RT:%3u  ", msg->triger_right);
    }
    if (msg->key_dir != last_msg.key_dir) {
        last_msg.key_dir = msg->key_dir;
        printf("十字键:%1u  ", msg->key_dir);
    }
    if (msg->key_a != last_msg.key_a) {
        last_msg.key_a = msg->key_a;
        if (msg->key_a) printf("A按下  ");
        else printf("A松开  ");
    }
    if (msg->key_b != last_msg.key_b) {
        last_msg.key_b = msg->key_b;
        if (msg->key_b) printf("B按下  ");
        else printf("B松开  ");
    }
    if (msg->key_x != last_msg.key_x) {
        last_msg.key_x = msg->key_x;
        if (msg->key_x) printf("X按下  ");
        else printf("X松开  ");
    }
    if (msg->key_y != last_msg.key_y) {
        last_msg.key_y = msg->key_y;
        if (msg->key_y) printf("Y按下  ");
        else printf("Y松开  ");
    }
    if (msg->key_lb != last_msg.key_lb) {
        last_msg.key_lb = msg->key_lb;
        if (msg->key_lb) printf("LB按下  ");
        else printf("LB松开  ");
    }
    if (msg->key_rb != last_msg.key_rb) {
        last_msg.key_rb = msg->key_rb;
        if (msg->key_rb) printf("RB按下  ");
        else printf("RB松开  ");
    }
    if (msg->key_page != last_msg.key_page) {
        last_msg.key_page = msg->key_page;
        if (msg->key_page) printf("PAGE键按下  ");
        else printf("PAGE键松开  ");
    }
    if (msg->key_menu != last_msg.key_menu) {
        last_msg.key_menu = msg->key_menu;
        if (msg->key_menu) printf("菜单键按下  ");
        else printf("菜单键松开  ");
    }
    if (msg->share != last_msg.share) {
        last_msg.share = msg->share;
        if (msg->share) printf("分享键按下  ");
        else printf("分享键松开  ");
    }
    printf("\n");
}

static char *bda2str(esp_bd_addr_t bda, char *str, size_t size)
{
    if (bda == NULL || str == NULL || size < 18) {
        return NULL;
    }

    uint8_t *p = bda;
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
            p[0], p[1], p[2], p[3], p[4], p[5]);
    return str;
}

void hidh_callback(void *handler_args, esp_event_base_t base, int32_t id, void *event_data)
{
    esp_hidh_event_t event = (esp_hidh_event_t)id;
    esp_hidh_event_data_t *param = (esp_hidh_event_data_t *)event_data;

    switch (event) {
    case ESP_HIDH_OPEN_EVENT: {
        if (param->open.status == ESP_OK) {
            const uint8_t *bda = esp_hidh_dev_bda_get(param->open.dev);
            ESP_LOGI(TAG, ESP_BD_ADDR_STR " OPEN: %s", ESP_BD_ADDR_HEX(bda), esp_hidh_dev_name_get(param->open.dev));
            esp_hidh_dev_dump(param->open.dev, stdout);
        } else {
            ESP_LOGE(TAG, " OPEN failed!");
        }
        break;
    }
    case ESP_HIDH_BATTERY_EVENT: {
        const uint8_t *bda = esp_hidh_dev_bda_get(param->battery.dev);
        ESP_LOGI(TAG, ESP_BD_ADDR_STR " BATTERY: %d%%", ESP_BD_ADDR_HEX(bda), param->battery.level);
        break;
    }
    case ESP_HIDH_INPUT_EVENT: {
        const uint8_t *bda = esp_hidh_dev_bda_get(param->input.dev);
        uint8_t* pdata = param->input.data;
        uint16_t len = param->input.length;
        if (len == 16 && !memcmp(bda, xbox_addr, 6)) { //是xbox手柄report的数据
            if (memcmp(pdata, &last_msg, 16) != 0) {
                xbox_analyse((xbox_message*)pdata);
            }
        }
        break;
    }
    case ESP_HIDH_FEATURE_EVENT: {
        const uint8_t *bda = esp_hidh_dev_bda_get(param->feature.dev);
        ESP_LOGI(TAG, ESP_BD_ADDR_STR " FEATURE: %8s, MAP: %2u, ID: %3u, Len: %d", ESP_BD_ADDR_HEX(bda),
                 esp_hid_usage_str(param->feature.usage), param->feature.map_index, param->feature.report_id,
                 param->feature.length);
        ESP_LOG_BUFFER_HEX(TAG, param->feature.data, param->feature.length);
        break;
    }
    case ESP_HIDH_CLOSE_EVENT: {
        const uint8_t *bda = esp_hidh_dev_bda_get(param->close.dev);
        ESP_LOGI(TAG, ESP_BD_ADDR_STR " CLOSE: %s", ESP_BD_ADDR_HEX(bda), esp_hidh_dev_name_get(param->close.dev));
        if (!memcmp(bda, xbox_addr, 6)) {
            is_find_flag = 0;            
        }
        break;
    }
    default:
        ESP_LOGI(TAG, "EVENT: %d", event);
        break;
    }
}

#define SCAN_DURATION_SECONDS 5

void hid_demo_task(void *pvParameters)
{
    while (1)
    {
        size_t results_len = 0;
        esp_hid_scan_result_t *results = NULL;       
            ESP_LOGI(TAG, "SCAN For Controller...");
        //start scan for HID devices
        if (esp_hid_scan(SCAN_DURATION_SECONDS, &results_len, &results) != ESP_OK) {
            ESP_LOGI(TAG, "HID SCAN ERROR\n");
        }
        ESP_LOGI(TAG, "SCAN: %u HID results", results_len);
        if (results_len) {
            esp_hid_scan_result_t *r = results;
            esp_hid_scan_result_t *cr = NULL;
            while (r) {
                printf("  %s: " ESP_BD_ADDR_STR ", ", (r->transport == ESP_HID_TRANSPORT_BLE) ? "BLE" : "BT ", ESP_BD_ADDR_HEX(r->bda));
                printf("RSSI: %d, ", r->rssi);
                printf("USAGE: %s, ", esp_hid_usage_str(r->usage));
    #if CONFIG_BT_BLE_ENABLED
                if (r->transport == ESP_HID_TRANSPORT_BLE) {
                    printf("APPEARANCE: 0x%04x, ", r->ble.appearance);
                    printf("ADDR_TYPE: '%s', ", ble_addr_type_str(r->ble.addr_type));
                }
    #endif /* CONFIG_BT_BLE_ENABLED */
    #if CONFIG_BT_NIMBLE_ENABLED
                if (r->transport == ESP_HID_TRANSPORT_BLE) {
                    cr = r;
                    printf("APPEARANCE: 0x%04x, ", r->ble.appearance);
                    printf("ADDR_TYPE: '%d', ", r->ble.addr_type);
                }
    #endif /* CONFIG_BT_BLE_ENABLED */
    #if CONFIG_BT_HID_HOST_ENABLED
                if (r->transport == ESP_HID_TRANSPORT_BT) {
                    printf("COD: %s[", esp_hid_cod_major_str(r->bt.cod.major));
                    esp_hid_cod_minor_print(r->bt.cod.minor, stdout);
                    printf("] srv 0x%03x, ", r->bt.cod.service);
                    print_uuid(&r->bt.uuid);
                    printf(", ");
                }
    #endif /* CONFIG_BT_HID_HOST_ENABLED */
                printf("NAME: %s ", r->name ? r->name : "");
                printf("\n");
                if ( !memcmp(r->bda, xbox_addr, 6) ) {
                    printf("Controller found\n");
                    // 找到符合预设地址的手柄设备
                    cr = r;
                    is_find_flag = 1;
                }
                r = r->next;
            }
            if (cr) {
                //open the last result
                esp_hidh_dev_open(cr->bda, cr->transport, cr->ble.addr_type);
            }
            //free the results
            esp_hid_scan_results_free(results);
        }
        if (is_find_flag == 0) {
            for (uint8_t ii = 4; ii > 0; ii--) {
                ESP_LOGI(TAG, "Rescan in %d second...", ii);
                vTaskDelay(pdMS_TO_TICKS(1000));
            }        
        }
        else {
            while (is_find_flag) {
                vTaskDelay(pdMS_TO_TICKS(100)); 
            }
        }
    }
}

#if CONFIG_BT_NIMBLE_ENABLED
void ble_hid_host_task(void *param)
{
    ESP_LOGI(TAG, "BLE Host Task Started");
    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();

    nimble_port_freertos_deinit();
}
void ble_store_config_init(void);
#endif

void uart_task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    send_msg.head = 33;
    send_msg.tail = 66;
    while (1)
    {
        if (is_find_flag) send_msg.online_flag = 1;
        else send_msg.online_flag = 0;
        send_msg.xbox_sta = last_msg;
        uart_send((uint8_t*)&send_msg, sizeof(send_msg));
        xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50));
    }
}

void app_main(void)
{
    char bda_str[18] = {0};
    esp_err_t ret;
#if HID_HOST_MODE == HIDH_IDLE_MODE
    ESP_LOGE(TAG, "Please turn on BT HID host or BLE!");
    return;
#endif
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    ESP_LOGI(TAG, "setting hid gap, mode:%d", HID_HOST_MODE);
    ESP_ERROR_CHECK( esp_hid_gap_init(HID_HOST_MODE) );
#if CONFIG_BT_BLE_ENABLED
    ESP_ERROR_CHECK( esp_ble_gattc_register_callback(esp_hidh_gattc_event_handler) );
#endif /* CONFIG_BT_BLE_ENABLED */
    esp_hidh_config_t config = {
        .callback = hidh_callback,
        .event_stack_size = 4096,
        .callback_arg = NULL,
    };
    ESP_ERROR_CHECK( esp_hidh_init(&config) );
    ESP_LOGI(TAG, "Own address:[%s]", bda2str((uint8_t *)esp_bt_dev_get_address(), bda_str, sizeof(bda_str)));
#if CONFIG_BT_NIMBLE_ENABLED
    /* XXX Need to have template for store */
    ble_store_config_init();

    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
	/* Starting nimble task after gatts is initialized*/
    ret = esp_nimble_enable(ble_hid_host_task);
    if (ret) {
        ESP_LOGE(TAG, "esp_nimble_enable failed: %d", ret);
    }
#endif

    uart_init();
    xTaskCreate(&uart_task, "uart_task", 2 * 1024, NULL, 2, NULL);
    xTaskCreate(&hid_demo_task, "hid_task", 6 * 1024, NULL, 2, NULL);
}
