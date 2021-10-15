The reboot_overclock command was created to produce a better emulation for Sega
Saturn on a Raspberry Pi 4b.
When called, it executes the following instructions:

- backup /boot/firmware/config.txt as /boot/firmware/config.txt.bkp
- append /boot/firmware/config_overclock.txt into /boot/firmware/config.txt
- reboot

The file "config.txt" must be previously prepared with the underclocking
parameters: dvfs, arm_freq_min, gpu_freq_min, and over_voltage_min; and the
kernel should be configured with Ondemand as frequency scaling governor.

The file "config_overclock.txt" sets the overclocking parameters: arm_freq,
gpu_freq and over_voltage.

The /etc/rc.local must be set with the following code (in order to automatically
revert the overclock after a second reboot):

if [ -s /boot/firmware/config.txt.bkp ]
 then
  mv /boot/firmware/config.txt.bkp /boot/firmware/config.txt
fi

If in doubt overclocking in a raspberry pi 4, then read these:

https://www.seeedstudio.com/blog/2020/02/12/how-to-safely-overclock-your-raspberry-pi-4-to-2-147ghz/
https://qengineering.eu/overclocking-the-raspberry-pi-4.html
https://raspberrypi.stackexchange.com/questions/123892/why-dvfs-is-not-working-on-my-raspberry-pi-4-model-b
https://www.raspberrypi.org/documentation/computers/raspberry-pi.html#using-dvfs
https://www.raspberrypi.org/documentation/computers/config_txt.html#gpu_mem_256

---------------------------------------

If you want to use reboot_overclock inside emulationstation, then do the following:
- configure your rc.local (with the code above)
- copy config_overclock.txt into /boot/firmware
- edit or copy config.txt into /boot/firmware

then run in the current directory:
cmake .
make
sudo make install
