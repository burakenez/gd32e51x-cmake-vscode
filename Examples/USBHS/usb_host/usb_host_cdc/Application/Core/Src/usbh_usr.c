/*!
    \file    usbh_usr.c
    \brief   user application layer for USBFS host mode cdc class operation

    \version 2026-02-09, V1.4.0, firmware for GD32E51x
*/

/*
    Copyright (c) 2025, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include <string.h>
#include "usbh_usr.h"
#include "usbh_fs.h"
#include "drv_usb_hw.h"
#include "usbh_cdc_core.h"

extern usbh_host usb_host;

uint8_t enable_display_received_data = 0U;
cdc_demo_state_machine cdc_demo;
cdc_demo_setting_state_machine cdc_settings_state;
cdc_demo_select_mode cdc_select_mode;

uint16_t LINE = START_LINE;

static uint8_t prev_baud_rate_idx = 0U;
static uint8_t prev_data_bits_idx = 0U;
static uint8_t prev_parity_idx = 0U;
static uint8_t prev_stop_bits_idx = 0U;
static uint8_t prev_select = 0U;

uint8_t *CDC_main_menu[] = {
    (uint8_t *)"      1 - Send Data                         ",
    (uint8_t *)"      2 - Receive Data                      ",
    (uint8_t *)"      3 - Configuration                     "
};

uint8_t *DEMO_SEND_menu[] = {
    (uint8_t *)"      1 - Send File 1                       ",
    (uint8_t *)"      2 - Send File 2                       ",
    (uint8_t *)"      3 - Send File 3                       ",
    (uint8_t *)"      4 - Send dummy data                   ",
    (uint8_t *)"      5 - Return                            "
};

uint8_t *DEMO_RECEIVE_menu[] = {
    (uint8_t *)"      1 - Return                            ",
    (uint8_t *)"                                            ",
    (uint8_t *)"                                            "
};

uint8_t *DEMO_CONFIGURATION_menu[] = {
    (uint8_t *)"      1 - Apply new settings                ",
    (uint8_t *)"      2 - Return                            ",
    (uint8_t *)"                                            "
};

const uint8_t MSG_BITS_PER_SECOND[] = "         : Bit Per Second                          ";
const uint8_t MSG_DATA_BITS[] = "               : Data width                              ";
const uint8_t MSG_PARITY[] = "                  : Parity                                  ";
const uint8_t MSG_STOP_BITS[] = "               : Stop Bits                               ";

const uint32_t BaudRateValue[NB_BAUD_RATES] = \
{2400U, 4800U, 9600U, 19200U, 38400U, 57600U, 115200U, 230400U, 460800U, 921600U};

const uint8_t DataBitsValue[4] = {5U, 6U, 7U, 8U};
const uint8_t ParityArray[3][5] = {"NONE", "EVEN", "ODD"};
const uint8_t StopBitsArray[2][2] = {"1", "2"};

/*  points to the usbh_user_cb structure */
/*  the purpose of this register is to speed up the execution */
usbh_user_cb usr_cb = {
    usbh_user_init,
    usbh_user_deinit,
    usbh_user_device_connected,
    usbh_user_device_reset,
    usbh_user_device_disconnected,
    usbh_user_over_current_detected,
    usbh_user_device_speed_detected,
    usbh_user_device_desc_available,
    usbh_user_device_address_assigned,
    usbh_user_configuration_descavailable,
    usbh_user_manufacturer_string,
    usbh_user_product_string,
    usbh_user_serialnum_string,
    usbh_user_enumeration_finish,
    usbh_user_userinput,
    usbh_user_application,
    usbh_user_device_not_supported,
    usbh_user_unrecovered_error
};

const uint8_t MSG_SEPARATION[]  = "-----------------------------------";
#if USE_USB_HS
    const uint8_t MSG_HOST_HEADER[] = "USB HS CDC Host";
#else
    const uint8_t MSG_HOST_HEADER[] = "USB FS CDC Host";
#endif
const uint8_t MSG_HOST_FOOTER[] = "USB Host Library v1.0.0";

static void cdc_select_item(uint8_t **menu, uint8_t item);
static void cdc_handle_menu(void);
static void cdc_handle_send_menu(void);
static void cdc_handle_receive_menu(void);
static void cdc_handle_config_menu(void);
static void cdc_change_select_mode(cdc_demo_select_mode select_mode);
static void cdc_adjust_setting_menu(cdc_demo_setting_state_machine *settings_state);
static void cdc_select_settings_item(uint8_t item);
static void cdc_output_data(uint8_t *ptr);
static void cdc_set_initial_value(void);
static void LCD_clear_part(void);

