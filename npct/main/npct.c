/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gattc_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_gatt_common_api.h"
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"

#define GATTS_TAG "GATTS_DEMO"

#define PARTITION "nvs"
#define STORAGE_KEY "dev_name"
#define MAX_STR 1024

///Declare the static function
static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

#define GATTS_SERVICE_UUID_TEST_A   0x00FEED
#define GATTS_CHAR_UUID_TEST_A      0x00C0DE
#define GATTS_DESCR_UUID_TEST_A     0x3333
#define GATTS_NUM_HANDLE_TEST_A     4

#define DEFAULT_DEVICE_NAME "NoPhoneContactTracer"
#define TEST_MANUFACTURER_DATA_LEN  17

#define GATTS_DEMO_CHAR_VAL_LEN_MAX 0x40

#define PREPARE_BUF_MAX_SIZE 1024

static uint8_t char1_str[] = {0x11,0x22,0x33};
static esp_gatt_char_prop_t a_property = 0;

static esp_attr_value_t npct_char1_val =
{
    .attr_max_len = GATTS_DEMO_CHAR_VAL_LEN_MAX,
    .attr_len     = sizeof(char1_str),
    .attr_value   = char1_str,
};

static uint8_t adv_config_done = 0;
#define adv_config_flag      (1 << 0)
#define scan_rsp_config_flag (1 << 1)

static uint8_t adv_service_uuid128[32] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xEE, 0x00, 0x00, 0x00,
    //second uuid, 32bit, [12], [13], [14], [15] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
};


// The length of adv data must be less than 31 bytes
//static uint8_t test_manufacturer[TEST_MANUFACTURER_DATA_LEN] =  {0x12, 0x23, 0x45, 0x56};
//adv data
static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = false,
    .min_interval = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
    .max_interval = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
    .appearance = 0x00,
    .manufacturer_len = 0, //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data =  NULL, //&test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(adv_service_uuid128),
    .p_service_uuid = adv_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};
// scan response data
static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = true,
    //.min_interval = 0x0006,
    //.max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = 0, //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data =  NULL, //&test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(adv_service_uuid128),
    .p_service_uuid = adv_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};


static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    //.peer_addr            =
    //.peer_addr_type       =
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

#define PROFILE_NUM 1
#define PROFILE_A_APP_ID 0

struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

/* One gatt-based profile one app_id and one gatts_if, this array will store the gatts_if returned by ESP_GATTS_REG_EVT */
static struct gatts_profile_inst gl_profile_tab[PROFILE_NUM] = 
{
    [PROFILE_A_APP_ID] = 
                        {
                            .gatts_cb = gatts_profile_a_event_handler,
                            .gatts_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
                        }
};

typedef struct {
    uint8_t                 *prepare_buf;
    int                     prepare_len;
} prepare_type_env_t;

static prepare_type_env_t a_prepare_write_env;


///////////// for discovery

#define ID_SIZE 16
#define HID_SIZE 18
#define PACKET_SIZE 20 //18(=id+health) + 2(=encounter count)
#define BLE_HEALTH_NAME_LEN 23
#define MAX_BUFFER 100000

char *Encounters = NULL;
int Encounter_count = 0;
char LastEncounter[ID_SIZE] = "";
int ReadPointer = -1;

#define PROFILE_APP_ID 0


int Gattc_if = ESP_GATT_IF_NONE;

static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    static esp_ble_scan_params_t ble_scan_params = 
    {
        .scan_type              = BLE_SCAN_TYPE_ACTIVE,
        .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
        .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
        .scan_interval          = 0x50,
        .scan_window            = 0x30,
        .scan_duplicate         = BLE_SCAN_DUPLICATE_DISABLE
    };

    switch (event) {
    case ESP_GATTC_REG_EVT:
        printf("REG_EVT");
        esp_err_t scan_ret = esp_ble_gap_set_scan_params(&ble_scan_params);
        if (scan_ret){
            printf("set scan params error, error code = %x", scan_ret);
        }
        break;
   
    default:
        break;
    }
}



