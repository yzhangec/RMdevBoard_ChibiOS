#include "keyboard.h"
#include "canBusProcess.h"
#include "ch.h"
#include "hal.h"
#include "dbus.h"
int bitmap[15] = {};
/* mouse button long press time */
#define LONG_PRESS_TIME  1000  //ms
/* key acceleration time */
#define KEY_ACC_TIME     1500  //ms

kb_ctrl_t km;

void keyboard_to_bitmap(Gimbal_Send_Dbus_canStruct* pRC){
  uint8_t i = 0;
  //uint32_t n = RC_get()->keyboard.key_code;
  uint32_t n = pRC->key_code;
  for(int j=0 ; j< 15; j++){
    bitmap[i] = n % 2;
    n = n/2;
    i++;
  }
}
bool keyboard_enable(Gimbal_Send_Dbus_canStruct* pRC){
  return (pRC->s1 == MI);
  //return RC_get()->rc.s2 == UP;
}
void keyboard_reset(){
  km.vx = 0;
  km.vy = 0;
  km.vw = 0;
  km.twist_ctrl = 0;
}
static void move_speed_ctrl(uint8_t fast, uint8_t slow)
{
  if (fast)
  {
    km.move = FAST_MODE;

    km.x_spd_limit = 0.4f * CHASSIS_KB_MAX_SPEED_X ;
    km.y_spd_limit = 0.4f * CHASSIS_KB_MAX_SPEED_Y ;

  }
  else if (slow)
  {
    km.move = SLOW_MODE;

    km.x_spd_limit = 0.2f * CHASSIS_KB_MAX_SPEED_X ;
    km.y_spd_limit = 0.2f * CHASSIS_KB_MAX_SPEED_Y ;

  }
  else
  {
    km.move = NORMAL_MODE;

    km.x_spd_limit = 0.3f * CHASSIS_KB_MAX_SPEED_X ;
    km.y_spd_limit = 0.3f * CHASSIS_KB_MAX_SPEED_Y ;

  }
}

static void move_direction_ctrl(uint8_t forward, uint8_t back,
                                uint8_t left,    uint8_t right)
{
  //add ramp
  if (forward)
  {
    km.vy = km.y_spd_limit;
  }
  else if (back)
  {
    km.vy = -km.y_spd_limit;
  }
  else
  {
    km.vy = 0;
    //ramp_init(&fb_ramp, KEY_ACC_TIME/INFO_GET_PERIOD);
  }

  if (left)
  {
    km.vx = -km.x_spd_limit ;
  }
  else if (right)
  {
    km.vx = km.x_spd_limit;
  }
  else
  {
    km.vx = 0;
  }

  if (forward || back || left || right)
    km.twist_ctrl = 0;
}

void keyboard_chassis_process(chassisStruct* chassisP,Gimbal_Send_Dbus_canStruct* pRC){
    keyboard_to_bitmap(pRC);


    if(bitmap[KEY_R]){
      chassisP->ctrl_mode = DODGE_MODE;
    }
    else if(bitmap[KEY_C]){
      chassisP->ctrl_mode = MANUAL_SEPARATE_GIMBAL;
    }
    else{
      chassisP->ctrl_mode = MANUAL_FOLLOW_GIMBAL;
      move_speed_ctrl(bitmap[KEY_SHIFT], bitmap[KEY_CTRL]);
      move_direction_ctrl(bitmap[KEY_W], bitmap[KEY_S], bitmap[KEY_A], bitmap[KEY_D]);
    }

   // chassis_operation_func(bitmap);

}



