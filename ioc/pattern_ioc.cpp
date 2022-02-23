
#include "pattern_ioc.h"

//自动生成反转控制函数
#define SPEECH_GEN_IOC_CREATE(ClassName) \
    static void * ClassName##_ioc_create_func() { \
        return static_cast<void *> (new ClassName); \
    }\
    static void ClassName##_ioc_construct_func(void *p) {\
        ::new(p) ClassName;\
    }\
    static size_t ClassName##_ioc_sizeof() {\
        return sizeof(ClassName);\
    }

//注册反转函数
#define SPEECH_ADD_IOC_CLASS(ioc, ClassName) \
    do { \
    ioc.add(#ClassName, ClassName##_ioc_create_func, ClassName##_ioc_construct_func, ClassName##_ioc_sizeof); \
    LOG(DEBUG) << "add "#ClassName" into "#ioc" ioc"; \
    } while (0)

SPEECH_GEN_IOC_CREATE(TaskWithMap)
SPEECH_GEN_IOC_CREATE(TaskEcho)
SPEECH_GEN_IOC_CREATE(TaskASR)
SPEECH_GEN_IOC_CREATE(TaskTTS)
SPEECH_GEN_IOC_CREATE(TaskWithJimuyu)

PatternIOC<UnitTask> g_speech_task_ioc;

void register_all_ioc_object() {
    SPEECH_ADD_IOC_CLASS(g_speech_task_ioc, TaskWithMap);
    SPEECH_ADD_IOC_CLASS(g_speech_task_ioc, TaskEcho);
    SPEECH_ADD_IOC_CLASS(g_speech_task_ioc, TaskASR);
    SPEECH_ADD_IOC_CLASS(g_speech_task_ioc, TaskTTS);
    SPEECH_ADD_IOC_CLASS(g_speech_task_ioc, TaskWithJimuyu);
}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
