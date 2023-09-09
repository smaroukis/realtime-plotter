Simple Python MQTT Data Logger

This software uses the Python logger to create a logfile
for all messages for all topics to which this MQTT client
has subscribed.

The data can also be graphed in realtime using matploblib. Enable this with the `-g` switch

Note: by default it will only log changed messages. This is for sensors 
that send out their state a regular intervals but that state doesn't change
The program is run from the command line
You can subscribe to multiple topics.

Usage
```sh
python app.py -t # -d -h 192.168.0.17 -g 
```
> i.e. "all topics ("#"), for mqtt broker on host 192.168.0.17, include plotting (-g)

You need to provide the script with:

    List of topics to monitor
    broker name and port
    username and password if needed.
    base log directory and number of logs have defaults

Valid command line Options:
--help <help>
-h <broker> 
-b <broker default = "127.0.0.1">
-p <port>
-t <topic> 
-q <QOS>
-v <verbose>
-d logging debug 
-n <Client ID or Name>
-u Username 
-P Password
-s <store all data>\
-l <log directory default= SEE_CODE > 
-r <number of records default=100>\
-f <number of log files default= "unlimited">
-g <include graphing/plotting>

## Example Usage:

You will always need to specify the broker name or IP address 
and the topics to log

Specify broker and topics 

    python app.py -b 192.168.1.157 -t sensors/#

Specify broker and multiple topics

    python app.py -b 192.168.1.157 -t sensors/# -t  home/#
	

Log All Data:

    python app.py b 192.168.1.157 -t sensors/# -s 

Specify the client name used by the logger

    python app.py b 192.168.1.157 -t sensors/# -n data-logger

Specify the log directory

    python app.py b 192.168.1.157 -t sensors/# -l mylogs

## Plotter Class

The plotter class in `plotter.py` allows the data to be plotted and updated in real time with the matplotlib.animate.FuncAnimation function.
It will also save the figure upon closing, in the same directory as the log data.

The plotter and logger interact with each other via a queue system, since the `log_worker` is run in its own thread.

The Plotter class typically requires a `data_queue` Queue object and the filepath to save the figure to (passed from the `mlogger` class).
```
def __init__(self, data_queue, save_path = None, style = "line", _frames = 100, _update_rate = 1000):
```

In `log_worker` we push the data onto the plotter q (`q_plot`) which is processed by the `plotter.update()` member function that is passed as a callback to `FuncAnimation()`. The data is actually stored in the member variables `plotter.x` and `plotter.y`. 

```
x = results["time"]
y = results["message"]
q_plot.put({'x': x, 'y': y}) 
```

In the `plotter.update()` member function we have
```
while not self.data_queue.empty():
    data = self.data_queue.get()
    # self.store_datum(float(data['x']), float(data['y']))
    self.store_datum(frame, float(data['y']))
```
 

## Logger Class
> from original repo

The class is implemented in a module called m_logger.py (message logger).

To create an instance you need to supply three parameters:

    The log directory- defaults to mlogs
    Number of records to log per log- defaults to 5000
    Number of logs. 0 for no limit.- defaults to 0

log=m_logger(log_dir="logs",log_recs=5000,number_logs=0):

The logger creates the log files in the directory using the current date and time for the directory names.

The format is month-day-hour-minute e.g.


You can log data either in plain text format or JSON format.

To log data either in plain text then use the

    log_data(data) method.

To log data as JSON encoded data call the

    log_json(data) method.

Both method takes a single parameter containing the data to log as a 
string(log_data) or list(log_json) or dictionary(log_json).

e.g.

log.log_data(data) 
or
log.log_json(data)

#The log file will contain the data as 
#plain text or  JSON encoded data strings
#each on a newline.

The logger will return True if successful and False if not.

To prevent loss of data in the case of computer failure the logs are continuously flushed to disk .
 
 Read more about this application here:
