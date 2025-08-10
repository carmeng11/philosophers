#include "philo.h"
 //Función estática que retorna 1 si detecta la muerte de un filósofo, 
 //0 si todos están vivos.
static int check_philosopher_death(t_data *data)
{
    int i = 0;
    long long current_time;
    long long last_meal;

    while (i < data->num_philo)//bucle para verificar cada filósofo uno a uno
    {
        current_time = get_current_time_ms();//tiempo actual que se obtiene para cada verificación
        
        // Leer last_meal_time de forma segura
        pthread_mutex_lock(&data->philosophers[i].meal_lock);
        last_meal = data->philosophers[i].last_meal_time;//copia local del tiempo de la última comida
        //el last_meal_time se actualiza en la funcion eat (philo->last_meal_time = get_current_time_ms();)
        //Cada vez que un filósofo empieza a comer actualiza su last_meal_time con el timepo actual. 
        //Esto marca cuando fue su última comida.
        pthread_mutex_unlock(&data->philosophers[i].meal_lock);
        
        // Verificar si el filósofo ha muerto
        if (current_time - last_meal > data->time_to_die)
        {
            set_game_over(data);//marca is_game_over a 1
            print_death(&data->philosophers[i]);//imprime el mensaje de muerte
            return (1);//sale del monitor
        }
        i++;
    }
    return (0);
}

¿Por qué está bien hacer la comprobación FUERA del mutex?
Principio de minimizar tiempo de bloqueo: Solo bloqueas para la operación crítica (leer last_meal_time)
Los datos copiados son seguros: Una vez que copias last_meal a una variable local, ya no necesitas el mutex. 
Evitas deadlocks: print_death() usa print_mutex, si lo hicieras dentro de meal_lock podrías crear dependencias 
circularesMejor rendimiento: Reduces el tiempo que cada filósofo tiene que esperar para acceder a su meal_lock

static int check_all_philosophers_fed(t_data *data)
{
    int i = 0;
    int meals_eaten;
    int all_fed = 1;

    if (data->max_meals == -1)
        return (0);

    while (i < data->num_philo)
    {
        pthread_mutex_lock(&data->philosophers[i].meal_lock);
        meals_eaten = data->philosophers[i].meals_eaten;
        pthread_mutex_unlock(&data->philosophers[i].meal_lock);
        
        if (meals_eaten < data->max_meals)
        {
            all_fed = 0;
            break;
        }
        i++;
    }
    
    if (all_fed)
    {
        set_game_over(data);
        return (1);
    }
    
    return (0);
}

void *monitor_routine(void *arg)
{
    t_data *data = (t_data *)arg;

    while (!is_game_over(data))
    {
        // Verificar muertes
        if (check_philosopher_death(data))
            break;
            
        // Verificar si todos han comido suficiente
        if (check_all_philosophers_fed(data))
            break;
            
        // Pausa pequeña para no sobrecargar CPU
        ft_usleep(5);//originalmente estaba a 1 pero se cambió para el caso 3 310 103 103
    }
    
    return (NULL);
}
//cambiado a esta función para el caso de 2 filósofos donde la espera debe ser más 
//pequeña y pueda detectar la muerte a tiempo
//Esta función se encarga de monitorizar el estado de los filósofos y detectar muertes o si todos han comido suficiente.
//Si hay 2 filósofos y el tiempo de muerte es menor a 200, se reduce el tiempo de espera a 1 milisegundo para detectar muertes más rápido.
//En otros casos, se usa un tiempo de espera de 5 milisegundos.
//El monitor revisa periódicamente si algún filósofo ha muerto o si todos han comido el número máximo de comidas permitido.
//Si detecta una muerte, marca el juego como terminado y muestra el mensaje de muerte del filósofo correspondiente.
//Si todos los filósofos han comido suficientes veces, también marca el juego como terminado.
//El monitor se ejecuta en un hilo separado y continúa hasta que se detecta una condición de finalización del juego.
void	*monitor_routine(void *arg)
{
	t_data	*data;
	int		sleep_time;
	
	data = (t_data *)arg;
	if (data->num_philo <= 2 && data->time_to_die < 200)
		sleep_time = 1;//cambiado para 2 el tiempo para que le de tiempo a detectar la muerte
	else
		sleep_time = 5;
	while (!is_game_over(data))
	{
		if (check_philosopher_death(data))
			break ;
		if (check_all_philosophers_fed(data))
			break ;
		ft_usleep(sleep_time);
	}
	return (NULL);
}

