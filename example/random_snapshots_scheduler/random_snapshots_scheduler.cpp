#include "../../relacy/pthread.h"

static int argc;
static char **argv;

void* thread_f(void* arg)
{
    printf("%i\n", *(int*)arg);
    return NULL;
}

int pids[2] = {};
void __main__()
{
    printf("start\n");
    for (int i = 0; i < 2; i++)
    {
        pids[i] = i;
        pthread_t thread;
        pthread_create(&thread, 0, thread_f, pids + i);
    }
    printf("finish\n");
}

int main(int c, char **v)
{
	printf("start_main\n");
    argc = c;
    argv = v; 
    rl::test_params p;
    p.static_thread_count = 1;
    p.dynamic_thread_count = 2;
	p.iteration_count = 5;
    p.search_type = rl::sched_random_snapshots;
    rl::execute<__main__>(p);
	printf("end_main\n");
    return 0;
}
