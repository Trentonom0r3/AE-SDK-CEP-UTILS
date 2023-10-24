import socket
from PIL import Image
from io import BytesIO

def main():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('127.0.0.1', 12345))
    server_socket.listen(1)

    print('Server is listening on port 12345...')

    while True:
        client_socket, addr = server_socket.accept()
        print(f'Connection from {addr}')

        data = b''
        while True:
            chunk = client_socket.recv(4096)
            if not chunk:
                break
            data += chunk
        
        client_socket.close()
        
        image_data = BytesIO(data)
        image = Image.open(image_data)
        image.show()

if __name__ == '__main__':
    main()
