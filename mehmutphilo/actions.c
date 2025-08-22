/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayilmaz <mayilmaz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 16:48:08 by mayilmaz          #+#    #+#             */
/*   Updated: 2025/08/22 17:12:33 by mayilmaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	fork_unlock(t_philo *philo, int id)
{
	if (id % 2 == 0)
	{
		pthread_mutex_unlock(&philo->data->forks[philo->right_fork]);
		pthread_mutex_unlock(&philo->data->forks[philo->left_fork]);
	}
	else
	{
		pthread_mutex_unlock(&philo->data->forks[philo->left_fork]);
		pthread_mutex_unlock(&philo->data->forks[philo->right_fork]);
	}
}

int	fork_lock(t_philo *philo, int id)
{
	if (id % 2 == 0)
	{
		pthread_mutex_lock(&philo->data->forks[philo->left_fork]);
		print_status(philo, philo->data, id + 1, "has taken a fork");
		pthread_mutex_lock(&philo->data->forks[philo->right_fork]);
		print_status(philo, philo->data, id + 1, "has taken a fork");
	}
	else
	{
		pthread_mutex_lock(&philo->data->forks[philo->right_fork]);
		print_status(philo, philo->data, id + 1, "has taken a fork");
		pthread_mutex_lock(&philo->data->forks[philo->left_fork]);
		print_status(philo, philo->data, id + 1, "has taken a fork");
	}
	if (dead_check(philo) == 1)
	{
		fork_unlock(philo, id);
		return (1);
	}
	return (0);
}

int	eat(t_philo *philo)
{
	int	id;

	id = philo->id - 1;
	if (dead_check(philo) == 1)
		return (1);
	if (fork_lock(philo, id) == 1)
		return (1);
	if (dead_check(philo) == 1)
	{
		fork_unlock(philo, id);
		return (1);
	}
	pthread_mutex_lock(&philo->count_mutex);
	philo->last_meal_time = get_time(philo->data);
	pthread_mutex_unlock(&philo->count_mutex);
	print_status(philo, philo->data, id + 1, "is eating");
	ft_usleep(philo->data, philo->data->time_to_eat);
	pthread_mutex_lock(&philo->eat_mutex);
	philo->eat_count++;
	pthread_mutex_unlock(&philo->eat_mutex);
	fork_unlock(philo, id);
	return (0);
}

int	uyku(t_philo *philo)
{
	if (dead_check(philo) == 1)
		return (1);
	print_status(philo, philo->data, philo->id, "is sleeping");
	ft_usleep(philo->data, philo->data->time_to_sleep);
	if (dead_check(philo) == 1)
		return (1);
	return (0);
}

int	think(t_philo *philo)
{
	int	time;

	time = (philo->data->time_to_die - (philo->data->time_to_eat
				+ philo->data->time_to_sleep)) / 2;
	if (dead_check(philo) == 1)
		return (1);
	print_status(philo, philo->data, philo->id, "is thinking");
	if (dead_check(philo) == 1)
		return (1);
	if (time >= 0)
		ft_usleep(philo->data, time);
	return (0);
}
