#include "philo.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

void    clean_exit(t_data *data, char *commit)
{

}

long long   get_time()
{
    long long s_time;
    long long ms_time;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    ms_time = (long long)tv.tv_usec;
    s_time = (long long)tv.tv_sec;
    return (s_time * 1000 + ms_time / 1000);
}

void    ft_usleep(t_data *data,int time)
{
    long long now;

    now = get_time();
    while(get_time() < (now + time))
        usleep(100);
}

void check_arguments(int ac, char **av)
{
    if (ac < 5 || ac > 6)
        return;
    int i;
    int j;

    i = 1;
    j = 0;
    while (i < ac)
    {
        j = 0;
        while (av[i][j] != '\0')
        {
            if (av[i][j] < '0' || av[i][j] > '9')
            {
                printf("Error: Invalid argument '%s'\n", av[i]);
                return;
            }
            j++;
        }
        if (ft_atoi(av[i]) <= 0)
        {
            printf("Error: Argument '%s' must be a positive integer\n", av[i]);
            return;
        }
        i++;
    }

}

void    init_arguments(int ac, char **av, t_data *data)
{
    data->num_philos = ft_atoi(av[1]);
    data->time_to_die = ft_atoi(av[2]);
    data->time_to_eat = ft_atoi(av[3]);
    data->time_to_sleep = ft_atoi(av[4]);
    if(ac == 5)
        data->max_eat_count = -1;
    else
        data->max_eat_count = ft_atoi(av[5]);
    data->is_dead = 0;
    data->start_time = 0;
    data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philos);
    if (!data->forks)
        clean_exit(data, "Memory allocation failed");
}

void    join(t_data *data)
{
    int i;

    i = 0;
    pthread_join(data->monitor, NULL);
    while(i < data->num_philos)
    {
        pthread_join(data->philos[i].thread, NULL);
        i++;
    }
    
}

void    *monitor_routine(void *tmp)
{
    t_data *data;
    int     i;
    long long now;

    data = (t_data *)tmp;
    i = 0;
    while (data->is_dead)
    {
        if(i == data->num_philos - 1)
            i = 0;
        pthread_mutex_lock(&data->dead_mutex);
        now = get_time();
        if((now - data->philos[i].last_meal_time) > data->time_to_die)
        {
            print_status(data,data->philos->id ,"is died");
            data->is_dead = 1;
        }
        pthread_mutex_unlock(&data->dead_mutex);
    }
    return (0);
}

void    init_philos(t_data *data)
{
    int i;

    i = 0;
    data->philos = malloc(sizeof(t_philo) * data->num_philos);
    if (!data->philos)
        clean_exit(data, "Memory allocation failed");
    while (i < data->num_philos)
    {
        data->philos[i].id = i + 1;
        data->philos[i].left_fork = i;
        data->philos[i].right_fork = (i + 1) % data->num_philos;
        data->philos[i].eat_count = 0;
        data->philos[i].last_meal_time = 0;
        pthread_create(&data->philos[i].thread, NULL, philo_routine, (void *)data);
        i++;
    }
    pthread_create(&data->monitor, NULL, monitor_routine, (void *)data);
    join(data);
}

void    init_mutexes(t_data *data)
{
    int i;

    i = 0;
    while (i < data->num_philos)
    {
        if (pthread_mutex_init(&data->forks[i], NULL) != 0)
            clean_exit(data, "Mutex initialization failed");
        i++;
    }
    if (pthread_mutex_init(&data->print_mutex, NULL) != 0)
            clean_exit(data, "Print mutex initialization failed");
    if (pthread_mutex_init(&data->dead_mutex, NULL) != 0)
        clean_exit(data, "Print mutex initialization failed");
    if (pthread_mutex_init(&data->eat_mutex, NULL) != 0)
        clean_exit(data, "Print mutex initialization failed");
    if (pthread_mutex_init(&data->time_mutex, NULL) != 0)
            clean_exit(data, "Print mutex initialization failed");
    
}

void    *philo_routine(void  *tmp)
{
    
    t_data *data;

    data = (t_data *)tmp;
    pthread_mutex_lock(&data->eat_mutex);
    data->start_time = get_time();
    pthread_mutex_unlock(&data->eat_mutex);
    
    if(data->num_philos == 1)
    {
        single_philo(data, 0);
        return (0);
    }
    while (1)
    {
        pthread_mutex_lock(&data->eat_mutex);
        // max eat count control
        if(data->is_dead == 1 || data->max_eat_count == 0)
        {
            pthread_mutex_unlock(&data->eat_mutex);
            break;
        }

        pthread_mutex_unlock(&data->eat_mutex);
        philo_cycle(data);
    }
    return (0);
}


void    print_status(t_data *data, int id, const char *status)
{
    long long now;
    pthread_mutex_lock(&data->print_mutex);
    now = get_time();
    printf("%d %lld %s\n", id,(now - data->start_time), status);
    pthread_mutex_unlock(&data->print_mutex);
}

void    single_philo(t_data *data, int id)
{
    pthread_mutex_lock(&data->forks[id]);
    print_status(data, id + 1, "has taken a fork");
    pthread_mutex_unlock(&data->forks[id]);
}

void    eat(t_data *data)
{
    int id = data->philos->id - 1;

    pthread_mutex_lock(&data->forks[id]);
    print_status(data, id + 1, "has taken a fork");
    pthread_mutex_lock(&data->forks[data->philos[id].right_fork]);
    print_status(data, id + 1, "has taken a fork");
    
    pthread_mutex_lock(&data->eat_mutex);
    data->philos[id].last_meal_time = get_time();
    data->philos[id].eat_count++;
    pthread_mutex_unlock(&data->eat_mutex);
    ft_usleep(data, data->time_to_eat);
    print_status(data, id + 1, "is eating");
    pthread_mutex_unlock(&data->forks[data->philos[id].right_fork]);
    pthread_mutex_unlock(&data->forks[id]);
}

void uyku(t_data *data)
{
    printf("--------------------------\n");
    ft_usleep(data, data->time_to_sleep);
    print_status(data, data->philos->id, "is sleeping");
}

void    think(t_data *data)
{
    print_status(data, data->philos->id, "is thinking");
}

void philo_cycle(t_data *data)
{
    while (1)
    {
        if(data->philos->eat_count == data->max_eat_count)
            break;
        pthread_mutex_lock(&data->dead_mutex);
        if(data->is_dead)
            break;
        pthread_mutex_unlock(&data->dead_mutex);
        eat(data);
        uyku(data);
        think(data);
    }
}



int main(int ac, char **av)
{
    t_data data;

    check_arguments(ac, av);
    init_arguments(ac, av, &data);
    init_mutexes(&data);
    init_philos(&data);
    return (0);
}
