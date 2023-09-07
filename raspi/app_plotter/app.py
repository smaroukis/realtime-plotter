# 2023-09-07 
# issue with control flow / blocking when trying to put it in a class
# looking to recreate digikey post
# maybe an issue with scope, threading, ? 

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
import time

class Plotter(object):
    """Class for plotting."""
    def __init__(self):
        # Clear other plots
        plt.close('all')

        # Init Data
        # self.ax_x_lim = [0, 0]
        # self.ax_y_lim = [0, 0]
        self.x_data, self.y_data = [], []
        self.interval = 1000 # set to update every 1 second
        self.frames = 100

        # Init Plot
        self.fig, self.ax = plt.subplots()
        self.ani = None

    def __del__(self):
        pass
        # if not self.fig.closed:
        #     print("closing plot")
        #     self.fig.close()

    # Not in use
    def init_plot(self):
        self.scatter = self.ax.scatter([], [])
        return self.scatter,

    # Not in use
    def store_datapoint(self, x, y):
        """ Store data in a list
        Separates the ingestion of data from the plotting of data"""
        self.x_data.append(x)
        self.y_data.append(y)

    # TODO - make a buffer to remove old data points
    # Q: Is frames needed as i? 
    def update(self, i):
        """ Function that is passed to animation.FuncAnimation.
        frame increments by one each time the function is called."""
        # optionally slice e.g. xs=self.x_data[-50:]
        print("y({}) = {}".format(self.x_data[i], self.y_data[i]))

        # Update Plot
        # array_2d = np.c_[self.x_data, self.y_data]
        self.ax.clear()

        # TODO - need to figure out the control, 
        # HACK FOR EXAMPLE INCREMENT
        xs = self.x_data[:i]
        ys = self.y_data[:i]
        self.ax.plot(xs, ys) 
        # self.scatter.set_offsets(array_2d) # create 2D array with data in vertical columns
        return self.scatter,

    # TODO - add blitting for better performance 
    def animate(self):
        # Note on frames: If frames is not passed it continues indefinitely
        ani = FuncAnimation(self.fig, self.update, frames=self.frames, interval = self.interval)
        plt.show()
        return ani   

##  ---------------- MAIN -------------------------
##  ---------------- MAIN -------------------------

# Test Plotter
plotter = Plotter()
plotter.x_data = np.linspace(0, 10, 100)
plotter.y_data = np.sin(plotter.x_data)
plt.ion()

ani = plotter.animate()
plt.show()

# TODO - do we need a thread to run this and the for loop at the same time? 
# Try as global scope:
# ani = FuncAnimation(plotter.fig, plotter.update, frames=plotter.frames, init_func=plotter.init_plot, interval = plotter.interval)
# OR  as class funcction
# plotter.animate() # will update based on plotter.interval
# Issue is the plotter blocks? need a queue? or threading?

# Alternatively: have the animate/update functioVn call the sensor for data
