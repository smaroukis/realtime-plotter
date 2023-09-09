## example usage: python mqtt-data-logger.py -h 192.168.0.17 -d -t test -g

# modified from http://www.steves-internet-guide.com/download/mqtt-data-logger/
"""
This will log messages to file.Los time,message and topic as JSON data
"""
mqttclient_log=False #MQTT client logs showing messages
Log_worker_flag=True
import paho.mqtt.client as mqtt
import json
import os
import time
import sys, getopt,random
import logging
import mlogger as mlogger
import threading
from queue import Queue
from command import command_input
import plotter as plotter
import matplotlib.pyplot as plt
import command
import sys
print("Python version is", sys.version_info)

q_log =Queue()
q_plot = Queue()

class MQTTClient(mqtt.Client):#extend the paho client class
   run_flag=False #global flag used in multi loop
   def __init__(self,cname,**kwargs):
      super(MQTTClient, self).__init__(cname,**kwargs)
      self.last_pub_time=time.time()
      self.topic_ack=[] #used to track subscribed topics
      self.run_flag=True
      self.submitted_flag=False #used for connections
      self.subscribe_flag=False
      self.bad_connection_flag=False
      self.bad_count=0
      self.connected_flag=False
      self.connect_flag=False #used in multi loop
      self.disconnect_flag=False
      self.disconnect_time=0.0
      self.pub_msg_count=0
      self.pub_flag=False
      self.sub_topic=""
      self.sub_topics=[] #multiple topics
      self.sub_qos=0
      self.devices=[]
      self.broker=""
      self.port=1883
      self.keepalive=60
      self.run_forever=False
      self.cname=""
      self.delay=10 #retry interval
      self.retry_time=time.time()

def Initialise_clients(cname,mqttclient_log=False,cleansession=True,flags=""):
    #flags set
   print("initialising clients")
   logging.info("initialising clients")
   client= MQTTClient(cname,clean_session=cleansession)
   client.cname=cname
   client.on_connect= on_connect        #attach function to callback
   client.on_message=on_message        #attach function to callback
   #client.on_disconnect=on_disconnect
   #client.on_subscribe=on_subscribe
   if mqttclient_log:
      client.on_log=on_log
   return client

def on_connect(client, userdata, flags, rc):
   """
   set the bad connection flag for rc >0, Sets onnected_flag if connected ok
   also subscribes to topics
   """
   logging.debug("(on_connect):Connected flags"+str(flags)+"result code "\
    +str(rc)+"client1_id")
   if rc==0:
      
      client.connected_flag=True #old clients use this
      client.bad_connection_flag=False
      if client.sub_topic!="": #single topic
         logging.debug("subscribing "+str(client.sub_topic))
         print("subscribing in on_connect")
         topic=client.sub_topic
         if client.sub_qos!=0:
            qos=client.sub_qos
         client.subscribe(topic,qos)
      elif client.sub_topics!="":
         #print("subscribing in on_connect multiple")
         client.subscribe(client.sub_topics)

   else:
     print("set bad connection flag")
     client.bad_connection_flag=True #
     client.bad_count +=1
     client.connected_flag=False #

## Call Back, 
def on_message(client,userdata, msg):
    topic=msg.topic
    m_decode=str(msg.payload.decode("utf-8","ignore"))
    message_handler(client,m_decode,topic)
    
# TODO - can remove json parsing here
# TODO - look into formatting for writing to csv file
# Affects: Updates a data dictionary and adds it to the queue for logging and plotting
# Time is a UTC float
def message_handler(client,msg,topic):
    data=dict()
    tnow=time.time() # in UTC (float)
    # tnow_local=time.localtime(time.time())
    # m=time.asctime(tnow_local)+" "+topic+" "+msg

    # Update data dict
    data["time"]=tnow
    data["topic"]=topic
    data["message"]=msg # either raw message or JSON object depending on previous try-except

    # Put the data in the queue
    if command.options["storechangesonly"]:
        if has_changed(client,topic,msg):
            client.q_log.put(data) #put messages on queue
            # NOTE - putting messages on queue will trigger the log_worker which also calls the Plotter
    else:
        client.q_log.put(data) #put messages on queue

    logging.debug("(message_handler): put data in queue, data='{}'".format(data))

def has_changed(client,topic,msg):
    topic2=topic.lower()
    if topic2.find("control")!=-1:
        return False
    if topic in client.last_message:
        if client.last_message[topic]==msg:
            return False
    client.last_message[topic]=msg
    return True

