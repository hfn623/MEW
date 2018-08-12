#include "mew_nmea.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"

char *mew_NEMA_Get_comma_pos(char *in, uint8_t skip)
{
	while(*in)
	{
		if(*in != ',')
		{
			*in++;
		}
		else
		{
			if(skip)
			{
				skip--;
				*in++;
			}
			else
			{
				return in;
			}
		}
	}
	return NULL;
}
double mew_NMEA_string2double(char *in)
{
	char *p;
	double bp, ap;
	char splite[2][16] = {0};
	uint8_t len;
	uint32_t factor;
	
	len = strlen(in);
	p = strchr(in, '.');
	if(p != NULL)
	{
		memcpy(splite[0], in, p - in);
		memcpy(splite[1], p + 1, len - (p - in));
		
		bp = atoi(splite[0]);
		ap = atoi(splite[1]);
		
		factor = pow(10, len - (p - in) - 1);
		ap /= factor;
		bp += ap;
		
	}
	else
	{
		bp = atoi(in);
	}
	
	return bp;
}
int8_t mew_NMEA_Parse_GNGLL(char *in, mew_GNGLL_Data *glldata)
{
	//$GNGLL,4543.4614,N,12636.4216,E,044025.000,A,A*40
	char out[6][16] = {0};
	
	char *pbase = NULL;
	char *word_begin, *word_end;
	
	uint8_t i;
	
	pbase = strstr(in, "$GNGLL");
	
	if(pbase == NULL)
	{
		return -1;
	}
	
	word_end = NULL;
	for(i = 0; i < 6; i++)
	{
		if(word_end == NULL)
		{
			word_begin = mew_NEMA_Get_comma_pos(pbase, i);
		}
		else
		{
			word_begin = word_end;
		}
		word_end = mew_NEMA_Get_comma_pos(pbase, i + 1);
		
		if(word_begin == NULL || word_end == NULL)
		{
			return -2;
		}
		
		memcpy(out[i], word_begin + 1, word_end - word_begin - 1);
	}
	
	glldata->Latitude = mew_NMEA_string2double(out[0]);
	glldata->Longitude = mew_NMEA_string2double(out[2]);
	glldata->UTC = mew_NMEA_string2double(out[4]);
	glldata->Vaild = out[5][0] == 'A' ? 1:0;
	return 0;
}
