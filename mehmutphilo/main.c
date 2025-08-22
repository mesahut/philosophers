/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayilmaz <mayilmaz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 07:26:30 by mayilmaz          #+#    #+#             */
/*   Updated: 2025/08/22 17:03:37 by mayilmaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

long long	get_time(t_data *data)
{
	long long		s_time;
	long long		ms_time;
	struct timeval	tv;

	pthread_mutex_lock(&data->time_mutex);
	gettimeofday(&tv, NULL);
	ms_time = (long long)tv.tv_usec;
	s_time = (long long)tv.tv_sec;
	pthread_mutex_unlock(&data->time_mutex);
	return (s_time * 1000 + ms_time / 1000);
}

void	ft_usleep(t_data *data, int time)
{
	long long	now;

	now = get_time(data);
	while (get_time(data) < (now + time))
		usleep(100);
}

void	philo_join(t_data *data)
{
	int	i;

	i = 0;
	pthread_join(data->monitor, NULL);
	while (i < data->num_philos)
	{
		pthread_join(data->philos[i].thread, NULL);
		i++;
	}
}

int	main(int ac, char **av)
{
	t_data	data;
	int		code;

	code = 0;
	if (check_arguments(ac, av) == 1)
		return (1);
	if (init_arguments(ac, av, &data) == 1)
		return (1);
	code = init_mutexes(&data);
	if (code != 0)
	{
		clean_exit(&data, "initialization failed", code);
		return (1);
	}
	init_philos(&data);
	clean_exit(&data, "bitti", 9);
	return (0);
}
