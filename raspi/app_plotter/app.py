# NEXT - merge app.py 
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np

class Plotter:

    def __init__(self):
        self.x = []
        self.y = []

        self.fig, self.ax = plt.subplots()
        self.line, = self.ax.plot([], [], lw=2)
        self.ax.set_xlim(0, 2*np.pi) 
        self.ax.set_ylim(-1, 1)
        self.ani = FuncAnimation(self.fig, self.update, interval=100) 

    def __del__(self):
        pass
        # if not self.fig.closed:
        #     print("closing plot")
        #     self.fig.close()

    def store_datapoint(self, x, y):
        """ Store data in a list
        Separates the ingestion of data from the plotting of data"""
        self.x.append(x)
        self.y.append(y)

    def update(self, i):
        # Update Data
        x_i = 2*np.pi*i/100
        y_i = np.sin(x_i)
        self.store_datapoint(x_i, y_i)
        
        # Update Plot
        if i > 1: # fix error for numpy of one element
            self.ax.set_xlim([min(self.x), max(self.x)])
            self.ax.set_ylim([min(self.y), max(self.y)])

        self.line.set_data(self.x, self.y)
        return self.line, 

plotter = Plotter()
plt.show()