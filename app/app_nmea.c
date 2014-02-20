#include <stdint.h>
#include<string.h>

#include "proto.h"
#include "app_gps.h"
#include "app_nmea.h"

#if ((defined (CAR_DB44_V1_0_20130315_)) || defined (DouLunJi_CAR_GBC_V1_2_130511_))

struct gps_device_t session;

int32_t app_nmea_parse(void *pgps)
{
	int32_t flag = -1;
	int32_t nmea_ret;

	session.packet.type = NMEA_PACKET;
	memcpy((void *)session.packet.outbuffer, (void *)pgps, strlen((void *)pgps));
	session.packet.outbuffer[strlen((void *)pgps)] = '\0';
	session.packet.outbuflen = strlen((void *)pgps);

	nmea_ret = aivdm_analyze(&session);

	if(nmea_ret)
	{
		switch(session.nmea.type) {
		case TYPE_GPGGA:
			//save_gpgga(session.nmea.nmea_u.gpgga.nsv);
			flag = 2;
			break;
		case TYPE_GPRMC:
			if (STATUS_FIX == session.nmea.nmea_u.gprmc.status )
				app_gps_gprmc_save(&session.nmea.nmea_u.gprmc);
			flag = 1;
			break;
		default:
			break;

		}

	}

	return flag;
}

#endif
