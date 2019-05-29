//
// Created by suitm on 2019-05-27.
//

#include "yaml.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>

void * XipYamlNew( FILE * fp);
int XipYamlFree( void * emitter);
void * XipYamlNewWithPara( FILE * fp, int canonical, int encoding);
int XipYamlDocStart(void * emitter);
int XipYamlDocEnd(void * emitter);

int XipYamlScalarKeyValue(  void * emitter, char * key,  char * value);
int XipYamlScalarAddWithPara(  void * emitter, char * value, int len, char * anchor,
    char * tag, int plain_implicit, int quoted_implicit, int style);
int XipYamlScalarAdd(  void * emitter, char * value);
int XipYamlMappingStart(void * emitter);
int XipYamlMappingStartFlow(void * emitter);
int XipYamlMappingEnd(void * emitter);
int XipYamlSequenceStart(void * emitter);
int XipYamlSequenceEnd(void * emitter);

#define GITOSC

#ifdef GITOSC
#define YAMLOG(...) AppLog(__FILE__,__LINE__,__VA_ARGS__)

static void AppLog(char *file, long line, char *level, char *fmtstr, ...)
{
    va_list ap;
    char   tmpstr[2048];

    memset(tmpstr, 0x0, sizeof(tmpstr));

    va_start(ap, fmtstr);

    vsprintf(tmpstr, fmtstr, ap);
    va_end(ap);

    printf("[%s][%s][%03ld]", level, file, line);
    printf("[%s]\n", tmpstr);

    return ;
}
#else
#define YAMLOG XIPLOG
#endif

int main(void)
{
    /** 打开文件 **/
    FILE *fp=NULL;

    fp = fopen("./out.yaml", "w");
    if( fp == NULL){
        YAMLOG("打开文件异常[%s]", strerror(errno));
        return -5;
    }

    void * em = NULL;

    /* 创建yaml文件 */
    em = XipYamlNew( fp);

    /* 创建document, 一个文件中有多个document */
    XipYamlDocStart(em);

    /* 开始MAPPPING 的 key value 模式 */
    XipYamlMappingStart(em);
    {
        /** 纯map **/
        XipYamlScalarKeyValue(em, "map_key1", "mapvalue1");
        XipYamlScalarKeyValue(em, "map_key2", "mapvalue2");

        /** map的value还是map **/
        XipYamlScalarAdd( em, "map_map");
        XipYamlMappingStart( em);
        {
            XipYamlScalarKeyValue(em, "map_map_key1", "value1");
            XipYamlScalarKeyValue(em, "map_map_key2", "value2");
        }
        XipYamlMappingEnd(em);

        /** map的value是sequence **/
        XipYamlScalarAdd( em, "SEQUENCE");
        XipYamlSequenceStart( em);
        {
            XipYamlScalarAdd( em, "sequence1");
            XipYamlScalarAdd( em, "sequence2");
            XipYamlScalarAdd( em, "sequence3");
            XipYamlScalarAdd( em, "sequence4");
            XipYamlScalarAdd( em, "sequence5");
            XipYamlMappingStart( em);
            {
                XipYamlScalarAdd( em, "sequence6_map");
                XipYamlMappingStartFlow( em);
                    XipYamlScalarKeyValue(em, "sequence_map_key1", "value1");
                    XipYamlScalarKeyValue(em, "sequence_map_key2", "value2");
                XipYamlMappingEnd(em);
            }
            XipYamlMappingEnd(em);
        }
        XipYamlSequenceEnd( em);
    }
    XipYamlMappingEnd(em);

    XipYamlDocEnd(em);

    XipYamlFree( em);

    fclose(fp);

    return 0;
}

/** 常用简洁版 **/
void * XipYamlNew( FILE * fp)
{
    return XipYamlNewWithPara( fp, 0, YAML_ANY_ENCODING);
}

/** 自定义参数版 **/
void *  XipYamlNewWithPara( FILE * fp, int canonical, int encoding)
{

    YAMLOG("D", "使用libyaml,版本为:[%s]\n", yaml_get_version_string());

    /** 声明输出 **/
    yaml_emitter_t * emitter;

    emitter = malloc(sizeof(yaml_emitter_t));

    memset( emitter, 0, sizeof(yaml_emitter_t));

    /** 初始化输出 **/
    int ret = 0;
    ret = yaml_emitter_initialize( emitter);
    if( ret != 1)
    {
        YAMLOG("E", "初始化异常[%d]", ret);
        return NULL;
    }

    //设置简洁性
    yaml_emitter_set_canonical(emitter, 0);
    //设置编码
    yaml_emitter_set_unicode(emitter, YAML_ANY_ENCODING);
    //设置输出
    yaml_emitter_set_output_file(emitter, fp);

    /** yaml流的头 **/
    yaml_event_t event;
    memset( &event, 0x00, sizeof(event));
    yaml_stream_start_event_initialize(&event, YAML_ANY_ENCODING);
    yaml_emitter_emit( emitter, &event);

    return emitter;
}
int XipYamlFree(void * emitter) {
    yaml_event_t event;

    /** yaml流的尾 **/
    memset( &event, 0x00, sizeof(event));
    yaml_stream_end_event_initialize(&event);
    yaml_emitter_emit(emitter, &event);

    if( emitter != NULL)
        free(emitter);

    return 0;
}

