FQBN = arduino:avr:micro
LIBS = ./libraries
 # Default sketch for generation of compile_commands.json
DEFAULT_COMPILE_CMDS_SKETCH = ./sketches/right_console

# Find all sketch directories automatically
SKETCHES = $(patsubst sketches/%/,%,$(dir $(wildcard sketches/*/)))

.PHONY: all dump-spec $(SKETCHES)

# Examples:
#
# "make right_console" to compile sketch
#
# "make right_console-upload PORT=COM3" to upload sketch
#
$(SKETCHES):
	$(MAKE) -C sketches/$@ compile

%-upload:
	$(MAKE) -C sketches/$* upload

gen-compile-commands:
	@echo "Generating compilation database for $(SKETCH)..."
	arduino-cli compile --fqbn $(FQBN) \
		--libraries $(LIBS) \
		--build-path $(DEFAULT_COMPILE_CMDS_SKETCH)/build \
		--only-compilation-database \
		$(DEFAULT_COMPILE_CMDS_SKETCH)
	@# Move the generated file from the sketch's build folder to the root
	@cp $(DEFAULT_COMPILE_CMDS_SKETCH)/build/compile_commands.json .
	@echo "Success! compile_commands.json is now at the project root."
