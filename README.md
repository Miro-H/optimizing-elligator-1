# Censorship-avoiding high-speed EC (Elligator with Curve1174)
In this repository, we look into a performance optimized implementation of [Elligator](https://elligator.cr.yp.to/) in the context of the Advanced Systems Lab. Elligator is a system that maps random strings to points on elliptic curves. This should thwart censorship as it is difficult to detect that elliptic curve cryptography is used.

Some of our major optimizations, including exponentiation with constant exponents close to a power of two, can be applied to other problems: they are often useful in modular inversion with prime modulus or finding the quadratic residue. Furthermore, we provide conceptual implementations of Elligator 1 in C, Python, and Sage. The C implementation features a custom Big Integer library, which we optimized for Elligator 1.

As this course solely focussed on optimization techniques, we did not consider practical security concerns such as timing side-channels. This implementations should thus not be deployed in any production environment.

## Team Members
This project is developed by the following people:
- Christopher Vogelsanger
- Freya Murphy
- [Miro Haller](https://www.mirohaller.com)

## Optimizations

The repository is split into different folders containing the following optimization states:
- [Version 1](./code_unoptimized): Straightforward C, unoptimized
- [Version 2](./code_optimized/standard_optimizations): Uptimized C using standard techniques
- [Version 3](./code_optimized/advanced_optimizations): C including AVX and cache optimizations

The tests and plots can be configured to run for a specific version by setting the environment variable `VERSION`.

Examples:
```bash
# Run tests for version 2, execute in repo's root folder
VERSION=2 make run-tests

# Run plots for version 1, execute in ./timing folder
cd timing
VERSION=1 make plots
```

### Optimization Target

| Processor   | CPU base frequency | CPU max. frequency | Intel dev. model phase |
|-------------|--------------------|--------------------|------------------------|
| Manufacturer: Intel<br>Name: Haswell<br>Number: i7-4980HQ | 2.8 GHz | 4 GHz with Turbo Boost | Tock |

Target OS:
- macOS Big Sur 11.2.3
- Compiler:
    - Apple clang version 12.0.0 (clang-1200.0.32.29)
    - Target: x86_64-apple-darwin20.3.0
    - Thread model: posix

## Development

The separate readme [DEVELOPMENT.md](./DEVELOPMENT.md) provides more information on the code organization and Makefile targets.

## Further Resources

More information on this project can be found in [this blog post](https://www.mirohaller.com/posts/2021/07/Elligator-1/) and this [paper](https://www.mirohaller.com/files/ASL_Elligator_1_Optimization.pdf)
