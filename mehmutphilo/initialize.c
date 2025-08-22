/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialize.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayilmaz <mayilmaz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 16:34:18 by mayilmaz          #+#    #+#             */
/*   Updated: 2025/08/22 17:12:27 by mayilmaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdlib.h>

void	init_arguments_value(t_data *data, int ac, char **av)
{
	data->num_philos = ft_atoi(av[1]);
	data->time_to_die = ft_atoi(av[2]);
	data->time_to_eat = ft_atoi(av[3]);
	data->time_to_sleep = ft_atoi(av[4]);
	if (ac == 5)
		data->max_eat_count = -1;
	else
		data->max_eat_count = ft_atoi(av[5]);
	data->is_dead = 0;
	data->start_time = 0;
	data->start_check = 0;
	data->forks = NULL;
	data->philos = NULL;
	data->eat_flag = 0;
}

int	init_arguments(int ac, char **av, t_data *data)
{
	init_arguments_value(data, ac, av);
	if (pthread_mutex_init(&data->start_check_mutex, NULL) != 0)
	{
		clean_exit(data, "initialization failed", 0);
		return (1);
	}
	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philos);
	if (!data->forks)
	{
		clean_exit(data, "Memory allocation failed", 1);
		return (1);
	}
	return (0);
}

int	init_mutexes(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_philos)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
		{
			free_forks(data, i);
			return (2);
		}
		i++;
	}
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0)
		return (3);
	if (pthread_mutex_init(&data->dead_mutex, NULL) != 0)
		return (4);
	if (pthread_mutex_init(&data->time_mutex, NULL) != 0)
		return (5);
	return (0);
}

void	init_monitor(t_data *data)
{
	if (pthread_create(&data->monitor, NULL, monitor_routine,
			(void *)data->philos))
		clean_exit(data, "initialization failed", 8);
}

void	init_philos(t_data *data)
{
	int	i;

	i = 0;
	data->philos = malloc(sizeof(t_philo) * data->num_philos);
	if (!data->philos)
		clean_exit(data, "Memory allocation failed", 6);
	while (i < data->num_philos)
	{
		data->philos[i].id = i + 1;
		data->philos[i].left_fork = i;
		data->philos[i].right_fork = (i + 1) % data->num_philos;
		data->philos[i].eat_count = 0;
		data->philos[i].last_meal_time = get_time(data);
		data->philos[i].data = data;
		if (pthread_mutex_init(&data->philos[i].count_mutex, NULL) != 0)
			clean_exit(data, "initialization failed", 7);
		if (pthread_mutex_init(&data->philos[i].eat_mutex, NULL) != 0)
			clean_exit(data, "initialization failed", 7);
		if (pthread_create(&data->philos[i].thread, NULL, philo_routine,
				(void *)&data->philos[i]))
			clean_exit(data, "initialization failed", 7);
		i++;
	}
	init_monitor(data);
	philo_join(data);
}
