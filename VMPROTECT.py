#!/usr/bin/python3

import Editor.main as editor

from tkinter import *
from tkinter import messagebox
from tkinter import filedialog

print("""\
 _   ____  _______________ _____ _____ _____ _____ _____ 
| | | |  \/  || ___ \ ___ \  _  |_   _|  ___/  __ \_   _|
| | | | .  . || |_/ / |_/ / | | | | | | |__ | /  \/ | |  
| | | | |\/| ||  __/|    /| | | | | | |  __|| |     | |  
\ \_/ / |  | || |   | |\ \\ \_/ / | | | |___| \__/\ | |  
 \___/\_|  |_/\_|   \_| \_|\___/  \_/ \____/ \____/ \_/  
""")
print("version 0.2.200926.1921")

# Creating TK Container
root = Tk()
# Passing Root to Editor Class
editor.Editor(root)
# Root Window Looping
root.mainloop()