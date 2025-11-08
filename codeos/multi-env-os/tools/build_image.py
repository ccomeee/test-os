#!/usr/bin/env python3
"""Build OS disk image with manifest-based layout."""

import argparse
import math
import os
import struct
import sys
from typing import List, Tuple

SECTOR_SIZE = 512
MANIFEST_HEADER_SIZE = 12
MANIFEST_ENTRY_SIZE = 32
MANIFEST_SIGNATURE = b"MFS1"

def align_up(value: int, alignment: int) -> int:
    return (value + alignment - 1) // alignment * alignment

def read_file(path: str) -> bytes:
    with open(path, "rb") as f:
        return f.read()

def pad_to(data: bytes, size: int) -> bytes:
    if len(data) > size:
        raise ValueError("Component larger than reserved size")
    return data + b"\x00" * (size - len(data))

def write_file(path: str, data: bytes) -> None:
    directory = os.path.dirname(path)
    if directory:
        os.makedirs(directory, exist_ok=True)
    with open(path, "wb") as f:
        f.write(data)

def build_manifest(entries: List[Tuple[bytes, int, int, int, int]]) -> bytes:
    """entries: list of (name, lba_start, length_bytes, dest_linear, flags)."""
    header = struct.pack("<4sHHI", MANIFEST_SIGNATURE, len(entries), MANIFEST_ENTRY_SIZE, 0)
    payload = bytearray(header)
    for name, lba_start, length_bytes, dest_linear, flags in entries:
        if len(name) > 8:
            raise ValueError(f"Manifest name {name!r} exceeds 8 bytes")
        name_field = name.ljust(8, b"\x00")
        entry = struct.pack(
            "<8sIIIIII",
            name_field,
            lba_start,
            length_bytes,
            dest_linear,
            flags,
            0,
            0,
        )
        if len(entry) != MANIFEST_ENTRY_SIZE:
            raise AssertionError("Manifest entry size mismatch")
        payload.extend(entry)
    return pad_to(payload, SECTOR_SIZE)

def main(argv: List[str]) -> int:
    parser = argparse.ArgumentParser(description="Build OS image with manifest")
    parser.add_argument("--boot", required=True)
    parser.add_argument("--stage2", required=True)
    parser.add_argument("--kernel", required=True)
    parser.add_argument("--wallpaper", required=True)
    parser.add_argument("--manifest", required=True)
    parser.add_argument("--output", required=True)
    args = parser.parse_args(argv)

    boot = read_file(args.boot)
    stage2 = read_file(args.stage2)
    kernel = read_file(args.kernel)
    wallpaper = read_file(args.wallpaper)

    if len(boot) != SECTOR_SIZE:
        raise ValueError("Boot sector must be exactly 512 bytes")

    stage2_size_aligned = align_up(len(stage2), SECTOR_SIZE)
    manifest_size = SECTOR_SIZE  # reserve one sector
    kernel_size_aligned = align_up(len(kernel), SECTOR_SIZE)
    wallpaper_size_aligned = align_up(len(wallpaper), SECTOR_SIZE)

    boot_lba = 0
    stage2_lba = boot_lba + 1
    stage2_sectors = stage2_size_aligned // SECTOR_SIZE
    manifest_lba = stage2_lba + stage2_sectors
    manifest_sectors = manifest_size // SECTOR_SIZE
    kernel_lba = manifest_lba + manifest_sectors
    kernel_sectors = kernel_size_aligned // SECTOR_SIZE
    wallpaper_lba = kernel_lba + kernel_sectors

    # Build manifest entries
    manifest_entries = [
        (b"KERNEL", kernel_lba, len(kernel), 0x00010000, 0),
        (b"WALLPAPR", wallpaper_lba, len(wallpaper), 0x00070000, 0),
    ]
    manifest = build_manifest(manifest_entries)

    image = bytearray()
    image.extend(boot)
    image.extend(pad_to(stage2, stage2_size_aligned))
    image.extend(manifest)
    image.extend(pad_to(kernel, kernel_size_aligned))
    image.extend(pad_to(wallpaper, wallpaper_size_aligned))

    write_file(args.manifest, manifest)
    write_file(args.output, image)

    # Diagnostics
    print(f"Boot LBA: {boot_lba}")
    print(f"Stage2 LBA: {stage2_lba} (sectors={stage2_sectors})")
    print(f"Manifest LBA: {manifest_lba}")
    print(f"Kernel LBA: {kernel_lba}, size={len(kernel)} bytes")
    print(f"Wallpaper LBA: {wallpaper_lba}, size={len(wallpaper)} bytes")
    return 0

if __name__ == "__main__":
    try:
        raise SystemExit(main(sys.argv[1:]))
    except Exception as exc:
        print("[build_image] 發生錯誤:", exc, file=sys.stderr)
        import traceback
        traceback.print_exc()
        raise
