#ifndef PHİLO_H
#define PHİLO_H

#include <pthread.h>
typedef struct s_philo
{
    int         id;
    int left_fork;
    int right_fork;
    int         eat_count;
    long long   last_meal_time;
    pthread_t   thread;
    struct      s_philo  *next;
}t_philo;

typedef struct s_data
{
    int                 num_philos;
    int                 time_to_eat;
    int                 time_to_die;
    int                 time_to_sleep;
    pthread_mutex_t     *forks;
    pthread_mutex_t     print_mutex;
    int                 max_eat_count;
    pthread_t           monitor;
    t_philo             *philos;
    int                 is_dead;
    long long           start_time;
}t_data;

#endif
