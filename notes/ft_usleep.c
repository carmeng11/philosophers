// funcion para pausar la ejecución durantte pequeñas pausas que se indican en usleep(100) 0.1 ms, mientras se ejecuta el bucle. E tiempo que para es el tiempo que se pasa como parametro

void	ft_usleep(long long time_ms)
{
    long long	start;

    start = get_current_time_ms();
    while ((get_current_time_ms() - start) < time_ms)
        usleep(100);
}

La función ft_usleep sirve para pausar la ejecución durante un tiempo 
determinado en milisegundos (ms), pero de forma más precisa que usando directamente usleep.

Explicación paso a paso:
Obtener el tiempo de inicio:
start = get_current_time_ms();
Guarda el tiempo actual en milisegundos.

Bucle de espera:
while ((get_current_time_ms() - start) < time_ms)
Mientras no haya pasado el tiempo deseado (time_ms), sigue esperando.

Pequeñas pausas:
usleep(100);
Dentro del bucle, duerme el hilo durante 100 microsegundos (0.1 ms) en cada iteración.
Esto evita consumir demasiada CPU y permite comprobar frecuentemente si ya ha pasado el tiempo.

¿Por qué no usar solo usleep(time_ms * 1000)?
Porque usleep y sleep pueden ser imprecisos, especialmente si el sistema está ocupado.
Este método permite comprobar repetidamente el tiempo real y salir lo más cerca posible del tiempo solicitado.
En resumen:
ft_usleep es una función de espera precisa en milisegundos, útil para simulaciones como el 
problema de los filósofos.

. ¿Por qué (get_current_time_ms() - start) no es siempre 0?
start guarda el tiempo en milisegundos justo cuando empieza la función.
get_current_time_ms() dentro del bucle se va actualizando cada vez que se ejecuta la línea.
Al principio, la resta es casi 0, pero a medida que pasa el tiempo, get_current_time_ms() aumenta, así que la diferencia crece.
El bucle termina cuando esa diferencia es igual o mayor que time_ms.
Ejemplo:
Supón que start = 1000 (milisegundos).
Si quieres esperar 500 ms (time_ms = 500):

Al principio: get_current_time_ms() = 1000, diferencia = 0
Después de un rato: get_current_time_ms() = 1200, diferencia = 200
Cuando get_current_time_ms() = 1500, diferencia = 500 → el bucle termina.
2. ¿Qué es time_ms y dónde se define?
time_ms es el parámetro de la función:
Cuando llamas a ft_usleep, tú decides cuánto tiempo quieres esperar (en milisegundos).
Ejemplo de llamada:
3. ¿Propósito de la función?
Esperar exactamente el tiempo que le pidas (en milisegundos), comprobando el tiempo real en cada vuelta del bucle.
Es más preciso que usar solo usleep o sleep.
Resumen:

start es el tiempo inicial.
get_current_time_ms() va aumentando.
time_ms es el tiempo que quieres esperar (lo pasas como argumento).
El bucle termina cuando ha pasado ese tiempo.
¿Te gustaría ver un ejemplo de uso real en tu código? 


Perfecto, aquí tienes un ejemplo real de uso de ft_usleep en tu código de los filósofos:

Supón que quieres que un filósofo "coma" durante un tiempo determinado.
En tu función eat, podrías tener algo así:
// ...existing code...
static void eat(t_philo *philo)
{
    print_status(philo, EATING);

    // Actualizar tiempo de última comida y contador de comidas
    pthread_mutex_lock(&philo->meal_lock);
    philo->last_meal_time = get_current_time_ms();
    philo->meals_eaten++;
    pthread_mutex_unlock(&philo->meal_lock);

    // Aquí usas ft_usleep para esperar el tiempo de comer
    ft_usleep(philo->data->time_to_eat);
}
Aquí, philo->data->time_to_eat es el tiempo (en milisegundos) que el filósofo debe pasar comiendo.
La función ft_usleep se encarga de pausar la ejecución exactamente ese tiempo.

Resumen:
Cuando llamas a ft_usleep(500);, el hilo se detiene aproximadamente 500 ms, comprobando el tiempo real en cada vuelta del bucle.
Así puedes controlar con precisión cuánto tiempo "come", "duerme" o "piensa" cada filósofo.