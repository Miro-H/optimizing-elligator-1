# Development
## Folder structure
This project is organized into the following folders:
- `src`: source code of this project.
- `tests`: testing infrastructure run by `make test` and the CI pipeline.

## Git
### Initial Setup
Don't forget to configure your name and email before you commit your first changes.
```
git config --global user.name "<YOUR NAME>"
git config --global user.email "<your eth username>@student.ethz.ch"
```

### Branching Model
The idea is to use the following simple branching model:
- **master**: this branch contains a _stable_ implementation (like the production environment in practice). You should never directly work on the _master_ branch.
- **dev**: this branch is intended for development. This branch should be used as source to create branches for new features. Completed features should be merged back into this branch. When a set of features is done and stable, this branch is merged back to _master_.
- **feature-branches**: those are branches that you should mainly work on. Whenever it makes sense, everyone should work on his own feature branch. Those branches should have descriptive names. Once a feature is done, you should create a _merge request_ to push it to the _dev_ branch. Every merge request must be reviewed by another team member. Moreover, it is only allowed to merge back if the CI pipeline ran successfully.
- **hotfix**: this is the exception to the rule. In case something is broken on the _master_ branch, which has to be fixed immediately, you can create a  branch directly from the _master_, apply the change and directly merge it back to the _master_ branch.

### .gitignore
We have a pretty restrictive `.gitignore` file. In case some of your changes do not appear on git status (i.e., you cannot commit them), check all `.gitignore` files, especially [this one](./.gitignore).

## Makefiles
This project uses Makefiles. A quick introduction to Makefiles can be found [here](https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/).

The following commands are in general supported:
- `make`: Compile the code in the current folder.
- `make clean`: Remove compilation artifacts, temporary files, and executables from the current folder. In general, it should always be possible to run this command without loosing any information that cannot be regenerated easily.
- `make test`: Run all tests in `$REPO_SRC/tests`.

## Testing
We use `Check` for unit tests for `C`. To run the tests, you need to install `Check` as described [on their website](https://libcheck.github.io/check/web/install.html). There is also a [reference](https://libcheck.github.io/check/doc/check_html/index.html) for how to write unit tests with `check`.

### Adding a new test case
To add a new test case, do the following:

- If there is already a test suite for this feature, add your test to the existing C file in [tests/src](tests/src). The basic test structure is the following:
```C
START_TEST(test_<your test name>)
{
    /* your test */
}
END_TEST
```
Next, you need to add your test case to the test suite (which is defined in the corresponding `*_suite` function):
```C
tcase_add_test(tc_basic, test_<your test name>);
```
For more information on `Check` unit tests, look at the [reference](https://libcheck.github.io/check/doc/check_html/index.html).
- If you want to add a new test suite, create a new C file in [tests/src](tests/src). For the format you can look at already existing files.
    **Important**: You need to add this new test file to the Makefile. First, add the following to compile it:
    ```Makefile
    # Specify the object files constituting the test
    _OBJ	= <test suite name>.o
    OBJ 	= $(patsubst %,$(ODIR)/%,$(_OBJ))
    # Specify the object files of the tested functions
    _OBJ_TESTED = <tested files>.o
    OBJ_TESTED 	= $(patsubst %,$(ODIR_TESTED)/%,$(_OBJ_TESTED))

    .PHONY: compile-<test suite name>
    compile-<test suite name>: all-tested <test suite name>

    <test suite name>: $(OBJ_TESTED) $(OBJ)
    	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)
    ```
    Replace `<test suite name>` with the name of the new C file (for which it would make sense to name it after the test suite). Replace `<tested files>` with the files from [src](./src) which are to be tested (thus, need to be compiled as well).
    Finally, add `compile-<test suite name>` to the `all` target in the Makefile:
    ```Makefile
    all: [...] compile-<test suite name>
    ```

### Running all tests locally
Execute `make run-tests` from the [repository root](./) or the [tests](./tests) folder.

## Code Quality
### Formating
Use the following coding conventions (to be extended/discussed):
- 4 spaces instead of taps
- `snake_case` for function and variable names.
- Max. 80 characters on a single line.

### Function Annotation
Use the following template to briefly describe your functions in the code. See also the code examples.
```
/**
* Put a *brief* description of the function here.
*
* @param vname describe parameters that the function takes as input.
*
* @return describe the return value of the function.
*/
```
