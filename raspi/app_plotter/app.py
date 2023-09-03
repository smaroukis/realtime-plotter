import matplotlib.pyplot as plt
import numpy as np
import csv

# read csv file and plot
dataFile = "../logs/data.csv"

plt.ion() # interactive mode 
fig = plt.figure()

x_i = 0 # start of index
x = []
y = []

# TODO - test printing data from file to console, make sure time is formatted correctly
# Gets the next data point from the csv file
def getData(_file):
    with open(_file, 'r') as csvfile:
        data = csv.reader(csvfile, delimiter=',')
        # TODO - debug
        for row in data:
            return row[0]
    

if __name__ == '__main__':
    


# TODO/later - after debug
    while False: # get data as it comes in
        data = getData(dataFile)
        x.append(x_i)
        y.append(data)

        plt.scatter(x_i, float(data))
        x_i += 1
        plt.show()
        plt.pause(0.001)
