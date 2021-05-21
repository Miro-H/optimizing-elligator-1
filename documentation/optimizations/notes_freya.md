# Optimization Notes by Freya

## Multiplication

### Basic

#### Miro's suggestions:
* strength reduction
* avoiding unnecessary function calls
* remove error checking, state assumptions in function description!

#### Ideas from meeting:
* `% BIG_INT_RADIX` could be replaced by `&`

#### Freya ideas:
* Check with Miro: can we remove the final `copy`?
* Maybe I can remove the `/` in the innermost `for` loop.
* Are we accessing consecutively?
* Look into `big_int_prune`

### AVX

#### Ideas from meeting:
* Vectorise to perform 4 mults at once

### Cache

## Elligator Point-to-String
#### Ideas from meeting:
* Chris pre-computed some curve values
