#!/usr/bin/env python3

import argparse
import json
import logging
from http.server import SimpleHTTPRequestHandler, HTTPServer
import socket


def run(port=8000):
    server_address = ('', port)
    httpd = HTTPServer(server_address, CustomHandler)
    print(f"Starting httpd on port {port}...")
    httpd.serve_forever()


class ConfigurationError(Exception):
    pass


class CustomHandler(SimpleHTTPRequestHandler):

    def do_GET(self):
        client_ip, _ = self.client_address

        if self.path == "/":
            self.send_response(200)
            self.send_header("Content-type", "text/html")
            self.end_headers()
            response_string = f"<p>Your IP address is: <b>{client_ip}</b></p>"
            response_string += "<p>Now, click on the target port you have been assigned:</p><ul>"
            response_string += "<li><a href='/inst0'>Target port 8100</a>"
            response_string += "<li><a href='/inst1'>Target port 8101</a>"
            response_string += "<li><a href='/inst2'>Target port 8102</a>"
            response_string += "<li><a href='/inst3'>Target port 8103</a>"
            response_string += "<li><a href='/inst4'>Target port 8104</a></ul>"
            self.wfile.write(bytes(response_string, "utf8"))

        elif self.path == "/inst0":
            self.handle_instance_port(client_ip, 8100)

        elif self.path == "/inst1":
            self.handle_instance_port(client_ip, 8101)

        elif self.path == "/inst2":
            self.handle_instance_port(client_ip, 8102)

        elif self.path == "/inst3":
            self.handle_instance_port(client_ip, 8103)

        elif self.path == "/inst4":
            self.handle_instance_port(client_ip, 8104)
        else:
            self.send_response(404)
            self.end_headers()

    def handle_instance_port(self, client_ip, port):
        retcode, body = self.process_instruction(client_ip, port)
        self.send_response(retcode)
        self.end_headers()
        if body:
            self.wfile.write(bytes(body, "utf8"))

    def process_instruction(self, client_ip_address, port):
        print(f"Received a request from {client_ip_address} for port {port}")
        try:
            self.push_config(client_ip_address, "localhost", port)
            return 200, f"Configuration to port {port} has been successful!"
        except ConfigurationError:
            return 400, f"Configuration was unsuccessful. Please try again, or try another port"

    def gen_config(self, ip_address):
        config = {
            "evp": {"iot-platform": "tb", "version": "EVP2"},
            "mqtt": {"host": ip_address, "port": "1884"},
            "webserver": {"host": "localhost", "port": "8000"}
        }
        return json.dumps(config)

    def push_config(self, source, target, port):

        config = self.gen_config(source)
        payload = "'" + config + "'"
        self.log_message("Payload is %s", payload)
        with socket.create_connection((target, port)) as sock:
            sock.send(payload.encode())
            result = sock.recv(256).decode()

        if not result.startswith("Config rec"):
            raise ConfigurationError(f"Got instead: {result}")



if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--port", default=9000,
                        help="Port on which this server listens")
    args = parser.parse_args()

    run(port=args.port)
