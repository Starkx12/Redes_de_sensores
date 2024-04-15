#IMPORTS
import serial

#VARIABLES
x_axis = 0.0
y_axis = 0.0
z_axis = 0.0
n_muestras =10
cont_muestras = 0
nombre_archivo = 'datos_giroscopo.txt'

#FUNCIONES
def verificar_archivo_vacio(nombre_archivo):
    try:
        with open(nombre_archivo, 'r') as archivo:
            contenido = archivo.read()
            if not contenido.strip():
                print(f"El archivo {nombre_archivo} está vacío.")
                return True
            else:
                print(f"El archivo {nombre_archivo} no está vacío.")
                return False
    except FileNotFoundError:
        print(f"El archivo {nombre_archivo} no existe.")
        return False
    except Exception as e:
        print(f"Error al leer el archivo {nombre_archivo}: {e}")
        return False






def almacenar_datos_en_txt(datos, nombre_archivo):
    try:
        with open(nombre_archivo, 'a') as archivo:
            
            # Escribir encabezados 
            if(cont_muestras ==0 and verificar_archivo_vacio(nombre_archivo)):
                archivo.write("Nº sample;Eje X; Eje Y;Eje Z\n")

            # Escribir datos al archivo
            for muestra in datos:
                # Formatear la muestra como una cadena con ";" como delimitador
                linea = ';'.join(map(str, muestra))
                
                # Agregar retorno de carro al final de cada muestra
                linea += '\n'

                # Escribir la línea en el archivo
                archivo.write(linea)

            print(f"Datos almacenados en el archivo {nombre_archivo}")
    except Exception as e:
        print(f"Error al almacenar datos: {e}")


# Configurar el puerto serie
ser = serial.Serial('COM15', 9600, timeout=1)  # Asegúrate de que 'COM9' sea el puerto correcto

try:
    while cont_muestras <n_muestras :
        # Leer datos del puerto serie
        data =  ser.readline().decode('utf-8').strip()        
        data_sub = data.split("/")
        
        # Verificar si hay al menos dos partes (indicando que hay un "/" presente)
        if len(data_sub) >= 2:
        # El substring estará entre el primer y segundo elemento de la lista
           x_axis = data_sub[1]           
           y_axis = data_sub[2]
           z_axis = data_sub[3]            
           
           datos_nuevos = [[cont_muestras,x_axis, y_axis, z_axis]]           

           # Añadir nuevas muestras al archivo existente
           almacenar_datos_en_txt(datos_nuevos, nombre_archivo)
           cont_muestras = cont_muestras+1
           
        else:
        # En caso de que no haya "/" en la cadena
            print("fail")     
        

except KeyboardInterrupt:
    # Manejar la interrupción de teclado (Ctrl+C) para cerrar adecuadamente el puerto serie
    ser.close()
    print("Puerto serie cerrado.")
