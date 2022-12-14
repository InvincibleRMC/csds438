import math
import random
import numpy as np
import pandas as pd
from pandas import DataFrame
import matplotlib.pyplot as plot
from matplotlib.colors import ListedColormap

def main():
    # NOTE BEFORE TESTING COMPILE WITH -O3
    data: DataFrame = pd.read_csv('c_version/Group18Data.csv')
    print(data)
    customPlot(data,'Thread Count')

def customPlot(data: DataFrame, across: str):

    
    maxVal:int = max(data[across])
    print(maxVal)
    customcmap = ListedColormap(["red", "blue", "darkmagenta"])
    
    fig, axis = plot.subplots(figsize=(8, 6))
    # After Startup Color Data
    if('center_point' in data.columns):
        plot.scatter(data.iloc[:,0], data.iloc[:,1],  marker = 'o', 
                    c=data['center_point'].astype('category'), 
                    cmap = customcmap, s=80, alpha=0.5)
    else:
        plot.scatter(data.iloc[:,0], data.iloc[:,1],  marker = 'o')

    plot.scatter(center_points.iloc[:,0], center_points.iloc[:,1],  
                marker = 's', s=200, c=range(len(center_points.iloc[:,0])), 
                cmap = customcmap)
    axis.set_xlabel(r'petal_length', fontsize=20)
    axis.set_ylabel(r'petal_width', fontsize=20)
    plot.savefig('my_plot.png')
    

if __name__ == "__main__":
    main()

