# -*- coding: utf-8 -*-
"""
Created on Fri Feb  1 14:52:38 2019

@author: nishanth
"""

import seaborn as sns
import numpy as np
import pandas as pd
import csv

#temp_data = pd.read_csv('example_data.csv')
#temp_data.columns = ['x', 'y', 'Temperature']
#temp_data = temp_data.pivot('x', 'y', 'Temperature')

#display_data = sns.heatmap(temp_data, annot=True, cmap="Blues")

location_temp = {}

reader = csv.DictReader(open('example_data.csv'))
counter = 1
for row in reader:
    for x in row['X']:
        location_temp[(int(row['X']), int(row['Y']))] = int(row['Temperature'])
        
ser = pd.Series(list(location_temp.values()), index=pd.MultiIndex.from_tuples(location_temp.keys()))
df = ser.unstack().fillna(float('nan'))
df.shape
sns.heatmap(df, annot=True, cmap="Blues");