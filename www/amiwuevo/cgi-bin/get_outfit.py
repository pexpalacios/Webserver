#!/usr/bin/env python3
import os

#Gets the file where the .txt is and gets teh content (the .png)
script_dir = os.path.dirname(os.path.abspath(__file__))
db_path = os.path.join(script_dir, "../../../database/outfit.txt")

default_outfit = 'base'

try:
    if not os.path.exists(db_path):
        print(default_outfit)
    else:
        with open(db_path) as f:
            outfit = f.read().strip()
            if not outfit:
                print(default_outfit)
            else:
                print(outfit)
except Exception:
    print(default_outfit)