static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    /* Store the gattc_if for this profile */
    if (event == ESP_GATTC_REG_EVT && param->reg.status == ESP_GATT_OK) 
    {
        Gattc_if = gattc_if; 
        return;
    }

    // call the profile callback if the gattc_if matches the one registered
    if (gattc_if == ESP_GATT_IF_NONE || gattc_if == Gattc_if)
        gattc_profile_event_handler(event, gattc_if, param);
}



/////////////

int get_encounter_count()
{
    return(Encounter_count);
}

int seen_before(char *hid)
{
    int i, count, ret;
    char hex[3];
    
    for(i=0;i<Encounter_count * PACKET_SIZE;i += PACKET_SIZE)
    {
        if (strncmp(Encounters + i,hid,HID_SIZE) == 0)
        {
            ret = sscanf(Encounters + i + HID_SIZE,"%x",&count);
            if (ret == 1)
            {
                count++;
                sprintf(hex,"%02X",count);
                *(Encounters + i + HID_SIZE) = hex[0];
                *(Encounters + i + HID_SIZE + 1) = hex[1];
                return(1);
            }

        }
    }
    return(0);
}

void add_encounter(char *encounter)
{
    int i,j;
    char *ptr;
    char *tptr;
    const int len0 = strlen("#C19:");

    /*
    if (!(encounter[0] == '#' && encounter[1] == 'C' && encounter[2] == '1' && encounter[3] == '9' && encounter[4] == ':'))
    {
        printf("Not a health token.\n");
        return;
    }

    if (strlen(encounter) != BLE_HEALTH_NAME_LEN)
    {
        printf("Not %d characters in length.\n",BLE_HEALTH_NAME_LEN);
        return;
    }

    if (strncmp(encounter + len0,LastEncounter,ID_SIZE) == 0)
    {
        printf("Last encounter repeat. Ignored from log.\n");
        return;
    }

    if (seen_before(encounter + len0))
    {
        printf("Seen before, but not last time. Updating counter.\n");
        return;
    }

    */



    if (Encounters == NULL)
    {
        Encounters = (char *)malloc(PACKET_SIZE);
        if (Encounters == NULL)
        {
            printf("add_encounter: malloc failed.\n");
            return;
        }
        ptr = Encounters;
        Encounter_count = 0;
    }
    else
    {
        tptr = (char *)realloc(Encounters,(Encounter_count+1) * PACKET_SIZE);
        if (tptr == NULL)
            printf("add_encounter: realloc failed.\n");
        Encounters = tptr;
        ptr = Encounters + Encounter_count * PACKET_SIZE;
    }

    j = 0;
    //for(i=len0;i<BLE_HEALTH_NAME_LEN;i++)
    tptr = ptr;
    for(i=0;i<strlen(encounter);i++)
    {
        *ptr = encounter[i];
        if (j < ID_SIZE)
        {
            LastEncounter[j] = *ptr;
            j++;
        }
        ptr++;
    }
    *ptr = '0';
    *(ptr + 1) = '1';

    while(ptr - tptr < PACKET_SIZE)
        *(ptr++) = 0;
    
    Encounter_count++;
    printf("Encounter added. Count=%d\n",Encounter_count);
}

void example_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);
void example_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);


static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    char *tmp;
    uint8_t *adv_name = NULL;
    uint8_t adv_name_len = 0;
    esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
    uint32_t duration = 0;
    switch (event) 
    {

    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~adv_config_flag);
        if (adv_config_done == 0)
            esp_ble_gap_start_advertising(&adv_params);
        esp_ble_gap_start_scanning(duration);
        break;
    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~scan_rsp_config_flag);
        if (adv_config_done == 0)
            esp_ble_gap_start_advertising(&adv_params);
        break;

    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        //advertising start complete event to indicate advertising start successfully or failed
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS)
            ESP_LOGE(GATTS_TAG, "Advertising start failed\n");
        break;
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS)
            ESP_LOGE(GATTS_TAG, "Advertising stop failed\n");
         else ESP_LOGI(GATTS_TAG, "Stop adv successfully\n");
        break;
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
        break;
    case ESP_GAP_BLE_SCAN_RESULT_EVT: 
        if (scan_result->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT)
        {
            adv_name = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv,ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
            //printf("searched Device Name Len %d\n", adv_name_len);
            if (adv_name_len != 0)
            {
                tmp = (char *)malloc(adv_name_len+1);
                if (tmp != NULL)
                {
                    memset(tmp,0,adv_name_len+1);
                    memmove(tmp,adv_name,adv_name_len);
                    printf("Device found: %s\n",tmp);
                    add_encounter(tmp);
                    printf("Encounters=%d\n",Encounter_count);
                    free(tmp);
                }           
            }
        }
        
        break;
    default:
        break;
    }
}

