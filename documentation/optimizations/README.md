# Optimizations
This folder contains information on the optimization process.

## Makefile Integration
The repository is split into different folders containing the following optimization states:
- [Version 1.0](../../code_unoptimized): Straightforward C, unoptimized
- [Version 2.0](../../code_optimized/standard_optimizations): Uptimized C using standard techniques
- [Version 3.0](../../code_optimized/advanced_optimizations) (in the future): C including AVX and cache optimizations

The tests and plots can be configured to run for a specific version by setting the environment variable `VERSION`.

Examples:
```bash
# Run tests for version 2.0, execute in repo's root folder
VERSION=2.0 make run-tests

# Run plots for version 1.0, execute in ./timing folder
cd timing
VERSION=1.0 make plots
```

## Optimization Target

### Target processor

| Processor   | CPU base frequency | CPU max. frequency | Intel dev. model phase |
|-------------|--------------------|--------------------|------------------------|
| Manufacturer: Intel<br>Name: Haswell<br>Number: i7-4980HQ | 2.8 GHz | 4 GHz with Turbo Boost | Tock |

### Target OS
- macOS Big Sur 11.2.3
- Compiler:
    - Apple clang version 12.0.0 (clang-1200.0.32.29)
    - Target: x86_64-apple-darwin20.3.0
    - Thread model: posix

## Notes
The following files contain individual notes:
- [Chris](./notes_chris.md)
- [Freya](./notes_freya.md)
- [Miro](./notes_miro.md)
