/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayilmaz <mayilmaz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 16:56:03 by mayilmaz          #+#    #+#             */
/*   Updated: 2025/08/22 17:12:54 by mayilmaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdio.h>

void	print_dead(t_philo *philo, int i)
{
	long long	now;
	long long	diff;

	now = get_time(philo->data);
	pthread_mutex_lock(&philo[i].count_mutex);
	diff = (now - (philo[i].last_meal_time));
	pthread_mutex_unlock(&philo[i].count_mutex);
	pthread_mutex_lock(&philo->data->dead_mutex);
	pthread_mutex_lock(&philo->data->print_mutex);
	if (diff > philo->data->time_to_die)
	{
		printf("%lld %d is died\n",
			(now - philo->data->start_time), philo[i].id);
		philo->data->is_dead = 1;
	}
	pthread_mutex_unlock(&philo->data->print_mutex);
	pthread_mutex_unlock(&philo->data->dead_mutex);
}

void	print_status(t_philo *philo, t_data *data, int id, const char *status)
{
	long long	now;

	if (dead_check(philo) == 1)
		return ;
	pthread_mutex_lock(&philo->eat_mutex);
	if (philo->eat_count == data->max_eat_count)
	{
		pthread_mutex_unlock(&philo->eat_mutex);
		return ;
	}
	pthread_mutex_unlock(&philo->eat_mutex);
	pthread_mutex_lock(&data->print_mutex);
	now = get_time(data);
	printf("%lld %d %s\n", (now - data->start_time), id, status);
	pthread_mutex_unlock(&data->print_mutex);
}
