import serial


import serial

# Configurar el puerto serie
ser = serial.Serial('COM9', 9600, timeout=1)  # Asegúrate de que 'COM9' sea el puerto correcto

try:
    while True:
        # Leer datos del puerto serie
        data = ser.readline().decode('utf-8').strip()
        
        # Imprimir los datos leídos
        print(data)

except KeyboardInterrupt:
    # Manejar la interrupción de teclado (Ctrl+C) para cerrar adecuadamente el puerto serie
    ser.close()
    print("Puerto serie cerrado.")