/*!
    \brief      displays the message on LCD for host lib initialization
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_init(void)
{
    static uint8_t startup = 0U;

    if(0U == startup) {
        startup = 1U;

        gd_eval_key_init(KEY_CET, KEY_MODE_GPIO);

        gd_eval_lcd_init();

        lcd_log_init();

        lcd_log_header_set((uint8_t *)MSG_HOST_HEADER, 60U);

        LCD_UsrLog("> Host Library Initialized\n");

        lcd_log_footer_set((uint8_t *)MSG_HOST_FOOTER, 40U);
    }
}

/*!
    \brief      user operation for device attached
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_connected(void)
{
    LCD_UsrLog("> Device Attached.\n");
}

/*!
    \brief      user operation when unrecoveredError happens
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_unrecovered_error(void)
{
    LCD_ErrLog("> UNRECOVERED ERROR STATE.\n");
}

/*!
    \brief      user operation for device disconnect event
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_disconnected(void)
{
    LCD_UsrLog("> Device Disconnected.\n");
}

/*!
    \brief      user operation for reset USB Device
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_reset(void)
{
    LCD_UsrLog("> Reset the USB device.\n");
}

/*!
    \brief      user operation for detecting device speed
    \param[in]  device_speed: device speed
    \param[out] none
    \retval     none
*/
void usbh_user_device_speed_detected(uint32_t device_speed)
{
    if(PORT_SPEED_HIGH == device_speed) {
        LCD_UsrLog("> High speed device detected.\n");
    } else if(PORT_SPEED_FULL == device_speed) {
        LCD_UsrLog("> Full speed device detected.\n");
    } else if(PORT_SPEED_LOW == device_speed) {
        LCD_UsrLog("> Low speed device detected.\n");
    } else {
        LCD_ErrLog("> Device Fault.\n");
    }
}

/*!
    \brief      user operation when device descriptor is available
    \param[in]  device_desc: device descriptor
    \param[out] none
    \retval     none
*/
void usbh_user_device_desc_available(void *device_desc)
{
    usb_desc_dev *pDevStr = device_desc;

    LCD_DevInformation("VID: %04Xh\n", (uint32_t)pDevStr->idVendor);
    LCD_DevInformation("PID: %04Xh\n", (uint32_t)pDevStr->idProduct);
}

/*!
    \brief      USB device is successfully assigned the Address
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_address_assigned(void)
{
}

/*!
    \brief      user operation when configuration descriptor is available
    \param[in]  cfg_desc: pointer to configuration descriptor
    \param[in]  itf_desc: pointer to interface descriptor
    \param[in]  ep_desc: pointer to endpoint descriptor
    \param[out] none
    \retval     none
*/
void usbh_user_configuration_descavailable(usb_desc_config *cfg_desc, \
        usb_desc_itf *itf_desc, \
        usb_desc_ep *ep_desc)
{
    usb_desc_itf *id = itf_desc;

    if(0x08U == (*id).bInterfaceClass) {
        LCD_UsrLog("> Mass storage device connected.\n");
    } else if(0x02U == (*id).bInterfaceClass) {
        LCD_UsrLog("> CDC class device connected.\n");
    } else if(0x03U == (*id).bInterfaceClass) {
        LCD_UsrLog("> HID device connected.\n");
    }
}

/*!
    \brief      user operation when manufacturer string exists
    \param[in]  manufacturer_string: manufacturer string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_manufacturer_string(void *manufacturer_string)
{
    LCD_DevInformation("Manufacturer: %s\n", (char *)manufacturer_string);
}

/*!
    \brief      user operation when product string exists
    \param[in]  product_string: product string of USB device
    \param[out] none
    \retval     none
*/
void usbh_user_product_string(void *product_string)
{
    LCD_DevInformation("Product: %s\n", (char *)product_string);
}

/*!
    \brief      user operation when serialNum string exists
    \param[in]  serial_num_string: serialNum string of USB device
    \param[out] none
    \retval     none
*/
void usbh_user_serialnum_string(void *serial_num_string)
{
    LCD_DevInformation("Serial Number: %s\n", (char *)serial_num_string);
}

/*!
    \brief      user response request is displayed to ask for application jump to class
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_enumeration_finish(void)
{
    /* enumeration complete */
    LCD_UsrLog("> Enumeration completed.\n");

    lcd_text_color_set(LCD_COLOR_RED);
    lcd_vertical_string_display(LCD_HINT_LINE0, 0U, (uint8_t *)"----------------------------------");
    lcd_text_color_set(LCD_COLOR_GREEN);
    lcd_vertical_string_display(LCD_HINT_LINE1, 0U, (uint8_t *)"To start the CDC demonstration");
    lcd_vertical_string_display(LCD_HINT_LINE2, 0U, (uint8_t *)"Press CET Key...");
}

/*!
    \brief      device is not supported
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_not_supported(void)
{
    LCD_ErrLog("> Device not supported.\n");
}

/*!
    \brief      user action for application state entry
    \param[in]  none
    \param[out] none
    \retval     usbh_usr_status: user response for key button
*/
usbh_user_status usbh_user_userinput(void)
{
    usbh_user_status usbh_usr_status = USR_IN_NO_RESP;

    /* Key B3 is in polling mode to detect user action */
    if(RESET == gd_eval_key_state_get(KEY_CET)) {
        LCD_UsrLog("> CDC Interface started.\n");
        usb_mdelay(500U);
        gd_eval_key_init(KEY_A, KEY_MODE_EXTI);
        gd_eval_key_init(KEY_C, KEY_MODE_EXTI);
        gd_eval_key_init(KEY_CET, KEY_MODE_EXTI);
        usbh_usr_status = USR_IN_RESP_OK;
    }

    return usbh_usr_status;
}

/*!
    \brief      device overcurrent detection event
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_over_current_detected(void)
{
    LCD_ErrLog("Over-current detected\n");
}

/*!
    \brief      deint User state and associated variables
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_deinit(void)
{
}

/*!
    \brief      CDC main application
    \param[in]  none
    \param[out] none
    \retval     none
*/
int usbh_user_application(void)
{
    cdc_handle_menu();

    return 0U;
}

