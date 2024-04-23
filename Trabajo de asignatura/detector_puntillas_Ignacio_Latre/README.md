# IMPLEMENTACIÓN DEL DETECTOR
Este script de Arduino contiene el código necesario para la detección del
movimiento de puntillas, obtniendo los siguientes datos:
* Tiempo que tarda en ponerse de puntillas (desde parado hasta puntillas máxima).
* Tiempo que permanece de puntillas (desde parado hasta parado).
* Ángulo máximo.
* Ángulo mínimo (mientras aguanta de puntillas).
* Ángulo medio (mientras aguanta de puntillas).
Tras esto se envian los datos recopilados median BLE.
## Explicación del código
El funcionamiento del programa es el siguiente:
1. Se incluyen las librerías externas.
2. Se inicializan los defines, las variables globales, la MEF, la interrupción y las carácteristicas de BLE.
3. Se inicializa el puerto serie, la comununicación BLE, el sensor y la interrupción periodica de 50Hz.
4. Dentro del bucle infinito se incializan las variables locales y se comprueba la conexión BLE.
5. Se comprueba si se quiere resetar el dispositivo mediante la comunicación BLE.
6. Se reopilan los datos de los sensores, se filtran y se saturan.
7. Se aplica la MEF mediante umbrales aplicados a los datos de los diferentes sensores.
8. Cuando la MEF permanece un tiempo en repsoso se resetea todo para evitar acumular errores.
9. Se envian los datos recapitulados mediante la comunicación BLE.
## Diagrama de funcionamiento
![img\DIAGRAMA.png](https://github.com/Starkx12/Redes_de_sensores/blob/main/Trabajo%20de%20asignatura/detector_puntillas_Ignacio_Latre/img/DIAGRAMA.png
)


