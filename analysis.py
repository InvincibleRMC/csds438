import math
import random
import numpy as np
import pandas as pd
from pandas import DataFrame
import matplotlib.pyplot as plot
from matplotlib.colors import ListedColormap

def main():
    # NOTE BEFORE TESTING COMPILE WITH -O3
    data: DataFrame = pd.read_csv('data/Group18Data.csv')
    # print(data)
    customPlot(data,'Thread Count')
    customPlot(data,'Elements')

def customPlot(data: DataFrame, across: str):

    threadCount: str = 'Thread Count'
    elements: str = 'Elements'
    algorithm: str = 'Algorithm'
    time: str = 'Time'

    if threadCount == across:
        data = data[data[elements] == max(data[elements])]
        X =data[[algorithm,threadCount,time]]
    else:
        data = data[data[threadCount] == max(data[threadCount])]
        X =data[[algorithm,elements,time]]
  
    customcmap = ["red", "blue", "green","grey","orange","purple"]
    
    fig, axis = plot.subplots(figsize=(8, 6))
    algorithmNames = np.unique(X[algorithm])

    for i in range(len(algorithmNames)):
        algoData = X[X[algorithm] == algorithmNames[i]]
        x = np.array(algoData[across])
        y = np.array(algoData[time])
        plot.plot(x, y,label=str(algorithmNames[i]), marker='o', color=customcmap[i])

    plot.legend(loc='best')
    axis.set_xlabel(across, fontsize=20)
    axis.set_ylabel(r'Time', fontsize=20)
    plot.savefig('graphs/' + across +'.png')
    

if __name__ == "__main__":
    main()