void example_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param)
{
    esp_gatt_status_t status = ESP_GATT_OK;
    if (param->write.need_rsp){
        if (param->write.is_prep){
            if (prepare_write_env->prepare_buf == NULL) {
                prepare_write_env->prepare_buf = (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE*sizeof(uint8_t));
                prepare_write_env->prepare_len = 0;
                if (prepare_write_env->prepare_buf == NULL) {
                    ESP_LOGE(GATTS_TAG, "Gatt_server prep no mem\n");
                    status = ESP_GATT_NO_RESOURCES;
                }
            } else {
                if(param->write.offset > PREPARE_BUF_MAX_SIZE) {
                    status = ESP_GATT_INVALID_OFFSET;
                } else if ((param->write.offset + param->write.len) > PREPARE_BUF_MAX_SIZE) {
                    status = ESP_GATT_INVALID_ATTR_LEN;
                }
            }

            esp_gatt_rsp_t *gatt_rsp = (esp_gatt_rsp_t *)malloc(sizeof(esp_gatt_rsp_t));
            gatt_rsp->attr_value.len = param->write.len;
            gatt_rsp->attr_value.handle = param->write.handle;
            gatt_rsp->attr_value.offset = param->write.offset;
            gatt_rsp->attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
            memcpy(gatt_rsp->attr_value.value, param->write.value, param->write.len);
            esp_err_t response_err = esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, gatt_rsp);
            if (response_err != ESP_OK){
               ESP_LOGE(GATTS_TAG, "Send response error\n");
            }
            free(gatt_rsp);
            if (status != ESP_GATT_OK){
                return;
            }
            memcpy(prepare_write_env->prepare_buf + param->write.offset,
                   param->write.value,
                   param->write.len);
            prepare_write_env->prepare_len += param->write.len;

        }else{
            esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, NULL);
        }
    }
}

void example_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param){
    if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC){
        esp_log_buffer_hex(GATTS_TAG, prepare_write_env->prepare_buf, prepare_write_env->prepare_len);
    }else{
        ESP_LOGI(GATTS_TAG,"ESP_GATT_PREP_WRITE_CANCEL");
    }
    if (prepare_write_env->prepare_buf) {
        free(prepare_write_env->prepare_buf);
        prepare_write_env->prepare_buf = NULL;
    }
    prepare_write_env->prepare_len = 0;
}

int save_to_nvs(uint8_t *value,int len)
{
    int i;
    nvs_handle_t my_handle;
    esp_err_t err;
    char *str;

    str = (char *)malloc(MAX_STR);
    if (str == NULL)
        return(1);
    memset(str,0,MAX_STR);

    str[0] = '#';
    str[1] = 'C';
    str[2] = '1';
    str[3] = '9';
    str[4] = ':';
    if (len > MAX_STR-1)
        len = MAX_STR-1;
    for(i=0;i<len;i++)
    {
        if (value[i] >= '#' && value[i] < 'z')
            str[5 + i] = value[i];
    }
    printf("Attempting to set device name to: [%s]\n",str);

    esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(str);
    if (set_dev_name_ret == ESP_OK)
        printf("Success! Device name set to: [%s]\n",str);

    // from: https://github.com/espressif/esp-idf/blob/7d75213674b4572f90c68162ad6fe9b16dae65ad/examples/storage/nvs_rw_blob/main/nvs_blob_example_main.c
    // Open
    err = nvs_open(PARTITION, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) 
        {
            free(str);
            printf("Fail: nvs_open.\n");
            return err;
        }
    
    err = nvs_set_str(my_handle, STORAGE_KEY, str);
    if (err != ESP_OK)
        {
            free(str);
            printf("Fail: nvs_set_str.\n");
            return err;
        }

    err = nvs_commit(my_handle);
    if (err != ESP_OK) 
        {
            printf("Fail: nvs_commit.\n");
            free(str);
            return err;
        }

    // Close
    nvs_close(my_handle);

    printf("Device name saved to nvs.\n");

    free(str);
    return ESP_OK;
}

