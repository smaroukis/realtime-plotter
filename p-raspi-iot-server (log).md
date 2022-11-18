#pinned #p/active (see `#p35` tags)
parent::[[P36-Home-Automation]]
garden-topic::[[RaspberryPi]]
child::[[Drawing p-home-iot-server.excalidraw]]
see-also:: [[p-esp32-mqtt (log)]]

Next Steps - Week 46 - Get Edge Device Up and Running
- [ ] see [[p-esp32-mqtt (log)]]
	- [x] blink esp32
- [ ] (at home) setup static IP in router

> [!NOTE]- Week 46 Goal Setting
> **Main Goal**: Get temperature sensor pushing data to the raspi server and graphing in grafana
> 
> **Secondary Goal**: One Example Code Reviewed in Vault Per Day
> 
> ![[p-raspi-iot-server-1668385460486.jpeg]]

Further

> [!NOTE]- Outstanding Tasks
> - [ ] Secure node-red (see [here](https://nodered.org/docs/user-guide/runtime/securing-node-red))
> 	![[p-raspi-iot-server-1667992115169.jpeg]]
> - [ ] only allow ssh connections from vpn or lan

> [!INFO]- Model 1 B+ Info
> ```
> **pi@raspberrypi**:**~ $** cat /proc/cpuinfo
> processor : 0
> model name : ARMv6-compatible processor rev 7 (v6l)
> BogoMIPS : 697.95
> Features : half thumb fastmult vfp edsp java tls 
> CPU implementer : 0x41
> CPU architecture: 7
> CPU variant : 0x0
> CPU part : 0xb76
> CPU revision : 7
> Hardware : BCM2835
> Revision : 0010
> Serial : 00000000b0875254
> Model : Raspberry Pi Model B Plus Rev 1.2
> ```

> [!TIP]- Review Info
>  Weekly reviews are located in the weekly review page, which we can pullup quickly with `#p35/rvw` and `#p35/rvw/learned`

## Log

## [[2022-W47-Nov]] - Week 3 of RasPi - esp32 setup
- see [[p-esp32-mqtt (log)]]

[[2022-11-18]]
- brainstorming mqtt flow in [[Drawing p-home-iot-server.excalidraw]]

### [[2022-11-15]] - mqtt client lib examples and research
 
### [[2022-11-14]] - installed influx db and mosquitto
- [[influxdb installation on RasPi]] && [[influxdb commands]]
- [[mosquitto installation]] && [[mosquitto broker common commands]]

---

## [[2022-11-W46]] - Learning RasPi, Corrupted SD Card, Installing Node Red

### [[2022-11-10]] 
- see DNP for notes on [[influxdb installation on RasPi]]

### [[2022-11-09]] new SD card
- #p35/rvw/learned found out that the sd card seemed to be corrupted, bought a new one 
- installed node red (finally) - see [[Node Red Hello World]]

### [[2022-11-08]] - [[THS]]
- docker iotstack was failing so I will install everything on the os
- talk w emery about esp - said he has some to use, he showed me the adafruit mqtt library 
	- also said to make sure you take notes on the influx db config, sonu dont fotget
	- he has notes on the [[influxdb]] config so ping him about that
- good idea - use the mac address to creat a unique id for the client and use that as part of the topic (then we can subscribe with wildcards)
- 

### [[2022-11-07]] (see DNP for verbose) - IOTstack issues building Dockerfile
- dur:: 120
- ✅ got the `docker-compose.yml` file created, fixed some docker group permissions issues, started to build and start the image, e.g. `iotstack_up` (aka `docker-compose up -d`)
	- ❌ docker build  seemed to fail on wireguard 
	- ❌ docker compose file not persisting after reboot
	- ❌ user groups not persisting after reboot
- next - node-red test and setup
- [ ] 🤔 should we update the firmware on the old raspi to help with hanging? 

---

### [[2022-11-06]]
- from https://sensorsiot.github.io/IOTstack/Basic_setup/, intstalled git and IoTstack
- installed iot stack into `~/IOTstack`, then ran `IOTstack/menu.sh` and it installed some new versions
- needed to rebout the raspberry pi then run sudo ./menu.sh from the iotstack dir again

### [[2022-11-05]] - iotstack and ssh
Connecting to RasPi over ssh
- https://www.raspberrypi.com/documentation/computers/remote-access.html#introduction-to-remote-access
```sh
sudo ssh <user>@<host> // e.g. pi@raspberrypi
```