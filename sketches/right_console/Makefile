# Project Settings
USB_DEVICE_NAME = \"Right Console\"
USB_MANUFACTURER = \"Arduino\"
USB_VENDOR_ID = 0x2341   # Default
USB_PRODUCT_ID = 0x8041  # Default for Arduino Micro is 0x8037
FQBN = arduino:avr:micro
LIBS = ../../libraries

# Build paths
BUILD_DIR = ./build

.PHONY: all compile upload clean check-port

all: compile

compile:
	@echo "Compiling $(notdir $(CURDIR))..."
	arduino-cli.exe compile --fqbn $(FQBN) \
		--libraries $(LIBS) \
		--build-path $(BUILD_DIR) \
		--build-property "build.usb_flags=-DUSB_VID=$(USB_VENDOR_ID) -DUSB_PID=$(USB_PRODUCT_ID) '-DUSB_MANUFACTURER=$(USB_MANUFACTURER)' '-DUSB_PRODUCT=$(USB_DEVICE_NAME)'" \
		.

upload: check-port compile
	arduino-cli.exe upload -p $(PORT) \
		--fqbn $(FQBN) \
		--build-path $(BUILD_DIR) \
		.

clean:
	rm -rf $(BUILD_DIR)

check-port:
	@if [ -z "$(PORT)" ]; then \
		echo "------------------------------------------------"; \
		echo "ERROR: PORT variable is missing!"; \
		echo "Example: make upload PORT=COM3"; \
		echo "------------------------------------------------"; \
		echo; \
		echo "Detected Boards:"; \
		echo "================================================"; \
		arduino-cli.exe board list; \
		exit 1; \
	fi
