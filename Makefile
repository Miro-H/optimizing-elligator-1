# This command just forwards make commands to the src subdirectory

TARGETS 	:= all clean
SUB_DIRS 	:= code_unoptimized code_optimized/standard_optimizations tests

.PHONY: $(SUB_DIRS) $(TARGETS)

run-tests: tests

$(TARGETS): $(SUB_DIRS)

$(SUB_DIRS):
	make -C $@ $(MAKECMDGOALS)