/*!
    \brief      clear LCD screen
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void LCD_clear_part(void)
{
    lcd_text_color_set(LCD_COLOR_BLACK);

    lcd_log_textzone_clear(LCD_TEXT_ZONE_X, \
                           LCD_TEXT_ZONE_Y, \
                           LCD_TEXT_ZONE_WIDTH, \
                           LCD_TEXT_ZONE_HEIGHT);

    LINE = START_LINE;
}

/*!
    \brief      probe the key state
    \param[in]  none
    \param[out] none
    \retval     none
*/
void CDC_DEMO_ProbeKey(uint8_t state)
{
    if(cdc_select_mode == CDC_SELECT_MENU) {
        /* handle menu inputs */
        if((1U == state) && (cdc_demo.select > 0U)) {
            cdc_demo.select--;
        } else if(((USER_CDC_WAIT == cdc_demo.state) && (2U == state) && (cdc_demo.select < 2)) || \
                  ((USER_CDC_SEND == cdc_demo.state) && (2U == state) && (cdc_demo.select < 3)) || \
                  ((USER_CDC_CFG == cdc_demo.state) && (2U == state) && (cdc_demo.select < 1U))) {
            cdc_demo.select++;
        } else if(3U == state) {
            cdc_demo.select |= 0x80U;
        }
    } else {
        /* handle Configuration inputs */
        /* vertical selection */
        if((1U == state) && (cdc_settings_state.select > 0U)) {
            cdc_settings_state.select --;
        } else if((2U == state) && (cdc_settings_state.select < 3U)) {
            cdc_settings_state.select ++;
            /* horizontal selection */
        } else if(3U == state) {
            if((0U == cdc_settings_state.select) && (cdc_settings_state.settings.BaudRateIdx < 9U)) {
                cdc_settings_state.settings.BaudRateIdx++;
            }

            if((1U == cdc_settings_state.select) && (cdc_settings_state.settings.DataBitsIdx < 3U)) {
                cdc_settings_state.settings.DataBitsIdx++;
            }

            if((2U == cdc_settings_state.select) && (cdc_settings_state.settings.ParityIdx < 2U)) {
                cdc_settings_state.settings.ParityIdx++;
            }

            if((3U == cdc_settings_state.select) && (cdc_settings_state.settings.StopBitsIdx < 1U)) {
                cdc_settings_state.settings.StopBitsIdx++;
            }
        } else if(4U == state) {
            if((0U == cdc_settings_state.select) && (cdc_settings_state.settings.BaudRateIdx > 0U)) {
                cdc_settings_state.settings.BaudRateIdx--;
            }

            if((1U == cdc_settings_state.select) && (cdc_settings_state.settings.DataBitsIdx > 0U)) {
                cdc_settings_state.settings.DataBitsIdx--;
            }

            if((2U == cdc_settings_state.select) && (cdc_settings_state.settings.ParityIdx > 0U)) {
                cdc_settings_state.settings.ParityIdx--;
            }

            if((3U == cdc_settings_state.select) && (cdc_settings_state.settings.StopBitsIdx > 0U)) {
                cdc_settings_state.settings.StopBitsIdx--;
            }
        }
    }
}

