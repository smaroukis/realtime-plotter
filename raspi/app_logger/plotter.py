# Next: integrate with logger
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
from queue import Queue

class Plotter:

    def __init__(self, data_queue, save_path = None, _frames = 100, _update_rate = 1000):
        """ 
        REQUIRES: data_queue with 'x' and 'y' keys. E.g. externally data_queue.put({'x': 1, 'y': 1}) 
        """
        self.x = []
        self.y = []
        self.save_path = save_path
        self.data_queue = data_queue

        self.fig, self.ax = plt.subplots()
        self.line, = self.ax.plot([], [], lw=2)
        self.ax.set_xlim(0, 2*np.pi) 
        self.ax.set_ylim(-1, 1)
        self.ani = FuncAnimation(self.fig, self.update, frames = _frames, interval= _update_rate) 

        # Setup callback to save figure on user closing window
        self.fig.canvas.mpl_connect('close_event', self.on_close)

    def __del__(self):
        plt.close(self.fig)

    def save(self):
        """ Saves figure to the path stored in self.save_path"""
        if self.save_path:
            self.fig.savefig("{}".format(self.save_path))

    def on_close(self, event):
        # Note the mpl handler passes the event as an arg
        self.save()

    def store_datum(self, x, y):
        """ Store data in a list
        Separates the ingestion of data from the plotting of data"""
        self.x.append(x)
        self.y.append(y)
        print("(Plotter.store_datum): appended x='{}'".format(x))
        print("(Plotter.store_datum): appended y='{}'".format(y))

    def update(self, i):
        """ Update function that is called by FuncAnimation
        Requires: frame/i passed by FuncAnimation
        Plots data from x and y member lists
        REQUIRES: data queue with 'x' and 'y' keys
        See store_datum(x,y) for adding data to members"""
        print("(plotter.update): call to plotter.update")
        while not self.data_queue.empty():
            data = self.data_queue.get()
            self.store_datum(data['x'], data['y'])
        
        # Update Plot
        if i > 1: # fix error for numpy of one element
            self.ax.set_xlim([min(self.x), max(self.x)])
            self.ax.set_ylim([min(self.y), max(self.y)])

        self.line.set_data(self.x, self.y)
        plt.pause(0.001)
        return self.line, 

if __name__ == "__main__":
    # Enable fake data in update() for testing
    plotter = Plotter("test_fig")
    plt.show()


# Later TODOs
# - allow ctrl-c to stop the plotting code and save the figure
# import atexit # needed for save on interrupt
#         atexit.register(self.on_exit)

