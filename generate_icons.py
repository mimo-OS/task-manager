#!/usr/bin/env python3
"""Validate the canonical SVG icons and optionally copy them to another directory."""

import argparse
import shutil
import xml.etree.ElementTree as ET
from pathlib import Path


SOURCE_DIR = Path(__file__).resolve().parent / "icons"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=SOURCE_DIR,
        help="directory to copy the canonical SVG icons into",
    )
    output_dir = parser.parse_args().output_dir
    icons = sorted(SOURCE_DIR.glob("*.svg"))
    if not icons:
        parser.error(f"no SVG icons found in {SOURCE_DIR}")

    output_dir.mkdir(parents=True, exist_ok=True)
    for source in icons:
        root = ET.parse(source).getroot()
        if root.tag != "{http://www.w3.org/2000/svg}svg":
            raise ValueError(f"{source} does not have an SVG root element")
        destination = output_dir / source.name
        if source.resolve() != destination.resolve():
            shutil.copyfile(source, destination)
        print(f"Validated {destination}")


if __name__ == "__main__":
    main()