/*!
    \brief      manage the menu on the screen
    \param[in]  menu: menu table
    \param[in]  item : selected item to be highlighted
    \param[out] none
    \retval     none
*/
static void cdc_select_item(uint8_t **menu, uint8_t item)
{
    lcd_text_color_set(LCD_COLOR_BLACK);
    lcd_log_textzone_clear(LCD_MENU_ZONE_X, \
                           LCD_MENU_ZONE_Y, \
                           LCD_MENU_ZONE_WIDTH, \
                           LCD_MENU_ZONE_HEIGHT);

    lcd_text_color_set(LCD_COLOR_RED);

    switch(item) {
    case 0U:
        lcd_vertical_string_display(LCD_MENU_LINE0, 0U, (uint8_t *)MSG_SEPARATION);
        lcd_vertical_string_display(LCD_MENU_LINE1, 0U, (uint8_t *)menu[0]);
        lcd_vertical_string_display(LCD_MENU_LINE2, 0U, (uint8_t *)MSG_SEPARATION);
        lcd_vertical_string_display(LCD_MENU_LINE3, 0U, (uint8_t *)menu[1]);
        lcd_vertical_string_display(LCD_MENU_LINE4, 0U, (uint8_t *)MSG_SEPARATION);
        lcd_vertical_string_display(LCD_MENU_LINE5, 0U, (uint8_t *)menu[2]);
        lcd_vertical_string_display(LCD_MENU_LINE6, 0U, (uint8_t *)MSG_SEPARATION);

        /* display cursor */
        lcd_text_color_set(LCD_COLOR_BLUE);
        lcd_rectangle_fill(LCD_CURSOR_LINE0, LCD_MENU_Y, 20U, 20U);
        lcd_text_color_set(LCD_COLOR_WHITE);
        lcd_rectangle_fill(LCD_CURSOR_LINE1, LCD_MENU_Y, 20U, 20U);
        lcd_text_color_set(LCD_COLOR_WHITE);
        lcd_rectangle_fill(LCD_CURSOR_LINE2, LCD_MENU_Y, 20U, 20U);
        break;

    case 1U:
        lcd_vertical_string_display(LCD_MENU_LINE0, 0U, (uint8_t *)MSG_SEPARATION);
        lcd_vertical_string_display(LCD_MENU_LINE1, 0U, (uint8_t *)menu[0]);
        lcd_vertical_string_display(LCD_MENU_LINE2, 0U, (uint8_t *)MSG_SEPARATION);
        lcd_vertical_string_display(LCD_MENU_LINE3, 0U, (uint8_t *)menu[1]);
        lcd_vertical_string_display(LCD_MENU_LINE4, 0U, (uint8_t *)MSG_SEPARATION);
        lcd_vertical_string_display(LCD_MENU_LINE5, 0U, (uint8_t *)menu[2]);
        lcd_vertical_string_display(LCD_MENU_LINE6, 0U, (uint8_t *)MSG_SEPARATION);

        /* display cursor */
        lcd_text_color_set(LCD_COLOR_WHITE);
        lcd_rectangle_fill(LCD_CURSOR_LINE0, LCD_MENU_Y, 20U, 20U);
        lcd_text_color_set(LCD_COLOR_BLUE);
        lcd_rectangle_fill(LCD_CURSOR_LINE1, LCD_MENU_Y, 20U, 20U);
        lcd_text_color_set(LCD_COLOR_WHITE);
        lcd_rectangle_fill(LCD_CURSOR_LINE2, LCD_MENU_Y, 20U, 20U);
        break;

    case 2U:
        lcd_vertical_string_display(LCD_MENU_LINE0, 0U, (uint8_t *)MSG_SEPARATION);
        lcd_vertical_string_display(LCD_MENU_LINE1, 0U, (uint8_t *)menu[0]);
        lcd_vertical_string_display(LCD_MENU_LINE2, 0U, (uint8_t *)MSG_SEPARATION);
        lcd_vertical_string_display(LCD_MENU_LINE3, 0U, (uint8_t *)menu[1]);
        lcd_vertical_string_display(LCD_MENU_LINE4, 0U, (uint8_t *)MSG_SEPARATION);
        lcd_vertical_string_display(LCD_MENU_LINE5, 0U, (uint8_t *)menu[2]);
        lcd_vertical_string_display(LCD_MENU_LINE6, 0U, (uint8_t *)MSG_SEPARATION);

        /* display cursor */
        lcd_text_color_set(LCD_COLOR_WHITE);
        lcd_rectangle_fill(LCD_CURSOR_LINE0, LCD_MENU_Y, 20U, 20U);
        lcd_text_color_set(LCD_COLOR_WHITE);
        lcd_rectangle_fill(LCD_CURSOR_LINE1, LCD_MENU_Y, 20U, 20U);
        lcd_text_color_set(LCD_COLOR_BLUE);
        lcd_rectangle_fill(LCD_CURSOR_LINE2, LCD_MENU_Y, 20U, 20U);
        break;

    case 3U:
        lcd_vertical_string_display(LCD_MENU_LINE0, 0U, (uint8_t *)MSG_SEPARATION);
        lcd_vertical_string_display(LCD_MENU_LINE1, 0U, (uint8_t *)menu[1]);
        lcd_vertical_string_display(LCD_MENU_LINE2, 0U, (uint8_t *)MSG_SEPARATION);
        lcd_vertical_string_display(LCD_MENU_LINE3, 0U, (uint8_t *)menu[2]);
        lcd_vertical_string_display(LCD_MENU_LINE4, 0U, (uint8_t *)MSG_SEPARATION);
        lcd_vertical_string_display(LCD_MENU_LINE5, 0U, (uint8_t *)menu[3]);
        lcd_vertical_string_display(LCD_MENU_LINE6, 0U, (uint8_t *)MSG_SEPARATION);

        /* display cursor */
        lcd_text_color_set(LCD_COLOR_WHITE);
        lcd_rectangle_fill(LCD_CURSOR_LINE0, LCD_MENU_Y, 20U, 20U);
        lcd_text_color_set(LCD_COLOR_WHITE);
        lcd_rectangle_fill(LCD_CURSOR_LINE1, LCD_MENU_Y, 20U, 20U);
        lcd_text_color_set(LCD_COLOR_BLUE);
        lcd_rectangle_fill(LCD_CURSOR_LINE2, LCD_MENU_Y, 20U, 20U);
        break;

    case 4U:
        lcd_vertical_string_display(LCD_MENU_LINE0, 0U, (uint8_t *)MSG_SEPARATION);
        lcd_vertical_string_display(LCD_MENU_LINE1, 0U, (uint8_t *)menu[2]);
        lcd_vertical_string_display(LCD_MENU_LINE2, 0U, (uint8_t *)MSG_SEPARATION);
        lcd_vertical_string_display(LCD_MENU_LINE3, 0U, (uint8_t *)menu[3]);
        lcd_vertical_string_display(LCD_MENU_LINE4, 0U, (uint8_t *)MSG_SEPARATION);
        lcd_vertical_string_display(LCD_MENU_LINE5, 0U, (uint8_t *)menu[4]);
        lcd_vertical_string_display(LCD_MENU_LINE6, 0U, (uint8_t *)MSG_SEPARATION);

        /* display cursor */
        lcd_text_color_set(LCD_COLOR_WHITE);
        lcd_rectangle_fill(LCD_CURSOR_LINE0, LCD_MENU_Y, 20U, 20U);
        lcd_text_color_set(LCD_COLOR_WHITE);
        lcd_rectangle_fill(LCD_CURSOR_LINE1, LCD_MENU_Y, 20U, 20U);
        lcd_text_color_set(LCD_COLOR_BLUE);
        lcd_rectangle_fill(LCD_CURSOR_LINE2, LCD_MENU_Y, 20U, 20U);
        break;

    default:
        lcd_vertical_string_display(LCD_MENU_LINE0, 0U, (uint8_t *)MSG_SEPARATION);
        lcd_vertical_string_display(LCD_MENU_LINE1, 0U, (uint8_t *)menu[0]);
        lcd_vertical_string_display(LCD_MENU_LINE2, 0U, (uint8_t *)MSG_SEPARATION);
        lcd_vertical_string_display(LCD_MENU_LINE3, 0U, (uint8_t *)menu[1]);
        lcd_vertical_string_display(LCD_MENU_LINE4, 0U, (uint8_t *)MSG_SEPARATION);
        lcd_vertical_string_display(LCD_MENU_LINE5, 0U, (uint8_t *)menu[2]);
        lcd_vertical_string_display(LCD_MENU_LINE6, 0U, (uint8_t *)MSG_SEPARATION);

        /* display cursor */
        lcd_text_color_set(LCD_COLOR_WHITE);
        lcd_rectangle_fill(LCD_CURSOR_LINE0, LCD_MENU_Y, 20U, 20U);
        lcd_text_color_set(LCD_COLOR_WHITE);
        lcd_rectangle_fill(LCD_CURSOR_LINE1, LCD_MENU_Y, 20U, 20U);
        lcd_text_color_set(LCD_COLOR_WHITE);
        lcd_rectangle_fill(LCD_CURSOR_LINE2, LCD_MENU_Y, 20U, 20U);
        break;
    }
}

