#!/usr/bin/env python3

import argparse
import re

HEADER = \
"""\
/**
 * This file is **autogenerated** by Python. Any manual changes to it will be
 * OVERWRITTEN!
 */

"""

LINE_FORMAT = "#define {0}{1}\n"

def unique_list(l):
    l_unique = []

    for e in l:
        if e not in l_unique:
            l_unique.append(e)

    return l_unique

if __name__ == '__main__':
    # Read arguments
    parser = argparse.ArgumentParser()

    parser.add_argument("--src_files", help="Source files to parse",
                        required=True, nargs="+")
    parser.add_argument("--dest_file", help="Destination file to write parsed enum to")
    parser.add_argument("--lookup_names", nargs="+",
                        help="Lookup the integer value of the given (text) benchmark types")
    parser.add_argument("--pattern", help="Pattern of elements to match",
                        default=r"([A-Z\d_]+TYPE_[A-Z\d_]+)[^A-Z]+")
    parser.add_argument("--add_translation", action="store_true",
                        help="Add array to translate numbers to names.")
    parser.add_argument("--array_name", help="Name of the translation array")
    parser.add_argument("--strip_prefixes", nargs="+", default=[""],
                        help="Prefixes to strip from matches")

    args = parser.parse_args()

    src_files       = args.src_files
    dest_file       = args.dest_file
    lookup_names    = args.lookup_names
    pattern         = args.pattern
    add_translation = args.add_translation
    array_name      = args.array_name
    strip_prefixes  = args.strip_prefixes

    if dest_file == None and lookup_names == None:
        print("ERROR: either --dest_file or --lookup_names must be specified")
        exit(1)

    if add_translation and array_name == None:
        print("ERROR: --array_name must be specified with --add_translation")
        exit(2)

    matches = []
    for src_file in src_files:
        with open(src_file, "r") as src_fp:
            regex   = re.compile(pattern)
            file_matches = regex.findall(src_fp.read())
        matches += file_matches

    matches = unique_list(matches)
    line_len = max([len(match) for match in matches]) + 1

    # Search last entry in matches
    last_entry = None
    for i, match in enumerate(matches):
        if match.endswith("_LAST"):
            last_entry = match
            matches = matches[:i] + matches[i+1:]

    if dest_file:
        with open(dest_file, "w+") as tar_fp:
            tar_fp.write(HEADER)

            # Include guard
            guard_macro = dest_file.split("/")[-1].split(".")[0].upper() + "_H_"
            tar_fp.write(f"#ifndef {guard_macro}\n#define {guard_macro}\n\n")

            # Sort all matches, except preserving the last entry
            matches_sorted = sorted(matches)
            if last_entry:
                matches_sorted.append(last_entry)

            for idx, match in enumerate(matches_sorted):
                tar_fp.write(LINE_FORMAT.format(match.ljust(line_len), idx))

            if add_translation:
                tar_fp.write("\n__attribute__((unused))")
                tar_fp.write(f"\nstatic char *{array_name}[] = {{\n")

                names = []
                for match in matches:
                    name = match
                    for strip_prefix in strip_prefixes:
                        name = name.replace(strip_prefix, "")
                    names.append(name.lower())

                names_sorted = sorted(names)
                if last_entry:
                    names_sorted.append(last_entry)

                for name in names_sorted:
                    tar_fp.write(f"    \"{name}\",\n")

                tar_fp.write(f"    \"{names[-1]}\"\n}};\n")

            tar_fp.write(f"\n#endif // {guard_macro}\n")

    if lookup_names:
        print(f"{matches_sorted.index(lookup_names[0])}", end="")
        for lookup_name in lookup_names[1:]:
            print(f" {matches_sorted.index(lookup_name)}", end="")
        print("")
