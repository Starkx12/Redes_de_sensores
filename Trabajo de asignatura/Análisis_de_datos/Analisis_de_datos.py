"""
Author 1: Diego Remon
Author 2: Ignacio Latre
Date: 20-03-2024

This file contains the functions to analyze a time-lapse of body motion.
"""

#IMPORTS
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import math
import time  as t
import sys
import os
import csv

#Functions

def Get_timelapse(excel_path):
    
    time = []
    accX = []
    accY = []
    accZ = []
    gyrX = []
    gyrY = []
    gyrZ = []
    with open(excel_path,'r') as csv_file:
        csv_data = csv.reader(csv_file, delimiter=' ')
        for row in csv_data:            
            row_data = row[0].split(",")            
            try:            
                time.append(float(row_data[0]))
                accX.append(float(row_data[1]))
                accY.append(float(row_data[2]))
                accZ.append(float(row_data[3]))
                gyrX.append(float(row_data[4]))
                gyrY.append(float(row_data[5]))
                gyrZ.append(float(row_data[6]))
            except:
                pass
    
    return time, accX, accY, accZ, gyrX, gyrY, gyrZ
        
        
        
def draw_data(data,time,type_data):
    
    plt.plot(time,data)
    plt.xlabel('Time')
    if type_data == 0:  
        plt.ylabel('accX')
    elif type_data == 1:   
        plt.ylabel('accY')
    elif type_data == 2:  
        plt.ylabel('accZ')
    elif type_data == 3:  
        plt.ylabel('gyrX')
    elif type_data == 4:  
        plt.ylabel('gyrY')
    elif type_data == 5:  
        plt.ylabel('gyrZ')
    plt.grid()
    plt.show()


def get_dt(data,time):
    dt = []
    for i in range(len(time)):
        if i>0:
            dt.append((data[i]-data[i-1])/(time[i]-time[i-1]))
    return dt


def split_window(data_acc,time,th_accM,th_accm,th_dt):
    
    dt = get_dt(data_acc,time)
    n_measures=0
    n_windows = 0
    windows_times =[]
    time_start = 0
    time_stop =0
    for i in range(len(time)-1):
        if data_acc[i]<th_accM and data_acc[i] > th_accm and  abs(dt[i])<th_dt:
            n_measures+=1
            if n_measures ==5 and time_start==0:
                time_start = time[i]
            elif n_measures==5 and time_start!=0:
                n_windows+=1                
                time_stop= time[i]
                n_measures=0                
                windows_times.append([time_start,time_stop])
                time_start = 0
                time_stop =0
        else:
            n_measures=0
    
    return n_windows,windows_times        
        
def smooth_data(data,n_smooth):
    data_smoothed=[]
    
    for i in range(len(data)):
        if i > n_smooth-1:
            
            data_smoothed.append(sum(data[i-j] for j in range(n_smooth))/n_smooth)
    return data_smoothed
        
def show_splited(data,time,split_time,type_data):

    
    for time_range in split_time:
        t_min = time_range[0]
        t_max = time_range[1]
        print(t_min, t_max)
        idx_min = time.index(t_min)
        idx_max =  time.index(t_max)
        print(idx_min,idx_max)
        time_splited = time[idx_min:idx_max]
        data_splited = data[idx_min:idx_max]

        plt.plot(time_splited,data_splited)
        plt.xlabel('Time')
        if type_data == 0:  
            plt.ylabel('accX')
        elif type_data == 1:   
            plt.ylabel('accY')
        elif type_data == 2:  
            plt.ylabel('accZ')
        elif type_data == 3:  
            plt.ylabel('gyrX')
        elif type_data == 4:  
            plt.ylabel('gyrY')
        elif type_data == 5:  
            plt.ylabel('gyrZ')
        plt.grid()
        plt.show()
        input("Pulse para siguiente")

def get_time(data,time,split_time):

    tiempo = []
    tiempo_2 = []
    for time_range in split_time:
        t_min = time_range[0]
        t_max = time_range[1]
        
        idx_min = time.index(t_min)
        idx_max =  time.index(t_max)
        tiempo_2.append(abs(time[idx_max]-time[idx_min]))
        
        time_splited = time[idx_min:idx_max]
        data_splited = data[idx_min:idx_max]
        
        max_value = max(data_splited)
        idx_max =  data.index(max_value)
        tiempo_ms = time[idx_min]-time[idx_max]
        tiempo.append(abs(tiempo_ms))
    return tiempo,tiempo_2
        
def get_angle(data,time,split_time) :
    max_angle = []
    
    for time_range in split_time:
        t_min = time_range[0]
        t_max = time_range[1]
        
        idx_min = time.index(t_min)
        idx_max =  time.index(t_max)      
        
        time_splited = time[idx_min:idx_max]
        data_splited = data[idx_min:idx_max]
        
        max_angle.append(math.atan(max(data_splited))*360/(2*math.pi))
        
    return max_angle



#Main theta = f(wy,az) wy = gyroscopo y az = aceleracion en z

