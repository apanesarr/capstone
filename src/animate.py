import time
import numpy as np
import matplotlib.colors as mc
import matplotlib.pyplot as plt
import matplotlib.animation as animation


def animate(N):
    fig, ax = plt.subplots(1, 1, tight_layout=True)
    def animate(i):
        pullData = open("coordinates.txt","r").read()
        dataArray = pullData.split('\n')
       
        #make an N*N matrix 
        data = np.ones((N, N)) * np.nan
        
        #get visited coordinates from textfile
        for eachLine in dataArray:
            if len(eachLine)>1:
                x,y = eachLine.split(',')
                data[int(x),int(y)] = 1
       
        #set values with 1 to blue
        my_cmap = mc.ListedColormap('b')
        
        # set the not visited grid to white
        my_cmap.set_bad(color='w', alpha=0)
        
        # draw the grid
        ax.clear()
        for x in range(N + 1):
            ax.axhline(x, lw=2, color='k', zorder=5)
            ax.axvline(x, lw=2, color='k', zorder=5)
   
        # draw the boxes
        ax.imshow(data, interpolation='none', cmap=my_cmap, extent=[0, N, 0, N], zorder=0)
        plt.title('Time (s): {}'.format(i), loc='right')
    ani = animation.FuncAnimation(fig, animate, interval=1000)
    plt.show()

if __name__ == "__main__":
    # execute only if run as a script
    animate(10)