/* 用户scalar基本元素添加-简洁函数 */
int XipYamlScalarAdd(  void * emitter, char * value){
    return XipYamlScalarAddWithPara( emitter, value, -1, NULL, NULL, 1, 1, YAML_ANY_SCALAR_STYLE);
}

/* 用户 scalar基本元素添加-带参数版
 * void * emitter: New返回的指针
 * cahr *value: 要添加的值
 * int len: 长度int
 * char * anchor: yaml的锚点
 * char * tag:  yaml的tag
 * int plain_implicit: 未知一般 填1
 * int quoted_implicit: 未知一般 填1
 * */
int XipYamlScalarAddWithPara(  void * emitter, char * value, int len, char * anchor,
        char * tag, int plain_implicit, int quoted_implicit, int style)
{

    yaml_event_t event;
    memset(&event, 0x00, sizeof(event));

    yaml_scalar_event_initialize(&event, (yaml_char_t *)anchor, (yaml_char_t *)tag, (yaml_char_t *)value,
            len, plain_implicit, quoted_implicit, (yaml_scalar_style_t)style);
    yaml_emitter_emit(emitter, &event);

    /*
    memset(&event, 0x00, sizeof(event));
    yaml_alias_event_initialize(&event, "anchor");
    yaml_emitter_emit(emitter, &event);
     */
}

/** 主要用于mapping的键值对的加入 **/
int XipYamlScalarKeyValue(  void * emitter, char * key,  char * value)
{
    yaml_emitter_t * pemitter = (yaml_emitter_t *)emitter;
    XipYamlScalarAdd( pemitter, key);
    XipYamlScalarAdd( pemitter, value);

    return 0;
}


/* document 开始 */
int XipYamlDocStart(void * emitter){

    yaml_event_t event;

    /** document 头 **/
    memset( &event, 0x00, sizeof(event));
    yaml_document_start_event_initialize(&event, NULL, NULL,NULL, 1);
    yaml_emitter_emit(emitter, &event);

    return 0;
}

/* document 结束 */
int XipYamlDocEnd(void * emitter) {

    yaml_event_t event;

    /** document 尾 **/
    memset( &event, 0x00, sizeof(event));
    yaml_document_end_event_initialize(&event, 1);
    yaml_emitter_emit(emitter, &event);


    return 0;
}

/**  mapping 的开头 **/
int XipYamlMappingStart(void * emitter) {
    yaml_event_t event;

    memset( &event, 0x00, sizeof(event));
    yaml_mapping_start_event_initialize(&event, NULL, NULL, 1, YAML_BLOCK_MAPPING_STYLE);
    yaml_emitter_emit(emitter, &event);

    return 0;
}

int XipYamlMappingStartFlow(void * emitter) {
    yaml_event_t event;

    memset( &event, 0x00, sizeof(event));
    yaml_mapping_start_event_initialize(&event, NULL, NULL, 1, YAML_FLOW_MAPPING_STYLE);
    yaml_emitter_emit(emitter, &event);

    return 0;
}

/**  mapping 的结尾 **/
int XipYamlMappingEnd(void * emitter) {
    yaml_event_t event;

    memset(&event, 0x00, sizeof(event));
    yaml_mapping_end_event_initialize(&event);
    yaml_emitter_emit(emitter, &event);
    return 0;
}

/**  sequence 的开头 **/
int XipYamlSequenceStart(void * emitter) {
    yaml_event_t event;

    memset( &event, 0x00, sizeof(event));
    yaml_sequence_start_event_initialize(&event, NULL, NULL, 1, YAML_ANY_SEQUENCE_STYLE);
    yaml_emitter_emit(emitter, &event);

    return 0;
}

/**  sequence 的结尾 **/
int XipYamlSequenceEnd(void * emitter) {
    yaml_event_t event;

    memset( &event, 0x00, sizeof(event));
    yaml_sequence_end_event_initialize(&event);
    yaml_emitter_emit(emitter, &event);

    return 0;
}

