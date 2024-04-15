#IMPORTS
import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.transforms import BlendedGenericTransform

#VARIABLES
x_axis = 0.0
y_axis = 0.0
z_axis = 0.0
n_muestras = 50000
cont_muestras = 0
cnt_limit = 5
cnt = 0
mem_cnt = [0]*5
promedio = [0]*3
suma_cuadrados_diferencias = [0]*3
varianza = [0]*3




fig1, ax1 = plt.subplots()
ax1.set_xlabel('Number sample')
ax1.set_title('DATA')

x2 = [0,0,0,0,0]
x1 = [0,0,0,0,0]
y1 = [0,0,0,0,0]
y2 = [0,0,0,0,0]
y3 = [0,0,0,0,0]
y4 = [0,0,0,0,0]
y5 = [0,0,0,0,0]
y6 = [0,0,0,0,0]
ln1, = ax1.plot(x1, y1, '-',color='blue',label='ProemdioX')
ln2, = ax1.plot(x1, y2, '-',color='red',label='ProemdioY')
ln3, = ax1.plot(x1, y3, '-',color='green',label='ProemdioZ')
ln4, = ax1.plot(x1, y4, '*',color='blue',label='VarianzaX')
ln5, = ax1.plot(x1, y5, '*',color='red',label='VarianzaY')
ln6, = ax1.plot(x1, y6, '*',color='green',label='VarianzaZ')
ax1.legend()

#FUCNIONES

def update(frame):
    global x1,y1,y2,y3,x2,y4,y5,y6   
    x1.append(cont_muestras) 
    y1.append(promedio[0])
    y2.append(promedio[1])
    y3.append(promedio[2])
    y4.append(varianza[0])
    y5.append(varianza[1])
    y6.append(varianza[2])
    ln1.set_data(x1, y1) 
    ln2.set_data(x1, y2) 
    ln3.set_data(x1, y3)     
    ln4.set_data(x1, y4) 
    ln5.set_data(x1, y5) 
    ln6.set_data(x1, y6)
    fig1.gca().relim()
    fig1.gca().autoscale_view() 
    return ln1,ln2,ln3,ln4,ln5,ln6

ser = serial.Serial('COM15', 9600, timeout=1)  # Asegúrate de que 'COM9' sea el puerto correcto

try:
    print("Execute")
    while (cont_muestras < n_muestras) :
        print("Numero de meuestra")
        print(cont_muestras)
        # Leer datos del puerto serie
        data =  ser.readline().decode('utf-8').strip() 
        
        data_sub = data.split("/")
        
        # Verificar si hay al menos dos partes (indicando que hay un "/" presente)
        if len(data_sub) >= 2:
        # El substring estará entre el primer y segundo elemento de la lista
           if(cnt==0):
               mem_cnt = [0]*5
               promedio = [0]*3
               suma_cuadrados_diferencias = [0]*3
               varianza = [0]*3
               
           x_axis = data_sub[1]           
           y_axis = data_sub[2]
           z_axis = data_sub[3]            
           
           datos_nuevos = [x_axis, y_axis, z_axis] 
           mem_cnt[cnt] = datos_nuevos
           
           print(datos_nuevos)
           print(cnt)           
           if(cnt==4):
               for i in range(int(cnt_limit)):
                   if i == 0:
                       print("Calculo del promedio")
                   promedio[0] = float(promedio[0]) + float(mem_cnt[i][0])
                   promedio[1] = float(promedio[1]) + float(mem_cnt[i][1])    
                   promedio[2] = float(promedio[2]) +float(mem_cnt[i][2])
                   
                   if i == 4:
                       promedio[0] = float(promedio[0])/cnt_limit
                       promedio[1] = float(promedio[1])/cnt_limit
                       promedio[2] = float(promedio[2])/cnt_limit
                       print("RESULTADOS MEDIA")
                       print(promedio[0])
                       print(promedio[1])
                       print(promedio[2])
                       
                       
                                           
                       print("Calculo de la varianza")
                       
                       # Calcular la suma de los cuadrados de las diferencias entre cada dato y la media
                       suma_cuadrados_diferencias[0] = sum((float(mem_cnt[j][0]) - float(promedio[0])) ** 2 for j in range(int(cnt_limit)))
                       suma_cuadrados_diferencias[1] = sum((float(mem_cnt[j][1]) - float(promedio[1])) ** 2 for j in range(int(cnt_limit)))
                       suma_cuadrados_diferencias[2] = sum((float(mem_cnt[j][2]) - float(promedio[2])) ** 2 for j in range(int(cnt_limit)))
                       
                       # Calcular la varianza
                       varianza[0] = suma_cuadrados_diferencias[0] / cnt_limit
                       varianza[1] = suma_cuadrados_diferencias[1] / cnt_limit
                       varianza[2] = suma_cuadrados_diferencias[2] / cnt_limit
                       print("RESULTADOS VARIANZA")
                       print(varianza[0])
                       print(varianza[1])
                       print(varianza[2])
                   
               if(cont_muestras>2):
                   
                    nim1 = FuncAnimation(fig1, update, interval=100000, repeat = True,blit = False)             
                     
                    plt.show()
                    plt.pause(0.001)                        
                                          

                       
           cont_muestras = cont_muestras+1
           cnt = (cnt+1)%cnt_limit           
           
           
           
        else:
        # En caso de que no haya "/" en la cadena
            print("fail")     
        

except KeyboardInterrupt:
    # Manejar la interrupción de teclado (Ctrl+C) para cerrar adecuadamente el puerto serie
    ser.close()
    print("Puerto serie cerrado.")
