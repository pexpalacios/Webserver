#!/usr/bin/env python3
import os

print("Content-Type: text/plain\n")

#Gets the file where the .txt is and gets teh content (the .png)
script_dir = os.path.dirname(os.path.abspath(__file__))
db_path = os.path.join(script_dir, "../../../database/background.txt")

with open(db_path) as f:
    print(f.read().strip())