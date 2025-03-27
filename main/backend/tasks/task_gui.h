#ifndef __TASK_TASK_GUI_H__
#define __TASK_TASK_GUI_H__


// GUI Task Settings
#define GUI_STACK_SIZE (20 * 1024)

#ifdef __cplusplus
extern "C" {
#endif

void task_gui(void* args);

#if __cplusplus
} /*extern "C"*/
#endif

#endif
