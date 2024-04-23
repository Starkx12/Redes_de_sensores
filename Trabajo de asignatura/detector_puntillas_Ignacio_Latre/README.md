# IMPLEMENTACIÓN DEL DETECTOR
Este script de Arduino contiene el código necesario para la detección del
movimiento de puntillas, obtniendo los siguientes datos:
* Tiempo que tarda en ponerse de puntillas (desde parado hasta puntillas máxima).
* Tiempo que permanece de puntillas (desde parado hasta parado).
* Ángulo máximo.
* Ángulo mínimo (mientras aguanta de puntillas).
* Ángulo medio (mientras aguanta de puntillas).
Tras esto se envian los datos recopilados median BLE.
##Explicación del código
