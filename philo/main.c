#include "philo.h"
 #include <sys/time.h>

void check_arguments(int ac, char **av)
{
    if (ac < 5 || ac > 6)
        return;

    int i;
    int j;

    i = 1;
    while (i < ac)
    {
        j = 0;
        while (av[i][j] != '\0')
        {
            if (av[i][j] < '0' || av[i][j] > '9')
                ;
            j++;
        }
        i++;
    }
}

init_arguments(int ac, char **av, t_data *data)
{
    data->num_philos = ft_atoi(av[1]);
    data->time_to_eat = ft_atoi(av[2]);
    data->time_to_die = ft_atoi(av[3]);
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

init_philos(t_data *data)
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
        pthread_create(&data->philos[i].thread, NULL, philo_routine, &data);
        i++;
    }
}

init_mutexes(t_data *data)
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
}

philo_routine(void *arg)
{
    t_philo *philo = (t_philo *)arg;
    while (1)
    {
    }
}
int main(int ac, char **av)
{
    t_data *data;
    check_arguments(ac, av);
    init_arguments(ac, av, data);
    init_philos(data);
    init_mutexes(data);
}