/*!
    \brief      demo state machine
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void cdc_handle_menu(void)
{
    static uint8_t debounce = 0U;

    switch(cdc_demo.state) {
    case USER_CDC_IDLE:
        LCD_clear_part();

        lcd_text_color_set(LCD_COLOR_GREEN);
        lcd_vertical_string_display(LCD_HINT_LINE2, 0U, (uint8_t *)"Select an operation to start");

        cdc_select_item(CDC_main_menu, 0U);
        cdc_demo.state = USER_CDC_WAIT;
        cdc_demo.select = 0U;

        cdc_set_initial_value();
        break;

    case USER_CDC_WAIT:
        if(cdc_demo.select != prev_select) {
            prev_select = cdc_demo.select;

            cdc_select_item(CDC_main_menu, cdc_demo.select & 0x7FU);

            /* handle select item */
            if(cdc_demo.select & 0x80U) {
                cdc_demo.select &= 0x7FU;

                switch(cdc_demo.select) {
                case 0U:
                    cdc_demo.state = USER_CDC_SEND;
                    cdc_demo.Send_state = USER_CDC_SEND_IDLE_STATE;
                    break;

                case 1U:
                    cdc_demo.state = USER_CDC_RECEIVE;
                    cdc_demo.Receive_state = USER_CDC_RECV_IDLE;
                    break;

                case 2U:
                    cdc_demo.state = USER_CDC_CFG;
                    cdc_demo.Configuration_state = CDC_CFG_IDLE;
                    break;

                default:
                    break;
                }
            }
        }
        break;

    case USER_CDC_SEND:
        cdc_handle_send_menu();
        break;

    case USER_CDC_RECEIVE:
        cdc_handle_receive_menu();
        break;

    case USER_CDC_CFG:
        cdc_handle_config_menu();
        break;

    default:
        break;
    }

    if(cdc_demo.state == USER_CDC_CFG) {
        if(RESET == gd_eval_key_state_get(KEY_CET)) {
            if(0U == debounce) {
                if(CDC_SELECT_MENU == cdc_select_mode) {
                    cdc_select_mode = CDC_SELECT_CONFIG;
                } else {
                    cdc_select_mode = CDC_SELECT_MENU;
                }

                cdc_change_select_mode(cdc_select_mode);
                debounce = 1U;
            }
        } else {
            debounce = 0U;
        }

        if(CDC_SELECT_CONFIG == cdc_select_mode) {
            cdc_adjust_setting_menu(&cdc_settings_state);
        }
    }
}

/*!
    \brief      handle send menu
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void cdc_handle_send_menu(void)
{
    struct fs_file file = {0U, 0U};

    switch(cdc_demo.Send_state) {
    case USER_CDC_SEND_IDLE_STATE:
        LCD_clear_part();

        lcd_text_color_set(LCD_COLOR_GREEN);
        lcd_vertical_string_display(LCD_HINT_LINE2, 0U, (uint8_t *)"Select a file to send        ");

        LINE = LCD_HINT_LINE;

        cdc_demo.Send_state = USER_CDC_SEND_WAIT_STATE;
        cdc_select_item(DEMO_SEND_menu, 0U);
        cdc_demo.select = 0U;
        break;

    case USER_CDC_SEND_WAIT_STATE:
        if(cdc_demo.select != prev_select) {
            prev_select = cdc_demo.select;
            cdc_select_item(DEMO_SEND_menu, cdc_demo.select & 0x7FU);

            /* handle select item */
            if(cdc_demo.select & 0x80U) {
                cdc_demo.select &= 0x7FU;

                switch(cdc_demo.select) {
                case 0U:
                    Fs_Open((uint8_t *)"file1.txt", &file);
                    cdc_data_send(&usb_host, file.data, file.len);
                    LCD_UsrLog("> 'file1.txt' sent.\n");
                    break;

                case 1U:
                    Fs_Open((uint8_t *)"file2.txt", &file);
                    cdc_data_send(&usb_host, file.data, file.len);
                    LCD_UsrLog("> 'file2.txt' sent.\n");
                    break;

                case 2U:
                    Fs_Open((uint8_t *)"file3.txt", &file);
                    cdc_data_send(&usb_host, file.data, file.len);
                    LCD_UsrLog("> 'file3.txt' sent.\n");
                    break;

                case 3U:
                    cdc_dummydata_send(&usb_host);
                    LCD_UsrLog("> dummy data sent.\n");
                    break;

                case 4U: /* return */
                    cdc_demo.state = USER_CDC_IDLE;
                    cdc_demo.select = 0;
                    break;

                default:
                    break;
                }
            }
        }
        break;

    default:
        break;
    }
}