# Separate Thread Worker for Processing the Queue and Sending it for Log
# Requires: mlogger.py log_json(), thread is started in main
# Runs in the background in infinite loop to process the queue
# HANDLES FORMATTING OF DATA (JSON/RAW)
def log_worker():
    """runs in own thread to log data from queue and add data to plot queue"""
    while Log_worker_flag:
        time.sleep(0.01)
        # Retrieve next queue item until empty
        while not q_log.empty():
            results = q_log.get()
            if results is None:
                continue
            # JSON DATA
            if options["JSON"]:
                try: 
                    results_json = json.dumps(json.loads(results)) 
                    # logging.debug("(log_worker): parsed json data")
                except:
                    logging.debug("(log_worker): couldnt parse json data, parsed as string")
                    log.log_json(str(results_json))
            # RAW DATA - TODO/later Add Header for CSV if first time 
            else:
                # logging.debug("(log_worker): parsed raw data")
                # TODO check how float is converted
                results_csv = "{}, {}, {}".format(results["time"], results["topic"], results["message"])
                log.log_data(results_csv)

            # Plotting 
            # add to plotting queue, see plotter update 
            x = results["time"]
            y = results["message"]
            q_plot.put({'x': x, 'y': y}) 
            logging.debug("(log_worker): put messages on plotting queue")

    log.close_file()

# ---------------- MAIN -------------------------

options=command.options ## load the options from the command.py module

if __name__ == "__main__" and len(sys.argv)>=2:
    options=command_input(options)
else:
    print("Need broker name and topics to continue.. exiting")
    raise SystemExit(1)

# Setup Debug Logging (see command.py for default log level)
if options["loglevel"]:
    try:
        logging.basicConfig(level=getattr(logging, options["loglevel"].upper()))
    except AttributeError:
        print(f"Invalid log level: {options['loglevel'].upper()}. Using default log level.")
        logging.basicConfig(level=logging.debug)  # Keep it verbose since we tried to pass a log level
else:
    pass

# Setup File Name
if not options["cname"]:
    r=random.randrange(1,10000)
    cname="logger-"+str(r)
else:
    cname="logger-"+str(options["cname"])
log_dir=options["log_dir"]

## Create Message Logger 
log_records=options["log_records"]
number_logs=options["number_logs"]
log = mlogger.m_logger(log_dir,log_records,number_logs) #create log object
print("Log Directory =",log_dir)
print("Log records per log =",log_records)
if number_logs==0:
    print("Max logs = Unlimited")
else:
    print("Max logs  =",number_logs)
    
logging.info("creating client"+cname)

## Create MQTT Client
client=Initialise_clients(cname,mqttclient_log,False)#create and initialise client object
if options["username"] !="":
    client.username_pw_set(options["username"], options["password"])

client.sub_topics=options["topics"]
client.broker=options["broker"]
client.port=options["port"]

if options["JSON"]:
    print("Logging JSON format")
else:
    print("Logging plain text")
if options["storechangesonly"]:
    print("starting storing only changed data")
else:
    print("starting storing all data")
    
## Setup Plotter
if options["plotter"]:
    plot_saveas = "{}.png".format(log.file_name)
    logging.getLogger('matplotlib.font_manager').disabled = True # to silence matplotlib.font_manager debug output
    plot = plotter.Plotter(q_plot, plot_saveas, interval=100)
    logging.info("Created plotter → {}".format(plot_saveas))

    # plt.ion() # might need to include matplotlib
    ani = plot.ani
else: 
    logging.debug(("PLOTTING NOT ENABLED"))

## Set Up Thread for Log Worker
#Log_worker_flag=True
t = threading.Thread(target=log_worker) #start logger
t.start() #start logging thread
###

client.last_message=dict()
client.q_log = q_log #make queue available as part of client

# Check for local broker, else connect to remote
try:
    res=client.connect(client.broker,client.port) # connect to broker, default is localhost
    client.loop_start() # start loop

except:
    logging.debug("connection to ",client.broker," failed")
    raise SystemExit("connection failed")

try:
    # Loop until a keyboard interrupt
    while True:
        plt.pause(0.1) # need small pause to allow animation to update
        pass 
except KeyboardInterrupt:
    print("interrrupted by keyboard")

# If keyboard interrupt, handle shutdown
if options["plotter"]:
    plot.stop()
    plt.ioff()
client.loop_stop() #start loop
Log_worker_flag=False #stop logging thread
time.sleep(5)