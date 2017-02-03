'''
ingimp -- Instrumented GIMP
Copyright (C) 2007 Michael Terry and Terry Park

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
'''

import Tkinter as tk
import Utils
import Config
import UserPrefs
import os
import sys
sys.path.append('../website/parsing')
from guilog_parser import InteractionLog

class FlatFileMaker:
    def __init__(self, window, prefs):
        self.prefs = prefs
        self._build_UI(window)
    def _build_UI(self, window):
        self.window = window
        window.protocol('WM_DELETE_WINDOW', self._handle_cancel)
        window.title("ingimp flat file maker")

        label = tk.Label(window, text="Flattens log files and places them in the chosen directory")
#        label.grid(row=0, column=0, sticky=tk.W)
        label.pack()
        frame = tk.Frame(window)
        label = tk.Label(frame, text="Output directory:")
        label.grid(row=1, column=0, sticky=tk.W)
        self.entry = tk.Entry(frame)
        self.entry.insert(tk.END, self.prefs.get(UserPrefs.LOG_DIR))
        self.entry.grid(row=1, column=1, sticky=tk.E)
        frame.pack()

        frame = tk.Frame(window)
        cancel_button = tk.Button(frame, command=self._handle_cancel, text="Cancel", padx=40)
        cancel_button.grid(row=2, column=0, sticky=tk.W)
        ok_button = tk.Button(frame, command=self._handle_ok, text="OK", padx=40)
        ok_button.grid(row=2, column=1, sticky=tk.E)
        ok_button.focus_set()
        frame.pack()
        self.window.bind("<Return>", self._handle_ok)

    def show(self):
        Utils.center_window(self.window)
        self.window.focus_set()
        self.window.mainloop()
        self.window.destroy()
    def _handle_ok(self, *args):
        self.window.withdraw()
        self.window.quit()
        log_dir = self.prefs.get(UserPrefs.LOG_DIR)
        log_files = os.listdir(log_dir)
        log_files = filter(lambda x: x.startswith(self.prefs.get(UserPrefs.UPLOADED_PREFIX)), log_files)
        log_files = filter(lambda x: x.endswith('.xml'), log_files)
        output_dir = self.entry.get()
        if log_files:
            for f in log_files:
                f = os.path.join(log_dir, f)
                new_file = os.path.basename(f)[:-4] + '.txt'
                new_file = os.path.join(self.entry.get(), new_file)
                new_file = open(new_file, 'w')
                log = InteractionLog(f)
                new_file.write(log.flatten())
                new_file.close()
    def _handle_cancel(self):
        self.window.withdraw()
        self.window.quit()

if __name__ == '__main__':
    prefs = UserPrefs.UserPrefs()
    about = FlatFileMaker(tk.Tk(), prefs)
    about.show()

