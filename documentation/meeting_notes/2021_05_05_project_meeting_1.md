# One-on-One Project Meeting 5.5.2021
## Large problem sizes?

Inputs:
- Represent number differently
- Use different sizes for integers
    - E.g. to avoid doing the carry
    - Radix 51
        - https://www.chosenplaintext.ca/articles/radix-2-51-trick.html
- Check if there are AVX instructions for 64-bit integers?

## Security
We focus on optimization not security.

## Cost function and profiling tools
- Cost function
    - For big int operations
        - Also for integers
- Intel vtune
    - Cache misses
    - Used units

We need to **argue why optimization work**.

## Comparisons

- Compare with GMP
    - Compare runtime
- Runtime measurements are fine
- **Roofline plot** is important too

## Optimizations

- Lookup tables
    - Maybe we can precompute something
- Add a naive power optimization and measure the difference
- Use variables on the stack, get rid of `malloc`
    - **`malloc` is a bottleneck**
    - Do it on stack
    - Add macro to define variables on stack
- Students didn’t get large speedups last year with AVX
    - Must still try and include in report
    - Explain why they do not give good speedup
- Memory bound
    - The problem of teams from the previous year was not memory bound.
        - **Argue this for our code in report**.

## Report

- Should give a story (follow milestones roughly):
    1. Compiler flags, try them
    2. Find bottleneck
    3. Apply optimizations, show which performance we get
    4. Then look at other steps
- Show full understanding of the model
- Finish last steps after presentation is also fine
    - **90-95%** should be done at presentation
