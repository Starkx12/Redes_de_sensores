# EXTRACCIÓN DE DATOS
El objetivo de este código es obtener los datos del acelerometro y el giroscopio a partir del
Arduino Nano 33 BLE, ya que estos datos permiten conocer el estado del suejuto, más 
especificamente el movimiento de puntillas mediante la localización de este en el tobillo.
## Procedimiento
Para realizar la recolección de los datos den diferentes csv se deben seguir los siguientes pasos:
1. Cargar el programa en el Arduino Nano 33 BLE.
2. Colocar el dispositivo en el tobillo mediante cinta.
3. Conectar el disositivo al ordenador mediante un cable USB de 1,5m o más.
4. Ejecutrar el programa de lectura "serial-data-collect-csv.py" en Spyder.
5. Para comenzar la recolección pulsar la tecla 's'.
6. Realizar los mobimientos de puntillas.
7. Para parar pulsar la tecla 't'.
8. Los datos quedan almacenados en un archivo csv con un id único.

## Funcionamiento del programa "Get_Data.ino"
La secuencia de funcionamiento de este programa es:
1. Inicialización de las variables, el puerto serie y del sensor "IMU".
2. Cuando el dispositivo recibe por el puerto serie el comando de "k", este empieza a mandar con
   una frecuencia de 50Hz los datos del acelerometro y del giroscopio a través del puerto serie.
   Mientras tanto el led del dispositivo permanece encendido.
4. Cuando el dispositivo recibe por el puerto serie el comando de "w" la recolección de datos
   finaliza y se enviar un carácter vacio. Además el led se apaga.
5. Tras esto se debe esperar 1seg para la siguiente captura de datos.

## Funcionamiento del programa "serial-data-collect-csv.py"
La secuencia de funcionamiento de este programa es:
1. Inicialización del puerto serie e inicialización de las variables.
2. Si se pulsa la tecla 's' se envia por el puerto serie al dipositivo el comando "k"
   para comenzar la recolección de datos.
3. Mientras se reolecten datos se lee el puerto serie y se almacenan en un "buffer".
4. Si se pulsa la tecla 't' se envia por el puerto serie al dispositvo el comando "wW
   para termincar la recolección de datos.
5. Cuando se recibe un caracter vacio se almacena el buffer generado en un archivo cvs
   con una identificación unica.
6. Se resetan todas las variables y se espera a que se pulse la tecla 's'.
