#!/usr/bin/env python3

"""
 Implements a simple HTTP/1.0 Server


 Code Based from: 
 https://www.codementor.io/@joaojonesventura/building-a-basic-http-server-from-scratch-in-python-1cedkg0842
"""
# sudo GPIOZERO_PIN_FACTORY=pigpio PIGPIO_ADDR=192.168.23.174 python3 httpserver.py

import socket
import os
import signal
# My Script
import lightScript

# Define socket host and port
SERVER_HOST = '0.0.0.0'
SERVER_PORT = 80
# HTTP Related
MAX_MESSAGE = 10000
HTTP_200H = 'HTTP/1.1 200 OK\n\n'
HTTP_404H = 'HTTP/1.1 404 Not Found\n\n'
# HACKED_IP
HACKED_IP = '192.168.23.174'

def main():
    # Create socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind((SERVER_HOST, SERVER_PORT))
    server_socket.listen(1)
    print('Listening on port %s ...' % SERVER_PORT)

    while True:
        # Wait for client connections
        client_connection, client_address = server_socket.accept()

        # Get the client request
        request = client_connection.recv(MAX_MESSAGE).decode()

        # Parse Request(GET, POST, PUT,... etc)
        parsed_request = request.split(" ", 2)
        verb = parsed_request[0]
        # Put in try catch to catch strange index error
        try:
            filename = parsed_request[1].replace("/", "")
        except IndexError as e:
            print("ERROR")
            pass
        # Formulate HTTP GET Response
        createHttpResponse(client_connection, verb, filename)
        
    # Close Socket
    server_socket.close()

# Creates HTTP response based on certain parameters
def createHttpResponse(client_connection, verb, filename):
    response = HTTP_200H
    if verb == "GET":
        if filename == "on":
            lightScript.turn_on(3)
            response = returnFile("index.html")
        elif filename == "swap":
            lightScript.swap(5)
            response = returnFile("index.html")
        elif filename == "" or filename == "index.html":
            response = returnFile("index.html")
        elif filename == "hack":
            lightScript.hack(5, HACKED_IP)
            response = returnFile("index.html")
        else:
            response = returnFile(filename)

    # Send Response back to client and close connection
    client_connection.sendall(response.encode())
    client_connection.close()

def returnFile(filename):
    try:
        with open(filename, "r") as file:
            fileData = file.read()
        return HTTP_200H + fileData
    except FileNotFoundError as e:
        with open('error.html', 'r') as file:
            fileData = file.read()
        return HTTP_404H + fileData

# Calls main again
main()
