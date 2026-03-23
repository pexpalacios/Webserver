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


#Read current outfit
try:
    with open(db_path, "r") as f:
        current = f.read().strip()
except FileNotFoundError:
    current = outfits[0]

#Find next outfit and write it in file
try:
    idx = outfits.index(current)
    next_fit = outfits[(idx + 1) % len(outfits)]
except ValueError:
    next_fit = outfits[0]

try:
	with open(db_path, "w") as f:
    	f.write(next_fit)
except PermissionError:
	next_fit = outfits[0]


print(next_fit)

##check file permissions