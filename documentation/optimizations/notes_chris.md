# Optimization Notes by Chris

## Elligator 1: 
### string_to_point

Steps done:

- Use refactored bigints
- Precompute various constants
- Strength reductions (Change sign instead of mult with chi)
- Compute the powers of you jointly.
- Reduce number of bigints used for inbetween results
- Reduce the amopunt of copying data
- Use optimized functions


## BigInts: 
### add

Steps done:

- Explicitly use strength reductions (>> isntead of /, mask instead of mod)
- Create unrolled verion for most common case (size 8)
- Remove redundant double checking with sub function
- TODO Check AVX
### sub

Steps done:

- Explicitly use strength reductions (>> isntead of /, mask instead of mod)
- TODO Create unrolled verion for most common case (size 8)
- Remove redundant double checking with sub function
- TODO Check AVX

### squared

Steps done:

- New function for squaring a bigints
- Less data movement
- Reduces computation since results can be reduced
- TODO Check AVX