select = int(input("¿Que acción quieres hacer? \n-0:Show data \n-1:Show dt data \n-2:Split data \n-3:Test split"))
if select ==0:
    excel_path = r'C:\Users\Usuario\Desktop\Trabajo edge impulse\3\Puntillas.a3de23c6a901.csv'
    #excel_path = r'C:\Users\Usuario\Desktop\Trabajo edge impulse\Dataset\Dataset\7\Puntillas.56f0f0a705c7.csv'
    time, accX, accY, accZ, gyrX, gyrY, gyrZ = Get_timelapse(excel_path)
    n_smooth = 10
    accX = smooth_data(accX,n_smooth)
    accY = smooth_data(accY,n_smooth)
    accZ = smooth_data(accZ,n_smooth) 
    gyrX = smooth_data(gyrX,n_smooth)
    gyrY = smooth_data(gyrY,n_smooth)
    gyrZ = smooth_data(gyrZ,n_smooth)
    time = time[0:(len(time)-n_smooth)]
    
    data_zip = [accX, accY, accZ, gyrX, gyrY, gyrZ]
    i=0
    for data in data_zip:     
        draw_data(data,time,i)
        i+=1
        
elif select==1:
    excel_path = r'C:\Users\Usuario\Desktop\Trabajo edge impulse\3\Puntillas.a3de23c6a901.csv'
    #excel_path = r'C:\Users\Usuario\Desktop\Trabajo edge impulse\Dataset\Dataset\7\Puntillas.56f0f0a705c7.csv'
    time, accX, accY, accZ, gyrX, gyrY, gyrZ = Get_timelapse(excel_path)
    n_smooth = 8
    accX = smooth_data(accX,n_smooth)
    accY = smooth_data(accY,n_smooth)
    accZ = smooth_data(accZ,n_smooth)
    gyrX = smooth_data(gyrX,n_smooth)
    gyrY = smooth_data(gyrY,n_smooth)
    gyrZ = smooth_data(gyrZ,n_smooth)
    time = time[0:(len(time)-n_smooth)]
    
    data_zip = [accX, accY, accZ, gyrX, gyrY, gyrZ]
    i=0
    for data in data_zip:
        
        dt_data = get_dt(data,time)        
        draw_data(dt_data,time[0:len(time)-1],i)
        i+=1
        
elif select ==2:
    excel_path = r'C:\Users\Usuario\Desktop\Trabajo edge impulse\3\Puntillas.a3de23c6a901.csv'
    #excel_path = r'C:\Users\Usuario\Desktop\Trabajo edge impulse\Dataset\Dataset\7\Puntillas.56f0f0a705c7.csv'
    time, accX, accY, accZ, gyrX, gyrY, gyrZ = Get_timelapse(excel_path)
    n_smooth = 5
    accX = smooth_data(accX,n_smooth)
    accY = smooth_data(accY,n_smooth)
    accZ = smooth_data(accZ,n_smooth)
    gyrX = smooth_data(gyrX,n_smooth)
    gyrY = smooth_data(gyrY,n_smooth)
    gyrZ = smooth_data(gyrZ,n_smooth)
    time = time[0:(len(time)-n_smooth)]   
    
    results =[]
    porcentage = 0
    for th_dt in range(0,100,4):
        for th_accm in range(0,200,4):
            for th_accM in range(0,300,4):
                porcentage+=1
                n_windows,_=split_window(accX,time,th_accM/1000,th_accm/1000,th_dt/100)
                if n_windows == 30:
                    results.append([n_windows,th_accM,th_accm,th_dt])
                print("Porcentage: ", 100*porcentage/(100*200*300/4/4/4), " %\n")
    
    for i in range(len(results)):
        print("###############")
        print(results[i])
        print("###############")
    
elif select ==3:         
    excel_path = r'C:\Users\Usuario\Desktop\Trabajo edge impulse\3\Puntillas.a3de23c6a901.csv'
    #excel_path = r'C:\Users\Usuario\Desktop\Trabajo edge impulse\Dataset\Dataset\10\Puntillas.fb41b249e346.csv'
    time, accX, accY, accZ, gyrX, gyrY, gyrZ = Get_timelapse(excel_path)        
        
    n_smooth = 20
    accX = smooth_data(accX,n_smooth)
    accY = smooth_data(accY,n_smooth)
    accZ = smooth_data(accZ,n_smooth)
    gyrX = smooth_data(gyrX,n_smooth)
    gyrY = smooth_data(gyrY,n_smooth)
    gyrZ = smooth_data(gyrZ,n_smooth)
    time = time[0:(len(time)-n_smooth)]  
    _windows,time_split=split_window(accX,time,264/1000,44/1000,12/100)   
    show_splited(accX,time,time_split,1)
    
    time_info,time_range = get_time(accX,time,time_split)
    max_angle = get_angle(accX,time,time_split)   
    print(time_info)
    print("**********")
    print(time_range)
    print("**********")
    print(max_angle)
    print("**********")

   
    
   
    
   
    
   
    
   
    


