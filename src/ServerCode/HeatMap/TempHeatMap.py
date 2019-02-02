# -*- coding: utf-8 -*-
"""
Created on Fri Feb  1 14:52:38 2019

@author: nisha
"""

import seaborn as sns
import numpy as np
import pandas as pd

temp_data = pd.read_csv('example_data.csv')
temp_data.columns = ['x', 'y', 'Temperature']
temp_data = temp_data.pivot('x', 'y', 'Temperature')

display_data = sns.heatmap(temp_data, annot=True, cmap="Blues")