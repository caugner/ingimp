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

import os
import urllib
import urllib2
import Tkinter as tk
import tkMessageBox
import webbrowser
import Config
import Utils
import UserPrefs

class Participation:
    def __init__(self, window, prefs):
        self.window = window
        self.prefs = prefs
    def _build_UI(self):
        self.window.protocol('WM_DELETE_WINDOW', self._handle_cancel)
        self.window.title("ingimp")
        self.window.resizable(width=0, height=0)
        message_label = tk.Label(self.window, text=self.message_text)
        message_label.pack(padx=5, pady=5)

        box = tk.Frame(self.window)
        cancel_button = tk.Button(box, text="No", width=10, command=self._handle_cancel)
        cancel_button.pack(side=tk.LEFT, padx=5, pady=5)
        ok_button = tk.Button(box, text="Yes", width=10, command=self._handle_ok, default=tk.ACTIVE)
        ok_button.pack(side=tk.LEFT, padx=5, pady=5)
        box.pack()

        self.window.bind("<Return>", self._handle_ok)
        self.window.bind("<Escape>", self._handle_cancel)
    def _handle_ok(self, *args):
        webbrowser.open_new(self.interaction_url)
        self._handle_cancel()
    def _handle_cancel(self, *args):
        self.window.withdraw()
        self.window.quit()
    def _show_dialog(self):
        self._build_UI()
        Utils.center_window(self.window)
        self.window.mainloop()
        self.window.destroy()
    def check_for_participation(self):
        participation_url = self.prefs.get(UserPrefs.PARTICIPATION_URL)
        if participation_url:
            url_to_open = Utils.get_redirect_url(participation_url)
            if url_to_open:
                url_to_open = url_to_open + '?' + urllib.urlencode([('id', self.prefs.user_id), ('version', Config.version)])
                try:
                    f = urllib2.urlopen(url_to_open)
                    proceed = f.readline().strip()
                    if (proceed == 'KILL'):
                        self.prefs.set_have_killswitch()
                        message_text = f.readline().strip()
                        tkMessageBox.showinfo("End of Study", message_text)
                    else:
                        proceed = (proceed == 'YES')
                        if proceed:
                            self.message_text = f.readline().strip()
                            self.interaction_url = f.readline().strip()
                        f.close()
                        if proceed:
                            if self.message_text and self.interaction_url:
                                self._show_dialog()
                except Exception, e:
                    pass

if __name__ == '__main__':
    participate = Participation(tk.Tk(), UserPrefs.UserPrefs())
    participate.check_for_participation()
