#include "rtl_433.h"
#include "util.h"
/* Inovalley kw9015b rain and Temperature weather station
 *
 * Copyright (C) 2015 Alexandre Coffignal
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

extern uint8_t reverse8(uint8_t x);

static int kw9015b_callback(bitbuffer_t *bitbuffer) {
	data_t *data;
	bitrow_t *bb = bitbuffer->bb;


	int i, iRain, device;
	unsigned char chksum;
	float fTemp;

	for (i = 0; i < 5; i++) {
		if (bitbuffer->bits_per_row[i] != 36) {
			/*10 24 bits frame*/
		}
		else 
		{
			//AAAAAAAA BBBBBBBB BBBBBBBB CCCCCCCC DDDD
			//A : ID
			//B : Temp
			//C : Rain
			//D : checksum

			device = reverse8(bb[i][0]);
			fTemp = (float)((signed short)(reverse8(bb[i][2]) * 256 + reverse8(bb[i][1]))) / 160;
			iRain = reverse8(bb[i][3]);
			chksum = ((reverse8(bb[i][0]) >> 4) + (reverse8(bb[i][0]) & 0x0F) +
				(reverse8(bb[i][1]) >> 4) + (reverse8(bb[i][1]) & 0x0F) +
				(reverse8(bb[i][2]) >> 4) + (reverse8(bb[i][2]) & 0x0F) +
				(reverse8(bb[i][3]) >> 4) + (reverse8(bb[i][3]) & 0x0F));

			if ((chksum & 0x0F) == (reverse8(bb[i][4]) & 0x0F)) {
				char time_str[LOCAL_TIME_BUFLEN];
				local_time_str(0, time_str);

				char rx_data[32] = { 0 };

				sprintf(&rx_data[0], "%02X %02X %02X %02X %02X", reverse8(bb[i][0]),
					reverse8(bb[i][1]),
					reverse8(bb[i][2]),
					reverse8(bb[i][3]),
					reverse8(bb[i][4]));

				data = data_make("time", "", DATA_STRING, time_str,
					"model", "", DATA_STRING, "Inovalley kw9015b Temperature and rain",
					"Device", "", DATA_INT, device,
					"temperature_C", "Temperature", DATA_FORMAT, "%.1f C", DATA_DOUBLE, fTemp,
					"rain", "Rain       ", DATA_FORMAT, "%d", DATA_INT, iRain,
					"checksum", "Checksum   ", DATA_FORMAT, "%02X", DATA_INT, chksum & 0xF,
					"data", "Data       ", DATA_STRING, rx_data,
					NULL);
				data_acquired_handler(data);

				return 1;
			}
		}
	}


	return 0;

}


static char *output_fields[] = {
	"time",
	"model",
	"Device",
	"temperature_C",
	"rain",
	"checksum",
	"data",
	NULL
};


r_device kw9015b = {
  .name = "Inovalley kw9015b rain and Temperature weather station",
  .modulation = OOK_PULSE_PPM_RAW,
  .short_limit = 3500,
  .long_limit = 4800,
  .reset_limit = 10000,
  .json_callback = &kw9015b_callback,
  .disabled = 1,
  .demod_arg = 0,
  .fields = output_fields
};
