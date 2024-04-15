import socket

# Configure the server's IP address and port
SERVER_IP = '0.0.0.0'  # Use '0.0.0.0' to listen on all available interfaces
SERVER_PORT = 4240
 
# Set up a TCP/IP server
tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
 
# Bind the socket to server address and port 81
server_address = (SERVER_IP, SERVER_PORT)
tcp_socket.bind(server_address)
 
# Listen on port 81
tcp_socket.listen(1)
 
while True:
    print("Waiting for connection")
    connection, client = tcp_socket.accept()
 
    try:
        print("Connected to client IP: {}".format(client))
         
        # Receive and print data 32 bytes at a time, as long as the client is sending something
        while True:
            data = connection.recv(32)
            print("Received data: {}".format(data.decode('utf-8').rstrip()))
 
            if not data:
                break
 
    finally:
        connection.close()