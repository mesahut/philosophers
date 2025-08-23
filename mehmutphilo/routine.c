/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayilmaz <mayilmaz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 16:44:12 by mayilmaz          #+#    #+#             */
/*   Updated: 2025/08/24 00:41:20 by mayilmaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*monitor_routine(void *tmp)
{
	t_philo		*philo;
	int			i;

	philo = (t_philo *)tmp;
	i = 0;
	pthread_mutex_lock(&philo->data->start_check_mutex);
	philo->data->start_time = get_time(philo->data);
	philo->data->start_check = 1;
	pthread_mutex_unlock(&philo->data->start_check_mutex);
	while (!philo->data->is_dead)
	{
		if (philo->data->num_philos == 1 || i == philo->data->num_philos - 1)
			i = 0;
		if (philo->data->max_eat_count != -1)
			if (eat_count_check(philo) == 0)
				return (0);
		if (print_dead(philo, i) == 1)
			return (0);
		i++;
	}
	return (0);
}

void	start_all_philo(t_philo *philo)
{
	while (1)
	{
		pthread_mutex_lock(&philo->data->start_check_mutex);
		if (philo->data->start_check == 1)
		{
			pthread_mutex_unlock(&philo->data->start_check_mutex);
			break ;
		}
		pthread_mutex_unlock(&philo->data->start_check_mutex);
	}
}

void	single_philo(t_philo *philo, int id)
{
	pthread_mutex_lock(&philo->data->forks[id]);
	print_status(philo, philo->data, id + 1, "has taken a fork");
	pthread_mutex_unlock(&philo->data->forks[id]);
	ft_usleep(philo->data, philo->data->time_to_die);
	return ;
}

void	*philo_routine(void *tmp)
{
	t_philo	*philo;

	philo = (t_philo *)tmp;
	start_all_philo(philo);
	pthread_mutex_lock(&philo->count_mutex);
	philo->last_meal_time = philo->data->start_time;
	pthread_mutex_unlock(&philo->count_mutex);
	if (philo->data->num_philos == 1)
	{
		single_philo(philo, 0);
		return (0);
	}
	if (philo->id % 2 == 0)
		ft_usleep(philo->data, 10);
	philo_cycle(philo);
	return (0);
}

int	philo_cycle(t_philo *philo)
{
	while (1)
	{
		// printf("----------------------------------%d\n", philo->data->is_dead);
		if (eat(philo) == 1)
			return (1);
		pthread_mutex_lock(&philo->eat_mutex);
		if (philo->eat_count == philo->data->max_eat_count)
		{
			pthread_mutex_unlock(&philo->eat_mutex);
			pthread_mutex_lock(&philo->data->start_check_mutex);
			philo->data->eat_flag++;
			pthread_mutex_unlock(&philo->data->start_check_mutex);
			return (1);
		}
		pthread_mutex_unlock(&philo->eat_mutex);
		if (uyku(philo) == 1)
			return (1);
		if (think(philo) == 1)
			return (1);
	}
	return (0);
}
