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
import About
import webbrowser
import urllib
import tkMessageBox

class SplashScreen:
    def __init__(self, window, prefs):
        self.autostart_cancelled = False
        self.logging_enabled = True
        self.startup_cancelled = True
        self.prefs = prefs
        self.autostart_delay_in_secs = int(self.prefs.get(UserPrefs.AUTOSTART_DELAY))
        self._build_UI(window)
    def _build_UI(self, window):
        self.window = window
        window.protocol('WM_DELETE_WINDOW', self._handle_close)
        window.title("ingimp")
        window.resizable(width=0, height=0)

        self.logo_image = Utils.get_logo_image()
        if self.logo_image:
            logo = tk.Label(window, image=self.logo_image)
#            logo = tk.Button(window, image=self.logo_image, command=self._handle_about)
        else:
            logo = tk.Button(window, text="ingimp", command=self._handle_about)

        space1 = tk.Label(window, text=' ')

        tag_frame = tk.Frame(window)
        tag_label = tk.Label(tag_frame, text="Activity tags (logged):")
        tag_entry = tk.Entry(tag_frame)
        tag_description = tk.Label(tag_frame, text="Example: I will be doing...")
        tag_example = tk.Label(tag_frame, text="image correction, graphic design, mashups, ...")
        tag_label.grid(row=0, column=0, sticky=tk.W)
        tag_entry.grid(row=0, column=1, sticky=tk.W+tk.E)
        tag_description.grid(row=1, column=0, sticky=tk.W)
        tag_example.grid(row=1, column=1)
        tag_entry.insert(tk.END, self.prefs.get(UserPrefs.SESSION_TAGS))
        self.tag_entry = tag_entry

        bottom_frame = tk.Frame(window)
        try:
            self.webstats_image = tk.PhotoImage(file=Config.webstats_button_filename)
            webstats_button = tk.Button(bottom_frame, image=self.webstats_image, text="website & stats", command=self._handle_webstats)
        except:
            webstats_button = tk.Button(bottom_frame, text="website & stats", command=self._handle_webstats)

        try:
            self.launch_image = tk.PhotoImage(file=Config.launch_button_filename)
            start_button = tk.Button(bottom_frame, image=self.launch_image, text="Start GIMP Now", command=self._handle_start_now)
        except:
            start_button = tk.Button(bottom_frame, text="Start GIMP Now", command=self._handle_start_now)

        divider_frame = tk.Frame(window)
        try:
            self.divider_line_image = tk.PhotoImage(file=Config.divider_line_filename)
            divider_line = tk.Label(divider_frame, image=self.divider_line_image)
        except Exception, e:
            divider_line = tk.Label(divider_frame, text=' ')
        divider_line.pack()

        webstats_button.pack(side=tk.LEFT, padx=5, pady=5)
        start_button.pack(side=tk.RIGHT, padx=5, pady=5)

        version_label = tk.Label(window, text='Version: '+Config.version)
        user_label = tk.Label(window, text='User: '+self.prefs.user_id)
        support_label = tk.Label(window, text='ingimp@cs.uwaterloo.ca')

        self.logging_disabled_var = tk.IntVar()
        logging_checkbox = tk.Checkbutton(window, variable=self.logging_disabled_var, command=self._stop_autostart, text="Disable application usage logging for this run", padx=10, pady=10)
        logging_checkbox.deselect()
        self.autostart_label = tk.Label(window, text="Automatically starting in %d seconds" % self.autostart_delay_in_secs, foreground='Black', pady=15)
        
        logo.grid(row=0, column=0, rowspan=2)
        version_label.grid(row=2, column=0, pady=10)
        space1.grid(row=3, column=0)
        tag_frame.grid(row=4, column=0, padx=15, pady=10, sticky=tk.W+tk.E)
        bottom_frame.grid(row=5, column=0, padx=15, sticky=tk.W+tk.E)
        logging_checkbox.grid(row=6, column=0, sticky=tk.W)

        tag_entry.focus_set()

        tag_entry.bind("<Return>", self._handle_start_now)
        self.window.bind("<Return>", self._handle_start_now)

        if self.autostart_delay_in_secs < 0:
            self._stop_autostart()

    def show(self):
        self.window.after(200, self._update_autostart_label)
        self.start_time = time.time()
        Utils.center_window(self.window)
        self.window.mainloop()
        self.window.destroy()
        self.autostart_cancelled = True
        self.logging_enabled = (self.logging_disabled_var.get() == 0)
        if not self.logging_enabled:
            cur_num = int(self.prefs.get(UserPrefs.NUM_DISABLED_RUNS))
            self.prefs.set(UserPrefs.NUM_DISABLED_RUNS, str(cur_num+1))
        return (self.startup_cancelled, self.logging_enabled)
    def _handle_close(self):
        self.window.withdraw()
        self.window.quit()
    def _handle_start_now(self, *args):
        self.startup_cancelled = False
        self.autostart_cancelled = True
        self.window.withdraw()
        self.prefs.set(UserPrefs.SESSION_TAGS, self.tag_entry.get())
        self.window.quit()
    def _handle_about(self):
        self._stop_autostart()
        self.window.withdraw()
        about = About.About(tk.Toplevel(self.window), self.prefs)
        about.show()
        self.window.deiconify()
    def _handle_webstats(self):
        self._stop_autostart()
        webstats_url = self.prefs.get(UserPrefs.WEBSTATS_REDIRECT_URL)
        webstats_url = Utils.get_redirect_url(webstats_url)
        if webstats_url:
            webstats_url = webstats_url + '?' + urllib.urlencode([('id', self.prefs.user_id), ('version', Config.version)])
            try:
                webbrowser.open_new(webstats_url)
            except Exception, e:
                tkMessageBox.showerror("Web Error", "Could not open webstats url: "+str(e))
    def _stop_autostart(self):
        self.autostart_cancelled = True
        self.autostart_label.config(text="")
    def _update_autostart_label(self):
        if not self.autostart_cancelled:
            new_time = self.autostart_delay_in_secs - int(time.time() - self.start_time)
            if new_time <= 0:
                self._handle_start_now()
            else:
                self.autostart_label.config(text="Automatically starting in %d seconds" % new_time)
                self.window.after(200, self._update_autostart_label)

if __name__ == '__main__':
    prefs = UserPrefs.UserPrefs()
    splash_screen = SplashScreen(tk.Tk(), prefs)
    splash_screen.show()
    print "Logging enabled:", splash_screen.logging_enabled, "Startup cancelled:", splash_screen.startup_cancelled