/*!
    \brief      handle receive menu
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void cdc_handle_receive_menu(void)
{
    switch(cdc_demo.Receive_state) {
    case USER_CDC_RECV_IDLE:
        LCD_clear_part();

        cdc_demo.Receive_state = USER_CDC_RECV_WAIT;
        cdc_select_item(DEMO_RECEIVE_menu, 0U);

        LINE = LCD_HINT_LINE;

        lcd_text_color_set(LCD_COLOR_GREEN);
        lcd_vertical_string_display(LCD_HINT_LINE2, 0U, (uint8_t *)"Received data (ASCII):");

        enable_display_received_data = 1U;
        cdc_start_reception(&usb_host);

        cdc_demo.select = 0U;
        break;

    case USER_CDC_RECV_WAIT:
        if(cdc_demo.select != prev_select) {
            prev_select = cdc_demo.select;
            cdc_select_item(DEMO_RECEIVE_menu, cdc_demo.select & 0x7FU);

            /* handle select item */
            if(cdc_demo.select & 0x80U) {
                cdc_demo.select &= 0x7FU;

                switch(cdc_demo.select) {
                /* return */
                case 0U:
                    cdc_demo.state = USER_CDC_IDLE;
                    cdc_demo.select = 0U;
                    enable_display_received_data = 0U;
                    cdc_stop_reception(&usb_host);
                    break;

                default:
                    break;
                }
            }
        }
        break;

    default:
        break;
    }
}

/*!
    \brief      handle configuration menu
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void cdc_handle_config_menu(void)
{
    switch(cdc_demo.Configuration_state) {
    case CDC_CFG_IDLE:
        cdc_settings_state.select = 0U;
        cdc_select_settings_item(0xFFU);
        cdc_demo.Configuration_state = CDC_CFG_WAIT;
        cdc_select_item(DEMO_CONFIGURATION_menu, 0U);
        cdc_demo.select = 0U;
        break;

    case CDC_CFG_WAIT:
        if(cdc_demo.select != prev_select) {
            prev_select = cdc_demo.select;
            cdc_select_item(DEMO_CONFIGURATION_menu, cdc_demo.select & 0x7FU);

            /* handle select item */
            if(cdc_demo.select & 0x80U) {
                cdc_demo.select &= 0x7FU;

                switch(cdc_demo.select) {
                case 0U:
                    cdc_change_state_to_issue_setconfig(&usb_host);
                    break;

                case 1U:
                    /* return */
                    cdc_demo.state = USER_CDC_IDLE;
                    cdc_demo.select = 0;
                    break;

                default:
                    break;
                }
            }
        }
        break;

    default:
        break;
    }
}

/*!
    \brief      change the selection mode
    \param[in]  select_mode: selection mode
    \param[out] none
    \retval     none
*/
static void cdc_change_select_mode(cdc_demo_select_mode select_mode)
{
    if(CDC_SELECT_CONFIG == select_mode) {
        cdc_select_item(DEMO_CONFIGURATION_menu, 0xFFU);
        cdc_settings_state.select = 0U;
        cdc_select_settings_item(0U);
    } else {
        cdc_select_item(DEMO_CONFIGURATION_menu, 0U);
        cdc_select_settings_item(0xFFU);
    }
}

/*!
    \brief      manage the setting menu on the screen
    \param[in]  settings_state: setting state
    \param[out] none
    \retval     none
*/
static void cdc_adjust_setting_menu(cdc_demo_setting_state_machine *settings_state)
{
    uint8_t str_temp[40U];
    usbh_cdc_handler *cdc = (usbh_cdc_handler *)usb_host.active_class->class_data;

    if(settings_state->select != prev_select) {
        prev_select = settings_state->select;
        cdc_select_settings_item(settings_state->select);
    }

    if(settings_state->settings.BaudRateIdx != prev_baud_rate_idx) {
        prev_baud_rate_idx = settings_state->settings.BaudRateIdx;

        if(BaudRateValue[settings_state->settings.BaudRateIdx] <= 9600U) {
            sprintf((char *)str_temp, "       %d", BaudRateValue[settings_state->settings.BaudRateIdx]);
        } else if(BaudRateValue[settings_state->settings.BaudRateIdx] <= 57600U) {
            sprintf((char *)str_temp, "       %d", BaudRateValue[settings_state->settings.BaudRateIdx]);
        } else {
            sprintf((char *)str_temp, "       %d", BaudRateValue[settings_state->settings.BaudRateIdx]);
        }

        cdc->line_code_set.b.dwDTERate = BaudRateValue[settings_state->settings.BaudRateIdx];
        cdc->line_code_get.b.dwDTERate = cdc->line_code_set.b.dwDTERate;
    }

    if(settings_state->settings.DataBitsIdx != prev_data_bits_idx) {
        prev_data_bits_idx = settings_state->settings.DataBitsIdx;
        sprintf((char *)str_temp, "            %d", DataBitsValue[settings_state->settings.DataBitsIdx]);
        cdc->line_code_set.b.bDataBits = DataBitsValue[settings_state->settings.DataBitsIdx];
        cdc->line_code_get.b.bDataBits = cdc->line_code_set.b.bDataBits;
    }

    if(settings_state->settings.ParityIdx != prev_parity_idx) {
        prev_parity_idx = settings_state->settings.ParityIdx;

        if(2U == settings_state->settings.ParityIdx) {
            sprintf((char *)str_temp, "    %s", (uint8_t *)ParityArray[settings_state->settings.ParityIdx]);
        } else {
            sprintf((char *)str_temp, "    %s", (uint8_t *)ParityArray[settings_state->settings.ParityIdx]);
        }

        cdc->line_code_set.b.bParityType = settings_state->settings.ParityIdx;
        cdc->line_code_get.b.bParityType = cdc->line_code_set.b.bParityType;
    }

    if(settings_state->settings.StopBitsIdx != prev_stop_bits_idx) {
        prev_stop_bits_idx = settings_state->settings.StopBitsIdx;
        sprintf((char *)str_temp, "            %s", StopBitsArray[settings_state->settings.StopBitsIdx]);
        cdc->line_code_set.b.bCharFormat = settings_state->settings.StopBitsIdx;
        cdc->line_code_get.b.bCharFormat = cdc->line_code_set.b.bCharFormat;
    }
}

