# Next: integrate with logger
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
from queue import Queue


class Plotter:

    def __init__(self, data_queue, save_path = None, interval = 1000):
        """ 
        REQUIRES: data_queue with 'x' and 'y' keys. E.g. externally data_queue.put({'x': 1, 'y': 1}) 
        """
        # TODO - add support for style = "scatter"
        # TODO - add "keep last" for not overflowing buffer
        # TODO - allow frames to be passed
        self.x = []
        self.y = []
        self.save_path = save_path
        self.data_queue = data_queue
        self.interval = interval

        self.fig, self.ax = plt.subplots()
        self.line, = self.ax.plot([], [], lw=2)
        # self.ax.set_xlim(0, 2*np.pi) 
        self.ax.set_ylim(-1, 1) # to avoid divide by zero when setting axis limits
        self.ani = FuncAnimation(self.fig, self.update,interval= self.interval) 

        # Setup callback to save figure on user closing window
        self.fig.canvas.mpl_connect('close_event', self.on_close)

    def __del__(self):
        self.stop() # to disconnect callback and save figure

    def stop(self):
        # TODO - issues with event_source being None
        # self.ani.event_source.stop() # need to disconnect the update callback to stop the animation
        # self.ani.disconnect(self.update) # disconnects the update callback
        try: 
            self.ani._stop() # stops the animation
            print("(plotter.stop): disconnecting animation callback")
        except AttributeError:
            print("(plotter.stop): AttributeError, animation callback not connected")
        self.save()
        plt.close(self.fig)

    def on_close(self, event):
        """ Callback for when user closes window """
        # Note the mpl handler passes the event as an arg
        print("(plotter.on_close): user close event, calling self.stop()")
        self.save()

    def show(self):
        # plt.show(self.fig)
        pass
        
    def save(self):
        """ Saves figure to the path stored in self.save_path"""
        if self.save_path:
            self.fig.savefig("{}".format(self.save_path))
            print(f"(plotter.save): saved figure to {self.save_path}")

    def store_datum(self, x, y):
        """ Store data in a list
        Separates the ingestion of data from the plotting of data"""
        self.x.append(x)
        self.y.append(y)
        print("(Plotter.store_datum): appended x='{}' (dtype={})".format(x, type(x)))
        print("(Plotter.store_datum): appended y='{}' (dtype={})".format(y, type(y)))

    def update(self, frame):
        """ Update function that is called by FuncAnimation
        Requires: frame/i passed by FuncAnimation
        Plots data from x and y member lists
        REQUIRES: data queue with 'x' and 'y' keys
        See store_datum(x,y) for adding data to members"""
        # See __init__ for adding queue to plotter
        print(f"(plotter.update): call to plotter.update (frame {frame})")

        while not self.data_queue.empty():
            data = self.data_queue.get()
            self.store_datum(float(data['x']), float(data['y']))
            # self.store_datum(frame, float(data['y']))
        
        # Update Plot
        if len(self.x) > 0:
            self.ax.set_xlim([min(self.x), max(self.x)])
            self.ax.set_ylim([min(self.y), max(self.y)])

        self.line.set_data(self.x, self.y)
        # self.fig.canvas.flush_events()
        plt.pause(0.001)
        return self.line, 


if __name__ == "__main__":
    # Demo with a queue
    # Expected: to plot all 10 points seemingly at once and then keep plotting

    q = Queue()

    # Enable fake data in update() for testing
    plot = Plotter(q, "test_fig.png")

    for i in range(10):
        q.put({'x': i, 'y': 1})

    plt.ion()
    ani = plot.ani 

    try: 
        while True:
            plt.pause(0.1) # need a small pause to allow animation updates
            pass
    except KeyboardInterrupt:
        plot.stop()
        plt.ioff()
        print("Exiting")
        exit(0)

# Later TODOs
# - allow ctrl-c to stop the plotting code and save the figure
# import atexit # needed for save on interrupt
#         atexit.register(self.on_exit)

