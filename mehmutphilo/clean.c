/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayilmaz <mayilmaz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 16:50:34 by mayilmaz          #+#    #+#             */
/*   Updated: 2025/08/22 17:12:41 by mayilmaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdlib.h>
#include <stdio.h>

void	free_forks(t_data *data, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&data->forks[i]);
		i++;
	}
	free(data->forks);
}

void	free_philos(t_data *data, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&data->philos[i].count_mutex);
		pthread_mutex_destroy(&data->philos[i].eat_mutex);
		i++;
	}
	free(data->philos);
}

void	clean_exit(t_data *data, char *commit, int code)
{
	if (code != 9 && code >= 0)
		printf("%s\n", commit);
	if (code >= 1)
		pthread_mutex_destroy(&data->start_check_mutex);
	if (code >= 3)
		free_forks(data, data->num_philos);
	if (code >= 4)
		pthread_mutex_destroy(&data->print_mutex);
	if (code >= 5)
		pthread_mutex_destroy(&data->dead_mutex);
	if (code >= 6)
		pthread_mutex_destroy(&data->time_mutex);
	if (code >= 7)
		free_philos(data, data->num_philos);
}
