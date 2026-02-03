.PHONY: right-console left-console

right-console:
	$(MAKE) -C sketches/right_console upload PORT=$(PORT)

left-console:
	$(MAKE) -C sketches/left_console upload PORT=$(PORT)

