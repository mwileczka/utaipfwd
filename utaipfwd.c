#define _DEFAULT_SOURCE 

// C library headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()




#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#include "minmea.h"
#include "taip.h"
#include "ini.h"

int get_baud(int baud);

typedef struct minmea_float minmea_f_t;

time_t time_last_send = 0;

float last_tx_lat_d5 = 0;
float last_tx_long_d5 = 0;

typedef struct
{
    const char* id;
    const char* server_ip;
    uint16_t server_port;
    uint32_t max_wait_move_s;
    uint32_t min_move_d5;
    uint32_t min_speed_mph;
    uint32_t min_tx_delay_s;
    const char* gps_port;
    uint32_t gps_baud;
} configuration;

configuration config;

static int config_handler(void* user, const char* section, const char* name, const char* value)
{
    configuration* pconfig = (configuration*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (MATCH("device", "id")) {
        pconfig->id = strdup(value);
    } else if (MATCH("server", "ip")) {
        pconfig->server_ip = strdup(value);
    } else if (MATCH("server", "port")) {
        pconfig->server_port = atol(value);
    } else if (MATCH("timing", "max_wait_move_s")) {
        pconfig->max_wait_move_s = atol(value);
    } else if (MATCH("timing", "min_move_d5")) {
        pconfig->min_move_d5 = atol(value);
    } else if (MATCH("timing", "min_speed_mph")) {
        pconfig->min_speed_mph = atol(value);
    } else if (MATCH("timing", "min_tx_delay_s")) {
        pconfig->min_tx_delay_s = atol(value);
    } else if (MATCH("gps", "port")) {
        pconfig->gps_port = strdup(value);
    } else if (MATCH("gps", "baud")) {
        pconfig->gps_baud = get_baud(atol(value));
    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}


int main(int argc, char* argv[]){

    

    if(ini_parse("utaipfwd.ini", config_handler, &config) < 0){
        printf("Can't load config\n");
        exit(1);
    }



    struct sockaddr_in server_addr;
    inet_aton(config.server_ip, &server_addr.sin_addr);
    server_addr.sin_port = htons(config.server_port);
    server_addr.sin_family = AF_INET;
  
    int udpfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);



 





    int gpsfd = open(config.gps_port, O_RDONLY | O_NOCTTY | O_SYNC);
    struct termios tty;

    bzero(&tty, sizeof(struct termios));

    cfsetispeed(&tty, config.gps_baud);
    cfsetospeed(&tty, config.gps_baud);

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE; // Clear all the size bits, then use one of the statements below
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag |= ICANON; // SPECIAL - line processing mode
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    // https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/
    tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VEOL] = 0;
    tty.c_cc[VEOL2] = 0;
    tty.c_cc[VEOF] = 0x04;

    // Save tty settings, also checking for error
    if (tcsetattr(gpsfd, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        exit(1);
    }


    char read_buf [MINMEA_MAX_SENTENCE_LENGTH];
    while(1){
        int rdlen = read(gpsfd, read_buf, sizeof(read_buf) - 1);
        if(rdlen <= 0) break;

        read_buf[rdlen] = 0;
        printf("RECV: %s\n", read_buf);

        switch (minmea_sentence_id(read_buf, false)) {
        case MINMEA_SENTENCE_RMC: {
            struct minmea_sentence_rmc frame;
            if (minmea_parse_rmc(&frame, read_buf)) {
                /* printf("$RMC: raw coordinates and speed: (%d/%d,%d/%d) %d/%d\n",
                        frame.latitude.value, frame.latitude.scale,
                        frame.longitude.value, frame.longitude.scale,
                        frame.speed.value, frame.speed.scale);
                printf("$RMC fixed-point coordinates and speed scaled to three decimal places: (%d,%d) %d\n",
                        minmea_rescale(&frame.latitude, 1000),
                        minmea_rescale(&frame.longitude, 1000),
                        minmea_rescale(&frame.speed, 1000));
                printf("$RMC floating point degree coordinates and speed: (%f,%f) %f\n",
                        minmea_tocoord(&frame.latitude),
                        minmea_tocoord(&frame.longitude),
                        minmea_tofloat(&frame.speed));

 */
                if(frame.valid){
                    bool send_it = false;

                    taip_msg_pv_t pv;
                    bzero(&pv, sizeof(taip_msg_pv_t));
                    pv.latitude_dd5 = minmea_tocoord(&frame.latitude) * 100000;
                    pv.longitude_dd5 = minmea_tocoord(&frame.longitude) * 100000;
                    pv.speed_mph = (int) (minmea_tofloat(&frame.speed) * 1.15078);


                    time_t tx_delta_s = time(NULL) - time_last_send;

                    if(abs(last_tx_lat_d5 - pv.latitude_dd5) > config.min_move_d5 || abs(last_tx_long_d5 - pv.longitude_dd5) > config.min_move_d5){
                        printf("TX move threshold\n");
                        send_it = true;
                    }

                    if(pv.speed_mph > config.min_speed_mph){
                        printf("TX speed threshold\n");
                        send_it = true;
                    }

                    if(tx_delta_s >= config.max_wait_move_s){
                        send_it = true;
                        printf("TX max wait\n");
                    }
                    
                    if(send_it && tx_delta_s < config.min_tx_delay_s){
                        send_it = false;
                        printf("TX CANCELLED not min wait time\n");
                    }

                    if(send_it){
                        time_last_send = time(NULL);
                        last_tx_lat_d5 = pv.latitude_dd5;
                        last_tx_long_d5 = pv.longitude_dd5;

                        pv.age = TAIP_AGE_FRESH;
                        strcpy(pv.id, config.id);
                        pv.heading_deg = (int) minmea_tofloat(&frame.course);
                        pv.source = TAIP_SOURCE_3DGPS;
                        pv.time_of_day_sec = (((frame.time.hours * 60) + frame.time.minutes) * 60) + frame.time.seconds;

                        char outbuf[TAIP_MAX_LEN];
                        int outlen = taip_generate_pv(&pv, outbuf);

                        printf("%s\n", outbuf);

                        sendto(udpfd, outbuf, strlen(outbuf), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
                    }else{

                    }
                }else{
                    printf("INVALID\n");
                }
                


            }
        } break;
        }
    }

    close(gpsfd);
    return 0;
}

int get_baud(int baud)
{
    switch (baud) {
    case 1200:
        return B1200;
    case 2400:
        return B2400;
    case 4800:
        return B4800;
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;
    case 460800:
        return B460800;
    case 500000:
        return B500000;
    case 576000:
        return B576000;
    case 921600:
        return B921600;
    case 1000000:
        return B1000000;
    case 1152000:
        return B1152000;
    case 1500000:
        return B1500000;
    case 2000000:
        return B2000000;
    case 2500000:
        return B2500000;
    case 3000000:
        return B3000000;
    case 3500000:
        return B3500000;
    case 4000000:
        return B4000000;
    default: 
        return -1;
    }
}