Finalmente al fallar con otro caso se busca solución más generica y mas elegante para con 1 ms o 5 ms según > o < a 100 filos

void	*monitor_routine(void *arg)
{
	t_data	*data;
	int		sleep_time;
	
	data = (t_data *)arg;
	if (data->num_philo <= 100)
		sleep_time = 1;//cambiado para 2 el tiempo para que le de tiempo a detectar la muerte
	else
		sleep_time = 5;
	while (!is_game_over(data))
	{
		if (check_philosopher_death(data))
			break ;
		if (check_all_philosophers_fed(data))
			break ;
		ft_usleep(sleep_time);
	}
	return (NULL);
}

EXPLICACION PORQUE ES NECESARIO EL MUTEX EN MEAL_LOCK
Problema sin mutex:
Imagina esta situación SIN el mutex:

Monitor lee last_meal_time = 1000ms
Justo en ese momento, el filósofo termina de comer y actualiza last_meal_time = 2000ms
Monitor calcula muerte basándose en el valor obsoleto 1000ms
FALSO POSITIVO - declara muerto a un filósofo que acaba de comer

Condición de carrera específica:
c// HILO FILÓSOFO (en función eat):
pthread_mutex_lock(&philo->meal_lock);
philo->last_meal_time = get_current_time_ms();  // ← ESCRITURA
pthread_mutex_unlock(&philo->meal_lock);

// HILO MONITOR (aquí):
// Sin mutex: podría leer JUSTO cuando el filósofo está escribiendo
last_meal = data->philosophers[i].last_meal_time; // ← LECTURA
c        last_meal = data->philosophers[i].last_meal_time;
Línea 8: Lee de forma atómica y segura el tiempo de última comida. Al estar protegido por el mutex, garantiza que:

O lee el valor antes de que el filósofo lo actualice
O lee el valor después de que el filósofo lo actualice
NUNCA lee un valor en estado intermedio o inconsistente

c        pthread_mutex_unlock(&data->philosophers[i].meal_lock);
Línea 9: Libera inmediatamente el mutex. El tiempo bloqueado es mínimo (solo una lectura).
c        // Verificar si el filósofo ha muerto
        if (current_time - last_meal > data->time_to_die)
        {
Línea 10: Calcula si ha pasado más tiempo del permitido desde la última comida. Ahora usa la copia local last_meal (ya no necesita el mutex).
c            set_game_over(data);//marca is_game_over a 1
Línea 11: Marca el fin del juego para que todos los hilos terminen ordenadamente.
c            print_death(&data->philosophers[i]);//imprime el mensaje de muerte
Línea 12: Imprime el mensaje de muerte del filósofo que murió de hambre.
c            return (1);//sale del monitor
Línea 13: Retorna 1 indicando que se detectó una muerte, el monitor puede terminar.
c        }
        i++;
    }
    return (0);
Líneas 14-16: Si ningún filósofo murió, incrementa el contador y continúa. Al final retorna 0 (todos vivos).
Resumen del meal_lock:
El meal_lock es esencial porque:

Protege lecturas concurrentes - Evita leer valores inconsistentes
Sincroniza con la función eat() - Garantiza que la lectura sea atómica
Previene falsos positivos - Un filósofo no puede morir "accidentalmente" por datos obsoletos
Mantiene la integridad temporal - El tiempo de muerte se calcula con datos precisos

Sin este mutex, podrías tener muertes fantasma donde filósofos sanos son declarados muertos por condiciones de carrera.
¿Te queda claro ahora por qué es fundamental el meal_lock en esta función?ReintentarClaude aún no tiene la capacidad de ejecutar el código que genera.Claude puede cometer errores. Por favor, verifique las respuestas. Sonnet 4