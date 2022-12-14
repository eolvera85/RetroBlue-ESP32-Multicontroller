#include "retroblue_backend.h"

GamepadButtonData g_button_data = {};
GamepadStickData g_stick_data = {};

// Resets all button data set
void rb_button_reset()
{
    g_button_data.b_right  = 0;
    g_button_data.b_down   = 0;
    g_button_data.b_up     = 0;
    g_button_data.b_left   = 0;

    g_button_data.b_home     = 0;
    g_button_data.b_start     = 0;
    g_button_data.t_r        = 0;
    g_button_data.t_zr        = 0;

    g_button_data.d_left         = 0;
    g_button_data.d_right        = 0;
    g_button_data.b_select    = 0;
    g_button_data.b_capture      = 0;

    g_button_data.t_l    = 0;
    g_button_data.t_zl    = 0;
    g_button_data.d_up       = 0;
    g_button_data.d_down     = 0;
}
