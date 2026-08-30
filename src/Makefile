FQBN = arduino:avr:nano
MONITOR_PORT = /dev/ttyUSB0

all:
	arduino-cli compile --output-dir=build --fqbn=$(FQBN) $(notdir $(CURDIR)).ino

upload:
	arduino-cli upload --input-dir=build -b $(FQBN) -p $(MONITOR_PORT) $(CURDIR)

clean:
	rm -rf $(CURDIR)/build

# monitor:
# 	screen $(MONITOR_PORT) 115200
