/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayilmaz <mayilmaz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 07:26:30 by mayilmaz          #+#    #+#             */
/*   Updated: 2025/08/22 11:42:10 by mayilmaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

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

int	dead_check(t_philo *philo)
{
	pthread_mutex_lock(&philo->data->dead_mutex);
	if (philo->data->is_dead == 1)
	{
		pthread_mutex_unlock(&philo->data->dead_mutex);
		return (1);
	}
	pthread_mutex_unlock(&philo->data->dead_mutex);
	return (0);
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

int	init_arguments(int ac, char **av, t_data *data)
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

void	join(t_data *data)
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

int	eat_count_check(t_philo *philo)
{
	int	i;

	i = 0;
	while (&philo[i])
	{
		pthread_mutex_lock(&(&philo)[i]->eat_mutex);
		if ((&philo)[i]->eat_count != (&philo)[i]->data->max_eat_count)
		{
			pthread_mutex_unlock(&(&philo)[i]->eat_mutex);
			return (0);
		}
		pthread_mutex_unlock(&(&philo)[i]->eat_mutex);
		i++;
	}
	return (1);
}

void	print_dead(t_philo *philo, long long diff, int i, long long now)
{
	pthread_mutex_lock(&philo->data->dead_mutex);
	if (diff > philo->data->time_to_die)
	{
		pthread_mutex_lock(&philo->data->print_mutex);
		printf("%d %lld is died\n",
			philo[i].id, (now - philo->data->start_time));
		pthread_mutex_unlock(&philo->data->print_mutex);
		philo->data->is_dead = 1;
	}
	pthread_mutex_unlock(&philo->data->dead_mutex);
}

void	*monitor_routine(void *tmp)
{
	t_philo		*philo;
	int			i;
	long long	now;
	long long	diff;

	philo = (t_philo *)tmp;
	i = 0;
	pthread_mutex_lock(&philo->data->start_check_mutex);
	philo->data->start_check = 1;
	philo->data->start_time = get_time(philo->data);
	pthread_mutex_unlock(&philo->data->start_check_mutex);
	while (!philo->data->is_dead)
	{
		if (philo->data->num_philos == 1 || i == philo->data->num_philos - 1)
			i = 0;
		if (eat_count_check(philo))
			return (0);
		now = get_time(philo->data);
		pthread_mutex_lock(&philo[i].count_mutex);
		diff = (now - (philo[i].last_meal_time));
		pthread_mutex_unlock(&philo[i].count_mutex);
		print_dead(philo, diff, i, now);
		i++;
	}
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
	join(data);
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

void	*philo_routine(void *tmp)
{
	t_philo	*philo;

	philo = (t_philo *)tmp;
	start_all_philo(philo);
	pthread_mutex_lock(&philo->data->print_mutex);
	philo->data->start_time = get_time(philo->data);
	pthread_mutex_unlock(&philo->data->print_mutex);
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
	printf("%d %lld %s\n", id, (now - data->start_time), status);
	pthread_mutex_unlock(&data->print_mutex);
}

void	single_philo(t_philo *philo, int id)
{
	pthread_mutex_lock(&philo->data->forks[id]);
	print_status(philo, philo->data, id + 1, "has taken a fork");
	pthread_mutex_unlock(&philo->data->forks[id]);
	usleep(philo->data->time_to_die);
	return ;
}

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
	pthread_mutex_lock(&philo->count_mutex);
	philo->last_meal_time = get_time(philo->data);
	pthread_mutex_unlock(&philo->count_mutex);
	if (dead_check(philo) == 1)
	{
		fork_unlock(philo, id);
		return (1);
	}
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

int	philo_cycle(t_philo *philo)
{
	while (1)
	{
		if (eat(philo) == 1)
			return (1);
		if (philo->eat_count == philo->data->max_eat_count)
			return (1);
		if (uyku(philo) == 1)
			return (1);
		if (think(philo) == 1)
			return (1);
	}
	return (0);
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
