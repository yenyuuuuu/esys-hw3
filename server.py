import socket
import json
import matplotlib.pyplot as plt
import numpy as np

HOST = '192.168.50.254'  # Standard loopback interface address (localhost) 192.168.50.254
PORT = 3000        # Port to listen on (non-privileged ports are > 1023)

def main():
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
                # print(data)
                j_data=json.loads(data)
                print("x=%.2f", j_data['x'])
                print("y=%.2f", j_data['y'])
                print("z=%.2f", j_data['z'])
        s.close()
    return

def plotter():
    return


if __name__ == '__main__':
    main()
