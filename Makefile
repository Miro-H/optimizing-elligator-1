# This command just forwards make commands to the src subdirectory

TARGETS 	:= all clean
SUB_DIRS 	:= tests timing

.PHONY: $(SUB_DIRS) $(TARGETS)

run-tests: tests

plots: timing

$(TARGETS): $(SUB_DIRS)

$(SUB_DIRS):
	make -C $@ $(MAKECMDGOALS)
