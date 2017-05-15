from toastParser import ToastParser

from http.server import BaseHTTPRequestHandler, HTTPServer

import logging
import time

logging.basicConfig(
    format = '%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    level = logging.INFO
    )

toaster = ToastParser()

# HTTPRequestHandler class
class testHTTPServer_RequestHandler(BaseHTTPRequestHandler):        
    def do_GET(self):
        # Send response status code
        self.send_response(200)

        # Send headers
        self.send_header('Content-type','text/html')
        self.end_headers()

        # Send message back to client        
        if toaster.isToasting():
            message = ' '.join([
                str(timestamp) for timestamp in toaster.getEpochTimes()
                ])
        else:
            message = "off"
        
        # Write content as utf-8 data
        self.wfile.write(bytes(message, "utf8"))
        print(message)
        return
                      
server_address = ('', 8080)
httpd = HTTPServer(server_address, testHTTPServer_RequestHandler)
print('running server...')
httpd.serve_forever()



