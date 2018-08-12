#ifndef MEW_NMEA_H
#define MEW_NMEA_H

#include "stdint.h"

//$GNGLL,4543.4614,N,12636.4216,E,044025.000,A,A*40	
typedef struct mew_GNGLL_Data
{	
	double Latitude;
	double Longitude;			
	double UTC;
	uint8_t Vaild;
}mew_GNGLL_Data;

int8_t mew_NMEA_Parse_GNGLL(char *in, mew_GNGLL_Data *glldata);
#endif
