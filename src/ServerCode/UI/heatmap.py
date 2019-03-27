# -*- coding: utf-8 -*-
"""
Created on Tue Mar 26 11:56:00 2019

@author: nishanth
"""
import seaborn as sns
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import csv

#fig = plt.figure()
#ax1 = fig.add_subplot(1,1,1)

def draw_heatmap():
    location_temp = {}
    #plt.ion()
    reader = csv.DictReader(open('data/example_data.csv'))
    for row in reader:
        for x in row['X']:
            location_temp[(int(row['X']), int(row['Y']))] = int(row['Temperature'])
        
    ser = pd.Series(list(location_temp.values()), index=pd.MultiIndex.from_tuples(location_temp.keys()))
    df = ser.unstack().fillna(float('nan'))
    df.shape
    drawing = sns.heatmap(df, annot=True, cmap="Blues");
    #ax1.plot(df)
    #fig.clear()
    figure = drawing.get_figure()
    figure.savefig('static/heatmap.png')
    
draw_heatmap()

#ani = animation.FuncAnimation(fig, draw_heatmap, interval=1000)
#plt.show()