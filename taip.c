#include "taip.h"

int taip_generate_pv(taip_msg_pv_t* pv, char* buf){
    char* pos;
    strcpy(buf, ">RPV");
    pos = buf + strlen(buf);
    sprintf(pos, "%05d%+08d%+09d%03d%03d",
        pv->time_of_day_sec, pv->latitude_dd5, pv->longitude_dd5,pv->speed_mph, pv->heading_deg);
    pos = buf + strlen(buf);
    sprintf(pos, "%1d%1d;ID=%s;*", pv->source, pv->age, pv->id);
    pos = buf + strlen(buf);
    uint8_t checksum = minmea_checksum(buf);
    sprintf(pos,"%02X<", checksum);
}