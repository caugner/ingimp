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

class UserPrefsEditor:
    def __init__(self, window, prefs):
        self.prefs = prefs
        self._build_UI(window)
    def _build_UI(self, window):
        self.window = window
        window.protocol('WM_DELETE_WINDOW', self._handle_cancel)
        window.title("ingimp prefs editor")

        row_num = 0
        self.pref_fields = {}
        for pref_key in UserPrefs._default_user_prefs:
            label = tk.Label(window, text=pref_key)
            field = tk.Entry(window)
            field.insert(tk.END, self.prefs.get(pref_key))
            label.grid(row=row_num, column=0)
            field.grid(row=row_num, column=1, sticky=tk.W+tk.E)
            row_num = row_num + 1
            self.pref_fields[pref_key] = field
        ok_button = tk.Button(window, command=self._handle_ok, text="OK", padx=40)
        ok_button.grid(row=row_num, column=1)
        ok_button.focus_set()
        self.window.bind("<Return>", self._handle_ok)

    def show(self):
        Utils.center_window(self.window)
        self.window.focus_set()
        self.window.mainloop()
        self.window.destroy()
    def _handle_ok(self, *args):
        for pref_key in UserPrefs._default_user_prefs:
            self.prefs.set(pref_key, self.pref_fields[pref_key].get())
        self.window.withdraw()
        self.window.quit()
    def _handle_cancel(self):
        self.window.withdraw()
        self.window.quit()

if __name__ == '__main__':
    prefs = UserPrefs.UserPrefs()
    about = UserPrefsEditor(tk.Tk(), prefs)
    about.show()

