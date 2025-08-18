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
    struct      s_data  *data;
}t_philo;

typedef struct s_data
{
    int                 num_philos;
    int                 time_to_eat;
    int                 time_to_die;
    int                 time_to_sleep;
    int                 max_eat_count;
    int                 is_dead;
    long long           start_time;
    pthread_mutex_t     *forks;
    pthread_mutex_t     print_mutex;
    pthread_mutex_t     dead_mutex;
    pthread_mutex_t     eat_mutex;
    pthread_mutex_t     time_mutex;
    pthread_t           monitor;
    t_philo             *philos;
}t_data;

int	ft_atoi(char *str);
void    *philo_routine(void *tmp);
void philo_cycle(t_data *data);
void    single_philo(t_data *data, int id);
void    print_status(t_data *data, int id, const char *status);

#endif
