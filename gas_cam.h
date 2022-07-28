#ifndef GAS_CAM_H
#define GAS_CAM_H
#define STAGES_NUMBER 5
#define CAPACITY 10
#define SNAPSHOT_HEIGHT 20
#define SNAPSHOT_WIDTH 20
#define MAX_TEMP 30
#include "Queue.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

double rgb_arr[MAX_TEMP][3]={
    {100,150,200},    {102,60,30},    {80,9,255},
    {80,9,255},    {80,9,255},    {100,150,200},
    {102,60,30},    {80,9,255},    {80,9,255},
    {80,9,255},    {100,150,200},    {102,60,30},
    {80,9,255},    {80,9,255},    {80,9,255},
    {100,150,200},    {102,60,30},    {80,9,255},
    {80,9,255},    {80,9,255},    {100,150,200},
    {102,60,30},    {80,9,255},    {80,9,255},
    {80,9,255},    {100,150,200},    {102,60,30},
    {80,9,255},    {80,9,255},    {80,9,255}
};
typedef struct stage{

    void* (*func)(void*);
    pthread_t thread;
    bool isActive;
    Queue* sourseQu;
    Queue* destQu;
}stage;

typedef struct handler{
    stage stages[STAGES_NUMBER];
    bool is_snapshot_on;
    bool is_record_on;
    FILE *  fp;

}handler;

typedef struct streaming_t{
    int ip;
    int port;
}streaming_t;

typedef struct snapshot_t{
    char* file_name;  //full path ?
    int width;
    int height;
    int type;  //GPEG,PNG,ppm
}snapshot_t;

typedef struct gpio_t{
    int num;
    int dir;  //in ,out
    int value;  //1 ,0
    int is_blink;
    int operation;  //record,straming
}gpio_t;

typedef struct record_t{
    char* file_name;
    int codec;
    int width;
    int height;
    int fps;
}record_t;
typedef struct ppm_image{
    int width;
    int height;
    char *data;
    size_t size;
    char * name;
} ppm_image;
typedef struct {
    void* (*init)();
    void (*free_all)(void* handle);
    int (*start_record)(void* handler);
    int (*stop_record)(void* handler);
    int (*start_streaming)(streaming_t*,char * file_name);
    int (*stop_streamig)(streaming_t*);
    int (*do_snapshot)(void*);
    char* (*get_dll_version)();
    char* (*get_video_statics)(record_t*);
    char* (*get_status)();
}gazapi_t;


#ifdef __cplusplus
}
#endif

#endif // GAS_CAM_H
