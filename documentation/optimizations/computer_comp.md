# Comparison of different devices

Please run the following commands on your device, while it is **idle** and with **turbo boost** (if you have it or something similar) disabled.

After running the command, you need to send me the log files (see below).

## Preparation
Make sure you are on `dev` and have the latest version of the code:
```
git checkout dev
git pull
```

## Chris
Laptop model:
```
TODO
```

Command to create logs for Elligator V1 to V3:
```
SKIP_COMP_TO_GMP=1 SETS=10 REPS=10 ./scripts/make_elligator_comp.sh
```

## Freya
Laptop model:
```
TODO
```

Command to create logs for Elligator V1 to V2:
```
MAX_VERSION=2 SKIP_COMP_TO_GMP=1 SETS=10 REPS=10 ./scripts/make_elligator_comp.sh
```

## Finding the log files
Find the line starting with `Paths to log files` in the command's output.

There's a list of paths to log folders, which are _separated by semicolon_. For each of them, create a zip file of the folder and name it for increasing versions. The first folder is for V1, the second for V2, the third for V3.
