#ifndef _TAIP_H_
#define _TAIP_H_

#include "minmea.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define TAIP_ID_MAX 16

#define TAIP_MAX_LEN 100


enum taip_age{
    TAIP_AGE_FRESH = 2,
    TAIP_AGE_OLD = 1,
    TAIP_AGE_INVALID = 0
};
typedef enum taip_age taip_age_e;

enum taip_source{
    TAIP_SOURCE_2DGPS = 0,
    TAIP_SOURCE_3DGPS = 1,
    TAIP_SOURCE_2DDGPS = 2,
    TAIP_SOURCE_3DDGPS = 3,
    TAIP_SOURCE_DR = 6,
    TAIP_SOURCE_DEGRADEDDR = 8,
    TAIP_SOURCE_UNKNOWN = 9
};
typedef enum taip_source taip_source_e;

typedef struct {
    int time_of_day_sec;
    int latitude_dd5;
    int longitude_dd5;
    int speed_mph;
    int heading_deg;
    taip_source_e source;
    taip_age_e age;
    char id[TAIP_ID_MAX];
} taip_msg_pv_t;

int taip_generate_pv(taip_msg_pv_t* pv, char* buf);

#endif