int set_device_name_from_nvs()
{
    char *str;
    size_t len;
    nvs_handle_t my_handle;
    esp_err_t err;
    int i;

    printf("Attempting to set device name...\n");

    err = nvs_open(PARTITION, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) 
        {
            printf("Can't open nvs partition. Using default name.\n");
            esp_ble_gap_set_device_name(DEFAULT_DEVICE_NAME);
            return err;
        }

    str = (char *)malloc(MAX_STR);
    if (str == NULL)
    {
        printf("Error: malloc.\n");
        return(1);
    }

    memset(str,0,MAX_STR);

    err = nvs_get_str(my_handle, STORAGE_KEY, str, &len);
    if (err != ESP_OK)
        {
            printf("nvs_get_str fail. Using default name.\n");
            strcpy(str,DEFAULT_DEVICE_NAME);
        }
    for(i=0;i<len;i++)
        printf("str[%d]=%d (%c)\n",i,str[i],str[i]);

    esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(str);
    if (set_dev_name_ret == ESP_OK)
        printf("Device name set from nvs to: %s\n",str);
    else 
        {
            printf("Fail! Unable to set device name set to %s.\n",str);
            esp_ble_gap_set_device_name(str);
        }
    free(str);
    nvs_close(my_handle);
    return(ESP_OK);
}

