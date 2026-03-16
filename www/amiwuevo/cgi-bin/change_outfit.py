#!/usr/bin/env python3
import os

print("Content-Type: text/plain\n")

#Get all outfits into an array
outfits = [
    "base",
    "penguin",
    "shark",
	"ray"
]

#Path to the database file
script_dir = os.path.dirname(os.path.abspath(__file__))
db_path = os.path.join(script_dir, "../../../database/outfit.txt")

#Read current background
try:
    with open(db_path, "r") as f:
        current = f.read().strip()
except FileNotFoundError:
    current = outfits[0]

#Find next background and write it in file
try:
    idx = outfits.index(current)
    next_fit = outfits[(idx + 1) % len(outfits)]
except ValueError:
    next_fit = outfits[0]

with open(db_path, "w") as f:
    f.write(next_fit)

print(next_fit)