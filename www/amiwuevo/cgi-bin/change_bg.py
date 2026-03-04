#!/usr/bin/env python3
import os
import json

# List of background images
bg_images = ["images/bg_01.png", "images/bg_02.png", "images/bg_03.png"]

# Get current from query string or default to first
import cgi
form = cgi.FieldStorage()
current = form.getvalue("current", bg_images[0])
try:
    idx = bg_images.index(current)
    next_bg = bg_images[(idx + 1) % len(bg_images)]
except ValueError:
    next_bg = bg_images[0]

print("Content-Type: application/json\n")
print(json.dumps({"next_bg": next_bg}))