static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) 
{
    switch (event) 
    {
    case ESP_GATTS_REG_EVT:
        ESP_LOGI(GATTS_TAG, "REGISTER_APP_EVT, status %d, app_id %d\n", param->reg.status, param->reg.app_id);
        gl_profile_tab[PROFILE_A_APP_ID].service_id.is_primary = true;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.inst_id = 0x00;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.uuid.uuid16 = GATTS_SERVICE_UUID_TEST_A;
       
        //config adv data
        esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
        if (ret){
            ESP_LOGE(GATTS_TAG, "config adv data failed, error code = %x", ret);
        }
        adv_config_done |= adv_config_flag;
        //config scan response data
        ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
        if (ret){
            ESP_LOGE(GATTS_TAG, "config scan response data failed, error code = %x", ret);
        }
        adv_config_done |= scan_rsp_config_flag;
        esp_ble_gatts_create_service(gatts_if, &gl_profile_tab[PROFILE_A_APP_ID].service_id, GATTS_NUM_HANDLE_TEST_A);
        break;

    case ESP_GATTS_READ_EVT: 
   
    // #define ESP_GATT_MAX_ATTR_LEN   600 //as same as GATT_MAX_ATTR_LEN
    // // Gatt attribute value 
    //         typedef struct {
    //             uint8_t           value[ESP_GATT_MAX_ATTR_LEN];         /*!< Gatt attribute value */
    //             uint16_t          handle;                               /*!< Gatt attribute handle */
    //             uint16_t          offset;                               /*!< Gatt attribute value offset */
    //             uint16_t          len;                                  /*!< Gatt attribute value length */
    //             uint8_t           auth_req;                             /*!< Gatt authentication request */
    //         } esp_gatt_value_t;

    

        ESP_LOGI(GATTS_TAG, "GATT_READ_EVT, conn_id %d, trans_id %d, handle %d\n", param->read.conn_id, param->read.trans_id, param->read.handle);
        esp_gatt_rsp_t rsp;
        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
        rsp.attr_value.handle = param->read.handle;
        rsp.attr_value.len = 4;
        if (ReadPointer == -1)
        {
                sprintf((char *)rsp.attr_value.value,"N=%d",Encounter_count);
                rsp.attr_value.len = strlen((char *)rsp.attr_value.value);
                ReadPointer++;
        }
        else if (ReadPointer == Encounter_count)
                {
                    sprintf((char *)rsp.attr_value.value,"end");
                    rsp.attr_value.len = strlen((char *)rsp.attr_value.value);
                    ReadPointer =-1;
                }
        else
        {
             memmove(rsp.attr_value.value,Encounters + ReadPointer * PACKET_SIZE,PACKET_SIZE);
             rsp.attr_value.len = PACKET_SIZE;
             ReadPointer++;
        }
        esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id,ESP_GATT_OK, &rsp);
        break;
    
    case ESP_GATTS_WRITE_EVT: 
        ESP_LOGI(GATTS_TAG, "GATT_WRITE_EVT, conn_id %d, trans_id %d, handle %d", param->write.conn_id, param->write.trans_id, param->write.handle);
        if (!param->write.is_prep)
        {
            ESP_LOGI(GATTS_TAG, "GATT_WRITE_EVT, value len %d, value :", param->write.len);
            {
                if (isalnum(param->write.value[0]))
                    {
                        if (ESP_OK == save_to_nvs(param->write.value,param->write.len))
                            printf("Storage succeeded.\n");
                    }
                else printf("Skipping name set. Input not alpha/num\n");
            }
        }
        example_write_event_env(gatts_if, &a_prepare_write_env, param);
        break;
    case ESP_GATTS_EXEC_WRITE_EVT:
        ESP_LOGI(GATTS_TAG,"ESP_GATTS_EXEC_WRITE_EVT");
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
        example_exec_write_event_env(&a_prepare_write_env, param);
        break;
    case ESP_GATTS_MTU_EVT:
        ESP_LOGI(GATTS_TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        break;
    case ESP_GATTS_UNREG_EVT:
        break;
    case ESP_GATTS_CREATE_EVT:
        ESP_LOGI(GATTS_TAG, "CREATE_SERVICE_EVT, status %d,  service_handle %d\n", param->create.status, param->create.service_handle);
        gl_profile_tab[PROFILE_A_APP_ID].service_handle = param->create.service_handle;
        gl_profile_tab[PROFILE_A_APP_ID].char_uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_A_APP_ID].char_uuid.uuid.uuid16 = GATTS_CHAR_UUID_TEST_A;

        esp_ble_gatts_start_service(gl_profile_tab[PROFILE_A_APP_ID].service_handle);
        a_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
        esp_err_t add_char_ret = esp_ble_gatts_add_char(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].char_uuid,
                                                        ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                                        a_property,
                                                        &npct_char1_val, NULL);
        if (add_char_ret){
            ESP_LOGE(GATTS_TAG, "add char failed, error code =%x",add_char_ret);
        }
        break;
    case ESP_GATTS_ADD_INCL_SRVC_EVT:
        break;
    case ESP_GATTS_ADD_CHAR_EVT: {
        uint16_t length = 0;
        const uint8_t *prf_char;

        ESP_LOGI(GATTS_TAG, "ADD_CHAR_EVT, status %d,  attr_handle %d, service_handle %d\n",
                param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);
        gl_profile_tab[PROFILE_A_APP_ID].char_handle = param->add_char.attr_handle;
        gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
        esp_err_t get_attr_ret = esp_ble_gatts_get_attr_value(param->add_char.attr_handle,  &length, &prf_char);
        if (get_attr_ret == ESP_FAIL){
            ESP_LOGE(GATTS_TAG, "ILLEGAL HANDLE");
        }

        ESP_LOGI(GATTS_TAG, "the gatts demo char length = %x\n", length);
        for(int i = 0; i < length; i++){
            ESP_LOGI(GATTS_TAG, "prf_char[%x] =%x\n",i,prf_char[i]);
        }
        esp_err_t add_descr_ret = esp_ble_gatts_add_char_descr(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].descr_uuid,
                                                                ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, NULL, NULL);
        if (add_descr_ret){
            ESP_LOGE(GATTS_TAG, "add char descr failed, error code =%x", add_descr_ret);
        }
        break;
    }
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        gl_profile_tab[PROFILE_A_APP_ID].descr_handle = param->add_char_descr.attr_handle;
        ESP_LOGI(GATTS_TAG, "ADD_DESCR_EVT, status %d, attr_handle %d, service_handle %d\n",
                 param->add_char_descr.status, param->add_char_descr.attr_handle, param->add_char_descr.service_handle);
        break;
    case ESP_GATTS_DELETE_EVT:
        break;
    case ESP_GATTS_START_EVT:
        ESP_LOGI(GATTS_TAG, "SERVICE_START_EVT, status %d, service_handle %d\n",
                 param->start.status, param->start.service_handle);
        break;
    case ESP_GATTS_STOP_EVT:
        break;
    case ESP_GATTS_CONNECT_EVT: {
        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        /* For the IOS system, please reference the apple official documents about the ble connection parameters restrictions. */
        conn_params.latency = 0;
        conn_params.max_int = 0x20;    // max_int = 0x20*1.25ms = 40ms
        conn_params.min_int = 0x10;    // min_int = 0x10*1.25ms = 20ms
        conn_params.timeout = 400;    // timeout = 400*10ms = 4000ms
        ESP_LOGI(GATTS_TAG, "ESP_GATTS_CONNECT_EVT, conn_id %d, remote %02x:%02x:%02x:%02x:%02x:%02x:",
                 param->connect.conn_id,
                 param->connect.remote_bda[0], param->connect.remote_bda[1], param->connect.remote_bda[2],
                 param->connect.remote_bda[3], param->connect.remote_bda[4], param->connect.remote_bda[5]);
        gl_profile_tab[PROFILE_A_APP_ID].conn_id = param->connect.conn_id;
        //start sent the update connection parameters to the peer device.
        esp_ble_gap_update_conn_params(&conn_params);
        break;
    }
    case ESP_GATTS_DISCONNECT_EVT:
        ESP_LOGI(GATTS_TAG, "ESP_GATTS_DISCONNECT_EVT, disconnect reason 0x%x", param->disconnect.reason);
        esp_ble_gap_start_advertising(&adv_params);
        break;
    case ESP_GATTS_CONF_EVT:
        ESP_LOGI(GATTS_TAG, "ESP_GATTS_CONF_EVT, status %d attr_handle %d", param->conf.status, param->conf.handle);
        if (param->conf.status != ESP_GATT_OK){
            esp_log_buffer_hex(GATTS_TAG, param->conf.value, param->conf.len);
        }
        break;
    case ESP_GATTS_OPEN_EVT:
    case ESP_GATTS_CANCEL_OPEN_EVT:
    case ESP_GATTS_CLOSE_EVT:
    case ESP_GATTS_LISTEN_EVT:
    case ESP_GATTS_CONGEST_EVT:
    default:
        break;
    }
}


