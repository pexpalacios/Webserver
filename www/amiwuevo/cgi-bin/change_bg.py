#!/usr/bin/env python3
import os

print("Content-Type: text/plain\n")

#Get all backgrounds into an array
backgrounds = [
    "images/backgrounds/bg_01.png",
    "images/backgrounds/bg_02.png",
    "images/backgrounds/bg_03.png"
]

#Path to the database file
script_dir = os.path.dirname(os.path.abspath(__file__))
db_path = os.path.join(script_dir, "../../../database/background.txt")

#Read current background
try:
    with open(db_path, "r") as f:
        current = f.read().strip()
except FileNotFoundError:
    current = backgrounds[0]

#Find next background and write it in file
try:
    idx = backgrounds.index(current)
    next_bg = backgrounds[(idx + 1) % len(backgrounds)]
except ValueError:
    next_bg = backgrounds[0]

with open(db_path, "w") as f:
    f.write(next_bg)

print(next_bg)