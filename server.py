import socket
import json
import matplotlib.pyplot as plt
import numpy as np

HOST = '192.168.50.254'  # Standard loopback interface address (localhost) 192.168.50.254
PORT = 3000        # Port to listen on (non-privileged ports are > 1023)

# use ggplot style for more sophisticated visuals
plt.style.use('ggplot')

def main():
    size = 100
    t_vec = np.linspace(0,1,size+1)[0:-1]
    x_vec = np.zeros(size)
    y_vec = np.zeros(size)
    z_vec = np.zeros(size)
    line_x=[]
    line_y=[]
    line_z=[]
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        print('listen...')
        s.listen()
        conn, addr = s.accept()
        with conn:
            print('Connected by', addr)
            while True:
                data = conn.recv(1024).decode('utf-8')
                if not data:
                    print("no data!!!")
                    break
                print(data)
                j_data=json.loads(data)
                x_vec = np.append(x_vec[1:],j_data['x'])
                y_vec = np.append(y_vec[1:],j_data['y'])
                z_vec = np.append(z_vec[1:],j_data['z'])
                line_x, line_y, line_z = plotter(t_vec,x_vec,y_vec,z_vec,line_x,line_y,line_z)
            s.close()
            return
    return


def plotter(t_vec,y1_data,y2_data,y3_data,line1,line2,line3,identifier='',pause_time=0.5):
    if (line1==[]) and (line2==[]) and (line3==[]):
        # this is the call to matplotlib that allows dynamic plotting
        plt.ion()
        fig = plt.figure(figsize=(13,6))
        ax = fig.add_subplot(111)
        # create a variable for the line so we can later update it
        line1, = ax.plot(t_vec,y1_data,'-o',alpha=0.8,color='purple')
        line2, = ax.plot(t_vec,y2_data,'-o',alpha=0.8,color='red')
        line3, = ax.plot(t_vec,y3_data,'-o',alpha=0.8,color='blue')        
        #update plot label/title
        plt.ylim([-1000,1000])
        plt.ylabel('Acc')
        plt.title("Acc (to time t)")
        plt.show()
    

    # after the figure, axis, and line are created, we only need to update the y-data
    line1.set_ydata(y1_data)
    line2.set_ydata(y2_data)
    line3.set_ydata(y3_data)
    # adjust limits if new data goes beyond bounds
    
    # this pauses the data so the figure/axis can catch up - the amount of pause can be altered above
    plt.pause(pause_time)
    
    # return line so we can update it again in the next iteration
    return line1,line2,line3

if __name__ == '__main__':
    main()
