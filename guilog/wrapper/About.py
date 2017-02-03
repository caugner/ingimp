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
import time
import Utils
import Config
import UserPrefs

class About:
    def __init__(self, window, prefs):
        self.prefs = prefs
        window.transient()
        self._build_UI(window)
    def _build_UI(self, window):
        self.window = window
        window.protocol('WM_DELETE_WINDOW', self._handle_ok)
        window.title("About: ingimp")
        window.resizable(width=0, height=0)

        self.logo_image = Utils.get_logo_image()
        if self.logo_image:
            logo = tk.Label(window, image=self.logo_image)
        else:
            logo = tk.Label(window, text="ingimp")

        version_label = tk.Label(window, text='Version: '+Config.version)
        user_label = tk.Label(window, text='ID: '+self.prefs.user_id)
        support_label = tk.Label(window, text='ingimp@cs.uwaterloo.ca')
        space1 = tk.Label(window, text=' ')
        ok_button = tk.Button(window, command=self._handle_ok, text="OK", padx=40)
        
        logo.grid(row=0, column=0, rowspan=2)
        version_label.grid(row=2, column=0)
        user_label.grid(row=3, column=0)
        support_label.grid(row=4, column=0)
        space1.grid(row=5, column=0)
        ok_button.grid(row=6, column=0)
        ok_button.focus_set()
        self.window.bind("<Return>", self._handle_ok)

    def show(self):
        Utils.center_window(self.window)
        self.window.focus_set()
        self.window.mainloop()
        self.window.destroy()
    def _handle_ok(self, *args):
        self.window.withdraw()
        self.window.quit()

if __name__ == '__main__':
    prefs = UserPrefs.UserPrefs()
    about = About(tk.Tk(), prefs)
    about.show()

