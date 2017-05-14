from telegram.ext import Updater
from telegram.ext import CommandHandler

from http.server import BaseHTTPRequestHandler, HTTPServer

import datetime
import logging
import time
import re

logging.basicConfig(
    format = '%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    level = logging.INFO
    )

class ToastParser(object):
    toastToken = '294460456:AAGYxax3Tply0Zc5Vy1d4E5TADA94dcUtK4'
    
    def __init__(self):
        self.utcStart = datetime.datetime.utcfromtimestamp(0)
        updater = Updater(self.toastToken)
        dispatcher = updater.dispatcher
        dispatcher.add_handler(CommandHandler(
            'toast', self.toast, pass_args = True
            ))
        dispatcher.add_handler(CommandHandler(
            'cancel', self.cancel
            ))
        dispatcher.add_handler(CommandHandler(
            'show', self.show
            ))

        self.dayList = (
            'mon','tues','wed','thurs','fri','sat','sun',
            'tmr','today'
            )
        
        self.bot = None
        self.chat_id = None
        self.timestamp = self.utcStart
    
        self.startTime = None
        self.endTime = None
        updater.start_polling()

    def generateTimestamp(self):
        self.timestamp = datetime.datetime.today()
        
    def reply(self, message):
        self.bot.sendMessage(
            chat_id = self.chat_id, text = message
            )

    def getDayOffset(self, arg):
        # match day (mon, tues, wed...        
        if arg == 'today':
            dayOffset = 0
        elif arg == 'tmr':
            dayOffset = 1
        else:
            weekdayNow = self.timestamp.weekday()
            dayOffset = self.dayList.index(arg) - weekdayNow

        return dayOffset

    def getTimeOfDay(self, arg):
        timeOfDay = 0
        hours,minutes = 0,0
        meridiem = arg[-2:]
        if meridiem == 'pm': hours += 12
        timeNum = arg[:-2]

        if '.' in timeNum:
            hours, minutes = timeNum.split('.')
            minutes = int(minutes)
            hours = int(hours)
        else:
            hours = int(timeNum)

        if minutes > 59:
            reply("cannot have %s minutes" % minutes)
            return
        if hours > 12:
            reply("cannot have %s hours" % hours)
            return
        
        timeOfDay += hours*3600
        timeOfDay += minutes*60
        return timeOfDay

    def getDuration(self, arg):
        duration = 0
                    
        for timeChunk in re.findall('\d+[hms]', arg):
            magnitudeMap = {'h': 3600, 'm': 60, 's': 1}
            magnitude = magnitudeMap[timeChunk[-1]]
            duration += int(timeChunk[:-1]) * magnitude

        return duration

    def extractValues(self, args):
        dayOffset = None
        timeOfDay = None
        duration = None
        
        for arg in args:
            if arg in self.dayList:
                if dayOffset != None: 
                    self.reply("error: multiple day arguments")
                    return
            
                dayOffset = self.getDayOffset(arg)

            elif re.match('^\d+\.?\d*[ap]m$', arg):
                # match time of day e.g: 2pm 3pm 2.30am 5.12pm
                if timeOfDay != None:
                    reply("error: multiple time arguments specified")
                    return
            
                timeOfDay = self.getTimeOfDay(arg)
                if timeOfDay == None: return
                print('TIME:', timeOfDay)

            elif re.match('^(\d+h)?(\d+m)?(\d+s)?$', arg):
                # match duration of toasting e.g:
                # 5h4s 7h4m32s 3900s 4m22s
                if duration != None:
                    reply("error: multiple duration arguments")
                    return
              
                duration = self.getDuration(arg)

        return(dayOffset, timeOfDay, duration)

    def getEpochTimes(self):
        if self.startTime == None: return
        
        startEpoch = int(self.startTime.timestamp())
        endEpoch = int(self.endTime.timestamp())
        return startEpoch, endEpoch

    def getDuration(self):
        times = self.getEpochTimes()
        if times != None:
            duration = times[1] - times[0]
            return duration
        else:
            return 0

    def showToast(self):
        if self.startTime == None:
            self.reply("No toast currently set :/")
        else:
            reply = "Toast information:\n"
            startDate = str(self.startTime)
            startDate = startDate[:startDate.rindex('.')]
            reply += "start: " + startDate + "\n"
            
            endDate = str(self.endTime)
            endDate = endDate[:endDate.rindex('.')]
            reply += "end: " + endDate + "\n"

            

            duration = self.getDuration()
            reply += "duration: %sh %sm %ss" % (
                duration//3600,
                (duration//60)%60,
                duration%60
                )
            self.reply(reply)

    def cancel(self, bot, update):
        self.chat_id = update.message.chat_id
        self.bot = bot

        if self.startTime == None:
            self.reply("No toasts were scheduled :/")
        else:
            self.startTime = None
            self.endTime = None
            
            self.reply("Toast cancelled!")

    def show(self, bot, update):
        self.chat_id = update.message.chat_id
        self.bot = bot
        self.showToast()
    
    def toast(self, bot, update, args):
        self.chat_id = update.message.chat_id
        self.bot = bot

        self.generateTimestamp()
        # fill in defaults
        values = self.extractValues(args)
        if values == None: return

        nowDaytime = (
            self.timestamp.hour*3600 +
            self.timestamp.minute*60 +
            self.timestamp.second
        )

        dayOffset, timeOfDay, duration = values
        if dayOffset == None: dayOffset = 0
        if timeOfDay == None: timeOfDay = nowDaytime
        if duration == None: duration = 1800
        
        if nowDaytime > timeOfDay and dayOffset == 0: dayOffset += 1
        elif dayOffset < 0: dayOffset += 7

        self.startTime = self.timestamp.replace(
            hour = timeOfDay//3600,
            minute = (timeOfDay//60)%60,
            second = timeOfDay%60,
            )
        
        self.startTime += datetime.timedelta(days = dayOffset)
        self.endTime = self.startTime
        self.endTime += datetime.timedelta(seconds = duration)

        #text = "now: " + str(self.timestamp) + "\n"
        #text += "start: " + str(self.startTime) + "\n"
        #self.reply(text)
        self.showToast()


# HTTPRequestHandler class
class testHTTPServer_RequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        # Send response status code
        self.send_response(200)

        # Send headers
        self.send_header('Content-type','text/html')
        self.end_headers()

        # Send message back to client
        epochTimes = toaster.getEpochTimes()
        if epochTimes != None:
            message = "on"
        else:
            message = "off"
        
        # Write content as utf-8 data
        self.wfile.write(bytes(message, "utf8"))
        print(message)
        return

toaster = ToastParser()
                      
server_address = ('', 8080)
httpd = HTTPServer(server_address, testHTTPServer_RequestHandler)
print('running server...')
httpd.serve_forever()



