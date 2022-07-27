#include "gas_cam.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void randMat(int** matrix_temp){//[ ][SNAPSHOT_WIDTH]
    int i,j,new_temp,height_pos,width_pos,num_iterations=0.1*SNAPSHOT_WIDTH*SNAPSHOT_HEIGHT;
    for(i=0;i<SNAPSHOT_HEIGHT;i++){
        for(j=0;j<SNAPSHOT_WIDTH;j++){
            matrix_temp[i][j]=27;
        }
    }
    for(i=0;i<num_iterations;i++)
    {
        height_pos=rand() % SNAPSHOT_HEIGHT;
        width_pos=rand() % SNAPSHOT_WIDTH;
        new_temp=rand()%(MAX_TEMP+1);
        matrix_temp[height_pos][width_pos]=new_temp;
    }
}
void covert_to_rgb(int*** rgb_matrix,int** matrix){//[SNAPSHOT_HEIGHT][SNAPSHOT_WIDTH] [SNAPSHOT_HEIGHT][SNAPSHOT_WIDTH]
    for(int i=0;i<SNAPSHOT_HEIGHT;i++)
        for(int j=0;j<SNAPSHOT_WIDTH;j++)
        {
            int * arr=rgb_arr[matrix[i][j]];
            for(int k=0;k<3;k++)
            {
                rgb_matrix[i][j][k]=arr[k];
            }
        }
}
void copy_matrix(int matrix[SNAPSHOT_HEIGHT][SNAPSHOT_WIDTH],void* data){
    int** data_matrix=(int**)(data);
    printf("%d",data_matrix);
    for(int i=0;i<SNAPSHOT_HEIGHT;i++){
        for(int j=0;j<SNAPSHOT_WIDTH;j++){
            matrix[i][j]=data_matrix[i][j];
            // printf("%d",data_matrix[i][j]);
        }
    }
}
void copy_rgb_matrix(int* rgb_matrix[SNAPSHOT_HEIGHT][SNAPSHOT_WIDTH],void* data){
    int*** data_matrix=(int***)(data);
    for(int i=0,j;i<SNAPSHOT_HEIGHT;i++){
        for(j=0;j<SNAPSHOT_WIDTH;j++){
            rgb_matrix[i][j]=data_matrix[i][j];
        }
    }
}

void free_matrix(int** m){
    for(int i=0;i<SNAPSHOT_HEIGHT;i++){
        free(m[i]);
        m[i]=NULL;
    }

}
void free_rgb_matrix(int*** m){
    for(int i=0;i<SNAPSHOT_HEIGHT;i++){
        for(int j=0;j<SNAPSHOT_WIDTH;j++){
            free(m[i][j]);
            m[i][j]=NULL;
        }
        free(m[i]);
        m[i]=NULL;
    }


}
void* capture(void* stg){
    printf("-----capture\n-----");
    stage* my_stage=(stage*)stg;
    Node*n;
    int i=0,** matrix=NULL;//[SNAPSHOT_HEIGHT][SNAPSHOT_WIDTH]={27};
    while(1){
        matrix=(int**)malloc(sizeof(int *)*SNAPSHOT_HEIGHT);
        for(int i=0;i<SNAPSHOT_HEIGHT;i++)  {
            matrix[i]=NULL;
            matrix[i]=(int *)malloc(sizeof(int)*SNAPSHOT_WIDTH);
        }
        randMat(matrix);
        n=createNode(matrix, sizeof(int));
        enqueu(my_stage->destQu,n);
        printf("capture %d\n",*(int*)(n->data));
        sleep(3);
        i++;
    }
}
void* rgb_converter(void* stg){
    printf("-----rgb_converter\n-----");
    stage* my_stage=(stage*)stg;
    Node*n;
    // int matrix[SNAPSHOT_HEIGHT][SNAPSHOT_WIDTH];
    int*** rgb_matrix=NULL;//[SNAPSHOT_HEIGHT][SNAPSHOT_WIDTH];

    while(1){
        //fort the meantime...
        rgb_matrix=(int***)malloc(sizeof(int**)*SNAPSHOT_HEIGHT);
        for(int i=0;i<SNAPSHOT_HEIGHT;i++){
            rgb_matrix[i]=NULL;
            rgb_matrix[i]=(int **)malloc(sizeof(int*)*SNAPSHOT_WIDTH);
            for(int j=0;j<SNAPSHOT_WIDTH;j++){
                rgb_matrix[i][j]=NULL;
                rgb_matrix[i][j]=(int*)malloc(sizeof(int)*3);
            }
        }
        while(isEmpty(my_stage->sourseQu))
            sleep(1);
        n=dequeue(my_stage->sourseQu);
        //copy_matrix(matrix,(int **)n->data);
        covert_to_rgb(rgb_matrix,(int**)n->data);
        free_matrix((int**)(n->data));
        freeNode(n);
        n=createNode(rgb_matrix,sizeof(rgb_matrix));
        printf("rgb \n");
        enqueu(my_stage->destQu,n);
        sleep(3);
    }
}
void* yuv_converter(void* stg){
    printf("-----yuv_converter\n-----");
    stage* my_stage=(stage*)stg;
    Node*n;
    //    int* rgb_matrix[SNAPSHOT_HEIGHT][SNAPSHOT_WIDTH];
    while(1){
        while(isEmpty(my_stage->sourseQu))
            sleep(1);
        n=dequeue(my_stage->sourseQu);
        //       copy_rgb_matrix(rgb_matrix,n->data);
        printf("yuv \n");
        enqueu(my_stage->destQu,n);
        //  free(n);
        sleep(3);
    }
}
void* encode(void* stg){
    printf("-----cencode\n-----");

    stage* my_stage=(stage*)stg;
    Node*n;

    while(1){
        while(isEmpty(my_stage->sourseQu))
            sleep(1);
        n=dequeue(my_stage->sourseQu);
        printf("encode\n");
        enqueu(my_stage->destQu,n);
        //   free(n);
        sleep(3);
    }
}