/*!
    \brief      manage the setting menu item on the screen
    \param[in]  item: selected item to be highlighted
    \param[out] none
    \retval     none
*/
static void cdc_select_settings_item(uint8_t item)
{
    uint8_t str_temp[40U];
    usbh_cdc_handler *cdc = (usbh_cdc_handler *)usb_host.active_class->class_data;

    switch(item) {
    case 0U: {
        if(cdc->line_code_get.b.dwDTERate <= 9600U) {
            sprintf((char *)str_temp, "       %d", cdc->line_code_get.b.dwDTERate);
        } else if(cdc->line_code_get.b.dwDTERate <= 57600U) {
            sprintf((char *)str_temp, "       %d", cdc->line_code_get.b.dwDTERate);
        } else {
            sprintf((char *)str_temp, "       %d", cdc->line_code_get.b.dwDTERate);
        }

        sprintf((char *)str_temp, "            %d", cdc->line_code_get.b.bDataBits);

        /* display the parity bits */
        if(2U == cdc->line_code_get.b.bParityType) {
            sprintf((char *)str_temp, "         %s", ParityArray[cdc->line_code_get.b.bParityType]);
        } else {
            sprintf((char *)str_temp, "         %s", ParityArray[cdc->line_code_get.b.bParityType]);
        }

        /* display the Stop bits */
        sprintf((char *)str_temp, "            %s", StopBitsArray[cdc->line_code_get.b.bCharFormat]);
    }
    break;

    case 1U: {
        if(cdc->line_code_get.b.dwDTERate <= 9600U) {
            sprintf((char *)str_temp, "       %d", cdc->line_code_get.b.dwDTERate);
        } else if(cdc->line_code_get.b.dwDTERate <= 57600U) {
            sprintf((char *)str_temp, "       %d", cdc->line_code_get.b.dwDTERate);
        } else {
            sprintf((char *)str_temp, "       %d", cdc->line_code_get.b.dwDTERate);
        }

        /* display the data bits */
        sprintf((char *)str_temp, "           %d", cdc->line_code_get.b.bDataBits);

        /* display the parity bits */
        if(2U == cdc->line_code_get.b.bParityType) {
            sprintf((char *)str_temp, "         %s", ParityArray[cdc->line_code_get.b.bParityType]);
        } else {
            sprintf((char *)str_temp, "         %s", ParityArray[cdc->line_code_get.b.bParityType]);
        }

        /* display the stop bits */
        sprintf((char *)str_temp, "            %s", StopBitsArray[cdc->line_code_get.b.bCharFormat]);
    }
    break;

    case 2U: {
        if(cdc->line_code_get.b.dwDTERate <= 9600U) {
            sprintf((char *)str_temp, "         %d", cdc->line_code_get.b.dwDTERate);
        } else if(cdc->line_code_get.b.dwDTERate <= 57600U) {
            sprintf((char *)str_temp, "        %d", cdc->line_code_get.b.dwDTERate);
        } else {
            sprintf((char *)str_temp, "       %d", cdc->line_code_get.b.dwDTERate);
        }

        if(2U == cdc->line_code_get.b.bParityType) {
            sprintf((char *)str_temp, "         %s", ParityArray[cdc->line_code_get.b.bParityType]);
        } else {
            sprintf((char *)str_temp, "         %s", ParityArray[cdc->line_code_get.b.bParityType]);
        }

        sprintf((char *)str_temp, "            %s", StopBitsArray[cdc->line_code_get.b.bCharFormat]);
    }
    break;

    case 3U: {
        if(cdc->line_code_get.b.dwDTERate <= 9600U) {
            sprintf((char *)str_temp, "       %d", cdc->line_code_get.b.dwDTERate);
        } else if(cdc->line_code_get.b.dwDTERate <= 57600U) {
            sprintf((char *)str_temp, "       %d", cdc->line_code_get.b.dwDTERate);
        } else {
            sprintf((char *)str_temp, "       %d", cdc->line_code_get.b.dwDTERate);
        }

        /* display the data bits */
        sprintf((char *)str_temp, "            %d", cdc->line_code_get.b.bDataBits);

        /* display the parity bits */
        if(2U == cdc->line_code_get.b.bParityType) {
            sprintf((char *)str_temp, "         %s", ParityArray[cdc->line_code_get.b.bParityType]);
        } else {
            sprintf((char *)str_temp, "         %s", ParityArray[cdc->line_code_get.b.bParityType]);
        }

        /* display the stop bits */
        sprintf((char *)str_temp, "            %s", StopBitsArray[cdc->line_code_get.b.bCharFormat]);
    }
    break;

    default:
        if(cdc->line_code_get.b.dwDTERate <= 9600U) {
            sprintf((char *)str_temp, "     %d", cdc->line_code_get.b.dwDTERate);
        } else if(cdc->line_code_get.b.dwDTERate <= 57600U) {
            sprintf((char *)str_temp, "     %d", cdc->line_code_get.b.dwDTERate);
        } else {
            sprintf((char *)str_temp, "       %d", cdc->line_code_get.b.dwDTERate);
        }

        /* display the data bits */
        sprintf((char *)str_temp, "            %d", cdc->line_code_get.b.bDataBits);

        /* display the parity bits */
        if(2U == cdc->line_code_get.b.bParityType) {
            sprintf((char *)str_temp, "         %s", ParityArray[cdc->line_code_get.b.bParityType]);
        } else {
            sprintf((char *)str_temp, "         %s", ParityArray[cdc->line_code_get.b.bParityType]);
        }

        /* display the stop bits */
        sprintf((char *)str_temp, "            %s", StopBitsArray[cdc->line_code_get.b.bCharFormat]);
        break;
    }
}

