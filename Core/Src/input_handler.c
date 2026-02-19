#include <stdio.h>
#include "garland.h"

void handle_input(uint8_t input) {
	if (!is_editing(input)) {
		  switch (input) {
		  case 0:
			  next_mode();
			  break;
		  case 3:
			  prev_mode();
			  break;
		  case 2:
			  speed_up();
			  break;
		  case 5:
			  speed_down();
			  break;
		  case 7:
			  format_user_mode_info();
			  break;
		  case 11:
			  start_editing();
			  break;
		  default:
			  sprintf(info_buf, "Unknown command\r\n");
			  break;
		  }
	  } else {
		  switch (input) {
		  case 0:
			  e_light_up();
			  break;
		  case 3:
			  e_light_down();
			  break;
		  case 2:
			  e_switch_color();
			  break;
		  case 5:
			  e_switch_seq();
			  break;
		  case 7:
			  format_user_mode_info();
			  break;
		  case 11:
			  finish_editing();
			  break;
		  default:
			  sprintf(info_buf, "Unknown command\r\n");
			  break;
		  }
	  }
}