void* write_record(void*stg){
    printf("-----write\n-----");
    stage* my_stage=(stage*)stg;
    Node*n;

    while(1){
        while(isEmpty(my_stage->sourseQu))
            sleep(1);
        n=dequeue(my_stage->sourseQu);
        printf("write \n");
        free_rgb_matrix((int***)(n->data));
        freeNode(n);
        sleep(3);
    }
}

void* GAS_API_init(){
    handler* handle=(handler*)malloc(sizeof(handler));
    if(handle==NULL){
        printf("error mallocing\n");
        exit(0);
    }
    //init queues
    int i=0;
    handle->stages[i].sourseQu=NULL;
    for(;i<STAGES_NUMBER-1;i++){
        handle->stages[i].destQu=createQueue(CAPACITY);
        handle->stages[i+1].sourseQu=handle->stages[i].destQu;
    }
    handle->stages[i].destQu=NULL;
    //init functions
    handle->stages[0].func=capture;
    handle->stages[1].func=rgb_converter;
    handle->stages[2].func=yuv_converter;
    handle->stages[3].func=encode;
    handle->stages[4].func=write_record;
    return (void*)handle;
}
void GAS_API_free_all(void* handle){
    handler* my_handler=(handler*)(handle);

    for(int i=0;i<STAGES_NUMBER-1;i++){
        freeQueue(my_handler->stages[i].destQu);
        my_handler->stages[i].destQu=NULL;

    }
    free(my_handler);


}
int GAS_API_start_record(void* handle){
    printf ("====start_record====\n");
    handler* my_handler=(handler*)(handle);
    for(int i=0;i<STAGES_NUMBER;i++){
        pthread_create(&(my_handler->stages[i].thread),NULL,(my_handler->stages)[i].func,&(my_handler->stages[i]));
    }
    return 0;

}
int GAS_API_stop_record(void* handle){

    printf ("====stop_record====\n\r");
    handler* my_handler=(handler*)(handle);
    for(int i=0;i<STAGES_NUMBER;i++){
        pthread_join(my_handler->stages[i].thread,NULL);
    }
    return 0;
}
int GAS_API_start_streaming(streaming_t* stream,char * file_name){
    printf("GAZ_API_start_streaming\n");
    return 1;
}
int GAS_API_stop_streamig(streaming_t*stream){
    printf("GAZ_API_stop_streamig");
    return 1;
}
int GAS_API_do_snapshot(snapshot_t* snapshot){
    printf("GAZ_API_stop_streamig");
    return 1;
}
char* GAS_API_get_dll_version(){
    printf("GAZ_API_get_dll_version");
    return NULL;
}
char* GAS_API_get_video_statics(record_t* r){
    printf("GAZ_API_get_video_statics");
    return NULL;
}
char* GAS_API_get_status(){
    printf("GAZ_API_get_status");
    return NULL;
}

gazapi_t p_gaz_api= {
    .init=GAS_API_init,
    .free_all=GAS_API_free_all,
    .start_record=GAS_API_start_record,
    .stop_record=GAS_API_stop_record,
    .start_streaming=GAS_API_start_streaming,
    .stop_streamig=GAS_API_stop_streamig,
    .do_snapshot=GAS_API_do_snapshot,
    .get_dll_version=GAS_API_get_dll_version,
    .get_video_statics=GAS_API_get_video_statics,
    .get_status=GAS_API_get_status
};




