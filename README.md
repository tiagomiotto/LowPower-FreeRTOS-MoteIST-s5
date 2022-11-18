# Low power implementation of FreeRTOS with Low power Modes and DVFS

Using the MRMv2 to program the mote
1- first you need to list the devices available
	ls /dev/ttyUSB*

2- give permissions to acess the device
	sudo chmod 777 /dev/ttyUSB0 (ex)
	(!) if u list the devices you can notice that ttyUSB0 now appears available
		$>java -jar bootloader/MRM3.jar list

make clean && \
make && \
java -jar ~/Desktop/distv2/MRM2.jar program --fp=build/moteist.hex --s=512 --p=/dev/ttyUSB0 && \
sudo putty /dev/ttyUSB0 -serial -sercfg 115200,8,n,1,N

You can also use Minicom to have timestamped results. Configure before running

https://wiki.emacinc.com/wiki/Getting_Started_With_Minicom

make clean && \
make && \
java -jar ~/Desktop/distv2/MRM2.jar program --fp=build/moteist.hex --s=512 --p=/dev/ttyUSB0 && \
sudo minicom
