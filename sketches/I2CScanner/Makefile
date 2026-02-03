# Project Settings
FQBN = arduino:avr:micro

# Build paths
BUILD_DIR = ./build

.PHONY: all compile upload clean check-port

all: compile

compile:
	@echo "Compiling $(notdir $(CURDIR))..."
	arduino-cli.exe compile --fqbn $(FQBN) \
		--build-path $(BUILD_DIR) \
		.

upload: check-port compile
	arduino-cli.exe upload -p $(PORT) \
		--fqbn $(FQBN) \
		--build-path $(BUILD_DIR) \
		.

monitor:
	arduino-cli.exe monitor -p $(PORT) \
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