static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    int idx;

    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gatts_if = gatts_if;
        } else {
            ESP_LOGI(GATTS_TAG, "Reg app failed, app_id %04x, status %d\n",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }

    /* If the gatts_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */

    for (idx = 0; idx < PROFILE_NUM; idx++) 
    {
        if (gatts_if == ESP_GATT_IF_NONE || gatts_if == gl_profile_tab[idx].gatts_if)  /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
        {
            if (gl_profile_tab[idx].gatts_cb) 
                gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
        }
    }

}


void app_main(void)
{
    esp_err_t ret;
    
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));\


    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(GATTS_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(GATTS_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(GATTS_TAG, "%s init bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(GATTS_TAG, "%s enable bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    set_device_name_from_nvs();

    // for discovery

    Encounters = NULL;
    Encounter_count = 0;
   
    // //register the  callback function to the gap module
    // ret = esp_ble_gap_register_callback(esp_gap_cb);
    // if (ret){
    //     printf("%s gap register failed, error code = %x\n", __func__, ret);
    //     return;
    // }


    //register the callback function to the gattc module
    ret = esp_ble_gattc_register_callback(esp_gattc_cb);
    if(ret){
        printf("%s gattc register failed, error code = %x\n", __func__, ret);
        return;
    }

    ret = esp_ble_gattc_app_register(PROFILE_APP_ID);
    if (ret){
        printf("%s gattc app register failed, error code = %x\n", __func__, ret);
    }
    

    // for BT server


    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret){
        ESP_LOGE(GATTS_TAG, "gatts register error, error code = %x", ret);
        return;
    }
    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret){
        ESP_LOGE(GATTS_TAG, "gap register error, error code = %x", ret);
        return;
    }
    ret = esp_ble_gatts_app_register(PROFILE_A_APP_ID);
    if (ret){
        ESP_LOGE(GATTS_TAG, "gatts app register error, error code = %x", ret);
        return;
    }

    
    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret){
        ESP_LOGE(GATTS_TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
    }
    
}
