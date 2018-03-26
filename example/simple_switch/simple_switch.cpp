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
    argc = c;
    argv = v; 
    rl::test_params p;
    p.static_thread_count = 1;
    p.dynamic_thread_count = 2;
    p.search_type = rl::sched_full;
    rl::execute<__main__>(p);
    return 0;
}