/*!
    \brief      displays a maximum of 40 small font char on the LCD
    \param[in]  ptr: pointer to string to display on LCD
    \param[out] none
    \retval     none
*/
void cdc_output_data(uint8_t *ptr)
{
    if(0U != enable_display_received_data) {
        LCD_UsrLog("%s\n", ptr);
    }
}

/*!
    \brief      set the CDC demo initial values
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void cdc_set_initial_value(void)
{
    usbh_cdc_handler *cdc = (usbh_cdc_handler *)usb_host.active_class->class_data;

    /* set the initial value */
    cdc->line_code_set.b.dwDTERate = cdc->line_code_get.b.dwDTERate;
    cdc->line_code_set.b.bDataBits = cdc->line_code_get.b.bDataBits;
    cdc->line_code_set.b.bParityType = cdc->line_code_get.b.bParityType;
    cdc->line_code_set.b.bCharFormat = cdc->line_code_get.b.bCharFormat;
    cdc_change_state_to_issue_setconfig(&usb_host);
    cdc->user_cb.receive = cdc_output_data;

    /* initialize baud rate index accordingly */
    switch(cdc->line_code_set.b.dwDTERate) {
    case 2400U:
        cdc_settings_state.settings.BaudRateIdx = 0U;
        break;

    case 4800U:
        cdc_settings_state.settings.BaudRateIdx = 1U;
        break;

    case 9600U:
        cdc_settings_state.settings.BaudRateIdx = 2U;
        break;

    case 19200U:
        cdc_settings_state.settings.BaudRateIdx = 3U;
        break;

    case 38400U:
        cdc_settings_state.settings.BaudRateIdx = 4U;
        break;

    case 57600U:
        cdc_settings_state.settings.BaudRateIdx = 5U;
        break;

    case 115200U:
        cdc_settings_state.settings.BaudRateIdx = 6U;
        break;

    case 230400U:
        cdc_settings_state.settings.BaudRateIdx = 7U;
        break;

    case 460800U:
        cdc_settings_state.settings.BaudRateIdx = 8U;
        break;

    case 921600U:
        cdc_settings_state.settings.BaudRateIdx = 9U;
        break;

    default:
        break;
    }

    /* initialize data bits index accordingly */
    switch(cdc->line_code_set.b.bDataBits) {
    case 5U:
        cdc_settings_state.settings.DataBitsIdx = 0U;
        break;

    case 6U:
        cdc_settings_state.settings.DataBitsIdx = 1U;
        break;

    case 7U:
        cdc_settings_state.settings.DataBitsIdx = 2U;
        break;

    case 8U:
        cdc_settings_state.settings.DataBitsIdx = 3U;
        break;

    default:
        break;
    }

    /* initialize stop bits index accordingly */
    switch(cdc->line_code_set.b.bCharFormat) {
    case 1U:
        cdc_settings_state.settings.StopBitsIdx = 0U;
        break;

    case 2U:
        cdc_settings_state.settings.StopBitsIdx = 1U;
        break;

    default:
        break;
    }

    /* initialize parity index accordingly */
    switch(cdc->line_code_set.b.bParityType) {
    case 0U:
        cdc_settings_state.settings.ParityIdx = 0U;
        break;

    case 1U:
        cdc_settings_state.settings.ParityIdx = 1U;
        break;

    case 2U:
        cdc_settings_state.settings.ParityIdx = 2U;
        break;

    default:
        break;
    }

    prev_baud_rate_idx = cdc_settings_state.settings.BaudRateIdx;
    prev_data_bits_idx = cdc_settings_state.settings.DataBitsIdx;
    prev_parity_idx = cdc_settings_state.settings.StopBitsIdx;
    prev_stop_bits_idx = cdc_settings_state.settings.ParityIdx;
    prev_select = 0U;
}
