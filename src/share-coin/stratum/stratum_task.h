







#ifndef __STRATUM__TASK_H__
#define __STRATUM__TASK_H__

/** */
task_t *task_init(int xn_len);

/** */
void task_free(task_t **task_p);

/** */
task_t *stratum_task(unsigned int task_id);

void stratum_task_gen(void);


#endif /* __STRATUM__TASK_H__ */

