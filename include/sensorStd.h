#ifndef sensorStd_h
#define sensorStd_h

// Define a custom error value that we use 
// to return if a sensor is not ready to return
// a true value e.g. a timing condition has not been met
#define RETURN_NULL -999

/************************* Debugging *********************************/
#define DEBUG 1
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#define debugf(format, ...) Serial.printf(format, ##__VA_ARGS__)
#else
#define debug(x) 
#define debugln(x)
#endif
/********************************************************************/

#endif