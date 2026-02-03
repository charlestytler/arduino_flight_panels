FQBN = arduino:avr:micro
LIBS = ./libraries
 # Default sketch for generation of compile_commands.json
SKETCH = ./sketches/right_console

.PHONY: right-console left-console

right-console:
	$(MAKE) -C sketches/right_console upload PORT=$(PORT)

left-console:
	$(MAKE) -C sketches/left_console upload PORT=$(PORT)

gen-compile-commands:
	@echo "Generating compilation database for $(SKETCH)..."
	arduino-cli compile --fqbn $(FQBN) \
		--libraries $(LIBS) \
		--build-path $(SKETCH)/build \
		--only-compilation-database \
		$(SKETCH)
	@# Move the generated file from the sketch's build folder to the root
	@cp $(SKETCH)/build/compile_commands.json .
	@echo "Success! compile_commands.json is now at the project root."
