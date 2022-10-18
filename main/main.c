#include "main.h"

// ADC channels for analog stick input
#define ADC_STICK_LX        ADC1_CHANNEL_0  // VP
#define ADC_STICK_LY        ADC1_CHANNEL_3  // VN
#define ADC_STICK_RX        ADC1_CHANNEL_6  // D34
#define ADC_STICK_RY        ADC1_CHANNEL_7  // D35

#define ADC_TRIGGER_L       ADC1_CHANNEL_4  // D32 ZL - Trigger
#define ADC_TRIGGER_R       ADC1_CHANNEL_5  // D33 ZR - Trigger

#define GPIO_BTN_ZL         GPIO_NUM_32     // D32
#define GPIO_BTN_ZR         GPIO_NUM_33     // D33

#define GPIO_BTN_DU         GPIO_NUM_25     // D25
#define GPIO_BTN_DD         GPIO_NUM_26     // D26
#define GPIO_BTN_DL         GPIO_NUM_27     // D27
#define GPIO_BTN_DR         GPIO_NUM_14     // D14

#define GPIO_BTN_SELECT     GPIO_NUM_12     // D12
#define GPIO_BTN_START      GPIO_NUM_13     // D13

#define GPIO_BTN_L          GPIO_NUM_23     // D23
#define GPIO_BTN_R          GPIO_NUM_22     // D22

#define GPIO_BTN_STICKL     GPIO_NUM_3      // RX0
#define GPIO_BTN_STICKR     GPIO_NUM_21     // D21

#define GPIO_BTN_B          GPIO_NUM_19     // D19
#define GPIO_BTN_A          GPIO_NUM_18     // D18
#define GPIO_BTN_Y          GPIO_NUM_5      // D5
#define GPIO_BTN_X          GPIO_NUM_17     // TX2

#define GPIO_BTN_HOME       GPIO_NUM_16     // RX2
#define GPIO_BTN_CAPTURE    GPIO_NUM_4      // D4

//Management Battery & Sync
#define ADC_BATTERY_LEVEL   ADC2_CHANNEL_2  // D2
#define GPIO_LED_SYNC       GPIO_NUM_1      // D1
#define GPIO_BTN_SYNC       GPIO_NUM_15     // D15

#define GPIO_OUTPUT_PIN_SEL    (1ULL<<GPIO_LED_SYNC)

// Input pin mask creation
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_BTN_SYNC) | (1ULL<<GPIO_BTN_DU) | (1ULL<<GPIO_BTN_DD) | (1ULL<<GPIO_BTN_DL) | (1ULL<<GPIO_BTN_DR) | (1ULL<<GPIO_BTN_STICKL) | (1ULL<<GPIO_BTN_SELECT) | (1ULL<<GPIO_BTN_L) | (1ULL<<GPIO_BTN_ZL) | (1ULL<<GPIO_BTN_STICKR) | (1ULL<<GPIO_BTN_R) | (1ULL<<GPIO_BTN_ZR) | (1ULL<<GPIO_BTN_A) | (1ULL<<GPIO_BTN_B) | (1ULL<<GPIO_BTN_X) | (1ULL<<GPIO_BTN_Y) | (1ULL<<GPIO_BTN_START) | (1ULL<<GPIO_BTN_HOME) | (1ULL<<GPIO_BTN_CAPTURE))

// Variables used to store register reads
uint32_t regread = 0;
uint8_t ns_controller_type = 0x03;
TaskHandle_t BlinkHandle = NULL;

bool getbit(uint32_t bytes, uint8_t bit)
{
    return !((bytes >> bit) & 0x1);
}

void sync_controller()
{
    const char* TAG = "sync_controller";

    ns_controller_type += 0x01;

    if (ns_controller_type > 0x0A)
    {
        ns_controller_type = 0x03;
    }

    ESP_LOGI(TAG, "New Controller Type: %d", ns_controller_type);

    //Write last controller
    nvs_handle_t my_handle_write;
    nvs_flash_init();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    nvs_open("storage", NVS_READWRITE, &my_handle_write);
    nvs_set_u8(my_handle_write, "controller_type", ns_controller_type);
    nvs_commit(my_handle_write);
    nvs_close(my_handle_write);

    ESP_LOGI(TAG, "Saved Controller Type: %d", ns_controller_type);

    esp_restart();    
}

