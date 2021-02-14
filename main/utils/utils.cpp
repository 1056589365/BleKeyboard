#include "utils.h"

float map_v(float val, float I_Min, float I_Max, float O_Min, float O_Max)
{
	return(((val-I_Min)*((O_Max-O_Min)/(I_Max-I_Min)))+O_Min);
}