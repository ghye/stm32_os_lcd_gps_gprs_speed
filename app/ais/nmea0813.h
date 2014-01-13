#ifndef __NMEA0813_H__
#define __NMEA0813_H__

#include "proto.h"
#include "common.h"

int32_t nmea_parse(int8_t *buf, struct gps_device_t *session);

#endif
