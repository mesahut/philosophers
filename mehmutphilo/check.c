/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayilmaz <mayilmaz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 07:47:49 by mayilmaz          #+#    #+#             */
/*   Updated: 2025/08/23 13:38:19 by mayilmaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdio.h>

int	ft_atoi(char *str)
{
	int			i;
	long int	number;
	int			sign;

	i = 0;
	number = 0;
	sign = 1;
	if (str[i] == 45 || str[i] == 43)
	{
		if (str[i] == 45)
			sign *= -1;
		i++;
	}
	while (str[i] >= 48 && str[i] <= 57)
	{
		number *= 10;
		number += str[i] - 48;
		i++;
	}
	number *= sign;
	return (number);
}

int	check_arg_value(int ac, char **av)
{
	int	i;
	int	j;

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
				return (1);
			}
			j++;
		}
		if (ft_atoi(av[i]) <= 0)
		{
			printf("Error: Argument '%s' must be a positive integer\n", av[i]);
			return (1);
		}
		i++;
	}
	return (0);
}

int	check_arguments(int ac, char **av)
{
	int	i;
	int	j;

	i = 1;
	j = 0;
	if (ac < 5 || ac > 6)
	{
		printf("Error: wrong arguments size\n");
		return (1);
	}
	if (check_arg_value(ac, av) == 1)
		return (1);
	return (0);
}

int	dead_check(t_philo *philo)
{
	int	dead;

	pthread_mutex_lock(&philo->data->dead_mutex);
	dead = philo->data->is_dead;
	pthread_mutex_unlock(&philo->data->dead_mutex);
	return (dead);
}

int	eat_count_check(t_philo *philo)
{
	pthread_mutex_lock(&philo->data->start_check_mutex);
	if (philo->data->eat_flag == philo->data->num_philos)
	{
		pthread_mutex_unlock(&philo->data->start_check_mutex);
		return (0);
	}
	pthread_mutex_unlock(&philo->data->start_check_mutex);
	return (1);
}