// Set up function to update inputs
// This will scan the sticks/buttons once
// at a refresh rate determined by the core.
void button_task()
{
    // Read the GPIO registers and mask the data
    regread = REG_READ(GPIO_IN_REG) & GPIO_INPUT_PIN_SEL;

    if (getbit(regread, GPIO_BTN_SYNC))
        sync_controller();

    g_button_data.d_up      = getbit(regread, GPIO_BTN_DU);
    g_button_data.d_down    = getbit(regread, GPIO_BTN_DD);
    g_button_data.d_left    = getbit(regread, GPIO_BTN_DL);
    g_button_data.d_right   = getbit(regread, GPIO_BTN_DR);

    g_button_data.b_right   = getbit(regread, GPIO_BTN_A) && !getbit(regread, GPIO_BTN_B);
    g_button_data.b_down    = getbit(regread, GPIO_BTN_B) && !getbit(regread, GPIO_BTN_A);    
    g_button_data.b_up      = getbit(regread, GPIO_BTN_X);
    g_button_data.b_left    = getbit(regread, GPIO_BTN_Y);

    g_button_data.t_l       = (getbit(regread, GPIO_BTN_L) || (getbit(regread, GPIO_BTN_START) && getbit(regread, GPIO_BTN_B))) && !getbit(regread, GPIO_BTN_START) && !getbit(regread, GPIO_BTN_R);
    g_button_data.t_r       = (getbit(regread, GPIO_BTN_R) || (getbit(regread, GPIO_BTN_START) && getbit(regread, GPIO_BTN_A))) && !getbit(regread, GPIO_BTN_START) && !getbit(regread, GPIO_BTN_L);
    g_button_data.t_zl      = !gpio_get_level(GPIO_BTN_ZL) || (getbit(regread, GPIO_BTN_START) && getbit(regread, GPIO_BTN_L));;
    g_button_data.t_zr      = !gpio_get_level(GPIO_BTN_ZR) || (getbit(regread, GPIO_BTN_START) && getbit(regread, GPIO_BTN_R));;

    g_button_data.b_select  = getbit(regread, GPIO_BTN_SELECT);
    g_button_data.b_start   = getbit(regread, GPIO_BTN_START) && !getbit(regread, GPIO_BTN_A) && !getbit(regread, GPIO_BTN_B) && !getbit(regread, GPIO_BTN_L) && !getbit(regread, GPIO_BTN_R);
    g_button_data.b_capture = getbit(regread, GPIO_BTN_CAPTURE) || (getbit(regread, GPIO_BTN_START) && getbit(regread, GPIO_BTN_L) && getbit(regread, GPIO_BTN_R));
    g_button_data.b_home    = getbit(regread, GPIO_BTN_HOME) || (getbit(regread, GPIO_BTN_L) && getbit(regread, GPIO_BTN_R));

    g_button_data.sb_left   = getbit(regread, GPIO_BTN_STICKL);
    g_button_data.sb_right  = getbit(regread, GPIO_BTN_STICKR);

}

// Separate task to read sticks.
// This is essential to have as a separate component as ADC scans typically take more time and this is only
// scanned once between each polling interval. This varies from core to core.
void stick_task()
{
    //const char* TAG = "stick_task";
    // read stick 1 and 2

    g_stick_data.lsx = (uint16_t) adc1_get_raw(ADC_STICK_LX);
    g_stick_data.lsy = (uint16_t) adc1_get_raw(ADC_STICK_LY);
    g_stick_data.rsx = (uint16_t) adc1_get_raw(ADC_STICK_RX);
    g_stick_data.rsy = (uint16_t) adc1_get_raw(ADC_STICK_RY);

    return;
}

void led_task()
{
    while(true) 
    {
        gpio_set_level(GPIO_LED_SYNC, 1);
        vTaskDelay(50);
        gpio_set_level(GPIO_LED_SYNC, 0);
        vTaskDelay(50);
    }
}

void app_main()
{
    const char* TAG = "app_main";

    //Config Led
    gpio_config_t io_conf_led;
    io_conf_led.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf_led.mode = GPIO_MODE_OUTPUT;
    io_conf_led.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf_led.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf_led.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf_led);

    //Simulate Led sync flash
    xTaskCreatePinnedToCore(led_task, "led_task", 1024, NULL, 1, &BlinkHandle, 1);

    // Set up ADC
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC_STICK_LX, ADC_ATTEN_DB_11));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC_STICK_LY, ADC_ATTEN_DB_11));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC_STICK_RX, ADC_ATTEN_DB_11));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC_STICK_RY, ADC_ATTEN_DB_11));

    // IO configuration we can reuse
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    //Read last controller
    nvs_handle_t my_handle_read;
    nvs_flash_init();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    nvs_open("storage", NVS_READWRITE, &my_handle_read);
    nvs_get_u8(my_handle_read, "controller_type", &ns_controller_type);
    nvs_close(my_handle_read);

    if(ns_controller_type >= 0x00 && ns_controller_type <= 0x02)
        ns_controller_type = 0x03;

    ESP_LOGI(TAG, "Read Controller Type: %d", ns_controller_type);

    rb_register_button_callback(button_task);
    rb_register_stick_callback(stick_task);

    rb_api_init();
    rb_api_setCore(CORE_NINTENDOSWITCH);
    rb_api_startController(ns_controller_type);

    vTaskDelay(600);
    vTaskDelete(BlinkHandle);
    BlinkHandle = NULL;
    gpio_set_level(GPIO_LED_SYNC, 1);
}
