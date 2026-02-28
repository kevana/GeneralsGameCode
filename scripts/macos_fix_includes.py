#!/usr/bin/env python3
"""
macos_fix_includes.py - Batch wrap Windows-only #include directives with
#ifdef _WIN32 guards across the Generals/GeneralsMD/Core source trees.

Usage:
    python3 scripts/macos_fix_includes.py [--dry-run]

Run from the repository root.
"""

import os
import re
import sys
from pathlib import Path

# Headers that only exist on Windows and should be guarded
WINDOWS_HEADERS = [
    'windows.h',
    'io.h',
    'mmsystem.h',
    'd3dx8.h',
    'd3d8.h',
    'objbase.h',
    'shlobj.h',
    'winsock.h',
    'winsock2.h',
    'atlbase.h',
    'ole2.h',
    'commdlg.h',
    'commctrl.h',
    'shellapi.h',
    'winreg.h',
    'psapi.h',
    'DbgHelp.h',
    'dbghelp.h',
]

# Directories to process (relative to repo root)
# Excludes Tools/ since they're not in the core build and have many legitimate
# Windows-only files that we don't need to compile on macOS.
TARGET_DIRS = [
    'Generals/Code/GameEngine',
    'Generals/Code/GameEngineDevice',
    'GeneralsMD/Code/GameEngine',
    'GeneralsMD/Code/GameEngineDevice',
    'Core/GameEngine',
    'Core/GameEngineDevice',
    'Core/Libraries/Source/WWVegas/WWLib',
    'Core/Libraries/Source/WWVegas/WW3D2',
]

# Subdirectories to always skip
SKIP_DIRS = {'build', 'Build', '.build', 'obj', 'Debug', 'Release', 'CMakeFiles'}

SOURCE_EXTENSIONS = {'.cpp', '.h', '.hpp', '.cxx', '.cc', '.inl'}


def compute_win32_guard_set(lines):
    """Return the set of line indices that are inside a #ifdef _WIN32 block."""
    inside = set()
    depth = 0
    win32_depths = set()  # depth levels where a Win32 guard was opened

    WIN32_PATTERNS = (
        re.compile(r'#\s*ifdef\s+_WIN32\b'),
        re.compile(r'#\s*ifdef\s+WIN32\b'),
        re.compile(r'#\s*if\s+defined\s*\(\s*_WIN32\s*\)'),
        re.compile(r'#\s*if\s+defined\s*\(\s*WIN32\s*\)'),
    )

    for i, line in enumerate(lines):
        s = line.strip()

        if re.match(r'#\s*(ifdef|ifndef|if)\b', s):
            depth += 1
            if any(p.match(s) for p in WIN32_PATTERNS):
                win32_depths.add(depth)
        elif re.match(r'#\s*endif', s):
            win32_depths.discard(depth)
            depth -= 1

        if win32_depths:
            inside.add(i)

    return inside


def fix_file(filepath, dry_run=False):
    """
    Process a single file and wrap unguarded Windows includes.
    Returns the number of changes made.
    """
    try:
        text = Path(filepath).read_text(encoding='utf-8', errors='replace')
    except Exception as e:
        print(f"  ERROR reading {filepath}: {e}", file=sys.stderr)
        return 0

    lines = text.splitlines(keepends=True)
    # Strip line endings for analysis, keep originals for output
    stripped = [l.rstrip('\r\n') for l in lines]

    guarded = compute_win32_guard_set(stripped)

    # Build include regex: matches #include <header> or #include "header"
    include_re = re.compile(
        r'^(\s*)#\s*include\s+[<"](' +
        '|'.join(re.escape(h) for h in WINDOWS_HEADERS) +
        r')[>"]'
    )

    result = []
    changes = 0

    for i, (orig, s) in enumerate(zip(lines, stripped)):
        m = include_re.match(s)
        if m and i not in guarded:
            indent = m.group(1)
            header = m.group(2)
            eol = '\n' if not orig.endswith('\r\n') else '\r\n'
            result.append(f'{indent}#ifdef _WIN32{eol}')
            result.append(orig if orig.endswith(('\n', '\r')) else orig + eol)
            result.append(f'{indent}#endif{eol}')
            changes += 1
            rel = os.path.relpath(filepath)
            print(f"  {rel}:{i+1}: wrapped <{header}>")
        else:
            result.append(orig if orig.endswith(('\n', '\r')) else s + '\n')

    if changes and not dry_run:
        try:
            Path(filepath).write_text(''.join(result), encoding='utf-8')
        except Exception as e:
            print(f"  ERROR writing {filepath}: {e}", file=sys.stderr)
            return 0

    return changes


def process_dir(root, target_dir, dry_run):
    dir_path = os.path.join(root, target_dir)
    if not os.path.isdir(dir_path):
        print(f"  Skipping (not found): {dir_path}")
        return 0, 0

    files_changed = 0
    total_wraps = 0

    for dirpath, dirnames, filenames in os.walk(dir_path):
        dirnames[:] = [d for d in dirnames if d not in SKIP_DIRS]
        for fname in sorted(filenames):
            if any(fname.endswith(ext) for ext in SOURCE_EXTENSIONS):
                fp = os.path.join(dirpath, fname)
                n = fix_file(fp, dry_run)
                if n:
                    files_changed += 1
                    total_wraps += n

    return files_changed, total_wraps


def main():
    dry_run = '--dry-run' in sys.argv

    repo_root = os.getcwd()
    if not os.path.isdir(os.path.join(repo_root, 'Generals')):
        print("ERROR: Run this script from the repository root (where Generals/ lives).")
        sys.exit(1)

    if dry_run:
        print("DRY RUN — no files will be modified.\n")

    grand_files = 0
    grand_wraps = 0

    for td in TARGET_DIRS:
        print(f"\n[{td}]")
        f, w = process_dir(repo_root, td, dry_run)
        if w:
            print(f"  => {f} files, {w} include(s) wrapped")
        else:
            print(f"  => nothing to do")
        grand_files += f
        grand_wraps += w

    print(f"\nTotal: {grand_files} files modified, {grand_wraps} includes wrapped.")
    if dry_run:
        print("(Dry run — rerun without --dry-run to apply.)")


if __name__ == '__main__':
    main()
