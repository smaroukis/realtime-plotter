import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np

XS = np.linspace(0, 2*np.pi, 100)
YS = np.sin(XS)
x = []
y = []

class Plotter:

    def __init__(self):
        self.fig, self.ax = plt.subplots()
        self.line, = self.ax.plot([], [], lw=2)
        self.ax.set_xlim(0, 2*np.pi) 
        self.ax.set_ylim(-1, 1)
        self.ani = FuncAnimation(self.fig, self.update, interval=100) 

    def update(self, i):
        x_i = 2*np.pi*i/100
        x.append(x_i)
        y.append(np.sin(x_i))
        if i > 0: # fix error for numpy of one element
            self.ax.set_xlim([min(x), max(x)])
            self.ax.set_ylim([min(y), max(y)])

        self.line.set_data(x, y)
        return self.line, 

plotter = Plotter()
plt.show()