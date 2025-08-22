/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mayilmaz <mayilmaz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/22 08:07:00 by mayilmaz          #+#    #+#             */
/*   Updated: 2025/08/22 08:07:03 by mayilmaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>

typedef struct s_philo
{
	int				id;
	int				left_fork;
	int				right_fork;
	int				eat_count;
	long long		last_meal_time;
	pthread_t		thread;
	pthread_mutex_t	count_mutex;
	pthread_mutex_t	eat_mutex;
	struct s_data	*data;
}					t_philo;

typedef struct s_data
{
	int				num_philos;
	int				time_to_eat;
	int				time_to_die;
	int				time_to_sleep;
	int				max_eat_count;
	int				is_dead;
	long long		start_time;
	int				start_check;
	pthread_mutex_t	start_check_mutex;
	pthread_mutex_t	*forks;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	dead_mutex;
	pthread_mutex_t	time_mutex;
	pthread_t		monitor;
	t_philo			*philos;
}					t_data;

int					ft_atoi(char *str);
void				*philo_routine(void *tmp);
int					philo_cycle(t_philo *philo);
void				single_philo(t_philo *philo, int id);
void				print_status(t_philo *philo, t_data *data, int id,
						const char *status);
#endif
