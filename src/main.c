
#include "yaml.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <errno.h>


static int scalar_add(  yaml_emitter_t * emitter, char * key);
static int scalar_kv(  yaml_emitter_t * emitter, char * key,  char * value);
static int yaml_start(yaml_emitter_t * emitter);
static int yaml_end(yaml_emitter_t * emitter);
static int mapping_start(yaml_emitter_t * emitter);
static int mapping_end(yaml_emitter_t * emitter);

#define GITOSC

#ifdef GITOSC
#define YAMLOG(...) AppLog(__FILE__,__LINE__,__VA_ARGS__);
static void AppLog(char *file, long line, char *level_in, char *fmtstr, ...)
{
    va_list ap;
    char level[20];
    char   tmpstr[2048];

    memset(tmpstr, 0x0, sizeof(tmpstr));
    memset( level, 0x00, sizeof( level));

    va_start(ap, fmtstr);

    vsprintf(tmpstr, fmtstr, ap);
    va_end(ap);
    if( level_in[0] == 'D')
        strcpy( level, "DEBUG");
    if( level_in[0] == 'E')
        strcpy( level, "ERROR");
    if( level_in[0] == 'I')
        strcpy( level, "INFO");

    printf("[%s][%s][%03ld]", level, file, line);
    printf("[%s]\n", tmpstr);

    return ;
}
#else
#define YAMLOG XIPLOG
#endif

int main(void)
{
    /** 声明输出 **/
    yaml_emitter_t emitter;

    /** 声明事件 **/
    yaml_event_t event;

    memset(&emitter, 0, sizeof(emitter));

    /** 获取版本 **/
    YAMLOG("D", "libyaml的版本为:[%s]\n", yaml_get_version_string());

    /** 打开文件 **/
    FILE *fp=NULL;

    fp = fopen("./out.yaml", "w");
    if( fp == NULL){
        YAMLOG("打开文件异常[%s]", strerror(errno));
        return -5;
    }

    /** 初始化输出 **/
    int ret = 0;
    ret = yaml_emitter_initialize( &emitter);
    if( ret != 1){
        printf("异常[%d]\n", ret);
        return -5;
    }
    //设置简洁性
    yaml_emitter_set_canonical(&emitter, 0);
    //设置编码
    yaml_emitter_set_unicode(&emitter, YAML_ANY_ENCODING);
    //设置输出
    yaml_emitter_set_output_file(&emitter, fp);


    /*** 写文件头 ***/
    yaml_start(&emitter);
    mapping_start(&emitter);
    {
        scalar_add( &emitter, "meminfo");

        mapping_start(&emitter);
        {
            scalar_kv(&emitter, "key1", "value1");
            scalar_kv(&emitter, "key2", "value2");
        }
        mapping_end(&emitter);
    }
    mapping_end(&emitter);
    yaml_end(&emitter);

    yaml_emitter_delete(&emitter);

    fclose(fp);

    return 0;
}

static int scalar_add(  yaml_emitter_t * emitter, char *  key){
    yaml_event_t event;

    memset(&event, 0x00, sizeof(event));
    yaml_scalar_event_initialize(&event, NULL, NULL, key, -1, 1, 1, YAML_ANY_SCALAR_STYLE);
    yaml_emitter_emit(emitter, &event);

}

static int scalar_kv(  yaml_emitter_t * emitter, char * key,  char * value) {
    yaml_event_t event;

    memset(&event, 0x00, sizeof(event));
    yaml_scalar_event_initialize(&event, NULL, NULL, key, -1, 1, 1, YAML_ANY_SCALAR_STYLE);
    yaml_emitter_emit(emitter, &event);

    memset(&event, 0x00, sizeof(event));
    yaml_scalar_event_initialize(&event, NULL, NULL, value, -1, 1, 1, YAML_ANY_SCALAR_STYLE);
    yaml_emitter_emit(emitter, &event);

    /*
    printf("\nSSSS-参数1anchor[%d]参数2tag[%d]参数3value[%s]参数4len[%d]5[%d]6[%d]7[%d]\n",
           event.data.scalar.anchor,
           event.data.scalar.tag,
           event.data.scalar.value,
           event.data.scalar.length,
           event.data.scalar.plain_implicit,
           event.data.scalar.quoted_implicit,
           event.data.scalar.style);
           */
    return 0;
}

static int yaml_start(yaml_emitter_t * emitter){

    yaml_get_version_string();

    yaml_event_t event;

    memset( &event, 0x00, sizeof(event));
    yaml_stream_start_event_initialize(&event, YAML_ANY_ENCODING);
    yaml_emitter_emit(emitter, &event);

    memset( &event, 0x00, sizeof(event));
    yaml_document_start_event_initialize(&event, NULL, NULL,NULL, 1);
    yaml_emitter_emit(emitter, &event);

    return 0;
}

static int yaml_end(yaml_emitter_t * emitter) {
    yaml_event_t event;
    /** document 尾 **/
    memset( &event, 0x00, sizeof(event));
    yaml_document_end_event_initialize(&event, 1);
    yaml_emitter_emit(emitter, &event);

    /** yaml流的尾 **/
    memset( &event, 0x00, sizeof(event));
    yaml_stream_end_event_initialize(&event);
    yaml_emitter_emit(emitter, &event);

    return 0;
}

static int mapping_start(yaml_emitter_t * emitter) {

    yaml_event_t event;

    memset( &event, 0x00, sizeof(event));
    yaml_mapping_start_event_initialize(&event, NULL, NULL, 1, YAML_BLOCK_MAPPING_STYLE);
    yaml_emitter_emit(emitter, &event);

    return 0;
}

static int mapping_end(yaml_emitter_t * emitter) {
    yaml_event_t event;

    /**  mapping 的尾 **/
    memset(&event, 0x00, sizeof(event));
    yaml_mapping_end_event_initialize(&event);
    yaml_emitter_emit(emitter, &event);
    return 0;
}
