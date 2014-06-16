







#ifndef __STRATUM__TASK_H__
#define __STRATUM__TASK_H__


/** */
void task_free(task_t **task_p);

/** */
task_t *stratum_task(unsigned int task_id);


task_t *task_init(void);


#endif /* __STRATUM__TASK_H__ */

