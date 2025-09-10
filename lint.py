#!/usr/bin/env python

import sys
import os
import json
from typing import Counter
import clang.cindex

def find_compile_commands(start_dir):
    """Search upwards for compile_commands.json from start_dir."""
    cur = os.path.abspath(start_dir)
    while True:
        candidate = os.path.join(cur, 'compile_commands.json')
        if os.path.isfile(candidate):
            return candidate
        parent = os.path.dirname(cur)
        if parent == cur:
            return None
        cur = parent

def load_compile_commands(filename):
    cc_path = find_compile_commands(os.path.dirname(filename))
    if not cc_path:
        print("Could not find compile_commands.json.")
        return None
    with open(cc_path) as f:
        return json.load(f), cc_path

def find_command_for_file(commands, filename):
    filename_abs = os.path.abspath(filename)
    for entry in commands:
        entry_file = os.path.abspath(os.path.join(entry['directory'], entry['file']))
        if entry_file == filename_abs:
            return entry
    # If not found directly (e.g. header file), pick the first entry that includes the header in its command
    for entry in commands:
        if filename in entry['command']:
            return entry
    # As a fallback, return the first entry
    return commands[0] if commands else None

def parse_command_args(entry, filename):
    # Extract arguments from "command" or "arguments"
    if "arguments" in entry:
        args = entry["arguments"][1:]  # skip compiler
    else:
        # Naive split, does not handle quoted args with spaces perfectly
        import shlex
        args = shlex.split(entry["command"])[1:]
    # Remove -c <file> or the filename itself
    cleaned_args = []
    skip_next = False
    for i, arg in enumerate(args):
        if skip_next:
            skip_next = False
            continue
        if arg == filename or os.path.abspath(arg) == os.path.abspath(filename):
            continue
        if arg == "-c":
            skip_next = True
            continue
        cleaned_args.append(arg)
    return cleaned_args

def functions_in_file(filename: str) -> list[str]:
    # Load compile_commands.json
    cc, cc_path = load_compile_commands(filename)
    if not cc:
        print("WARNING! no compile_commands.json found!!!")
        print("Falling back to basic parsing (may fail for headers).")
        args = ['-x', 'c++', '-std=c++17']
    else:
        entry = find_command_for_file(cc, filename)
        args = parse_command_args(entry, filename)

    index = clang.cindex.Index.create()
    try:
        tu = index.parse(filename, args=args)
    except clang.cindex.TranslationUnitLoadError as e:
        print(f"Error parsing translation unit: {e}")
        sys.exit(1)

    if not tu.cursor:
        print("missing cursor")
        sys.exit(1)

    function_names: list[str] = []
    for node in tu.cursor.get_children():
        if node.kind in (clang.cindex.CursorKind.FUNCTION_DECL, clang.cindex.CursorKind.CXX_METHOD):
            if node.is_static_method():
                continue
            if node.location.file and os.path.abspath(node.location.file.name) == os.path.abspath(filename):
                # print(f"{node.spelling} (Line {node.location.line})")
                function_names.append(node.spelling)
    return function_names

def diff_header_and_source(header_path: str, source_path: str) -> None:
    header_names = functions_in_file(header_path)
    source_names = functions_in_file(source_path)
    same_order = Counter(header_names) == Counter(source_names)
    if not same_order:
        print(f"function defintions not in the same order in {header_path} and {source_path}")
        sys.exit(1)

for file_base in [
    'src/base/fs'
]:
    diff_header_and_source(f"{file_base}.h", f"{file_base}.cpp")
