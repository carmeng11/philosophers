int	ft_atoi(char *str)
{
	int	result;
	int	sign;

	result = 0;
	sign = 1;
	while (*str == ' ' || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-')
		sign = -1;
	if (*str == '-' || *str == '+')
		str++;
	while (*str >= '0' && *str <= '9')
	{
		result = result * 10 + *str - '0';
		str++;
	}
	return (sign * result);
}

void	*ft_calloc(size_t nmemb, size_t size)
{
	unsigned char *str;
	size_t i = 0;

	str = malloc(nmemb * size);
	if (!str)
		return (NULL);

	while (i < nmemb * size)
		str[i++] = 0;

	return (str);
}