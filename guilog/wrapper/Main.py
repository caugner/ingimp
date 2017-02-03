#! /usr/bin/env python

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

import sys
import UserPrefs
from Pictoviewer import Pictoviewer
from FirstTimeHandler import FirstTimeHandler
import SplashScreen
import Config
import Launcher
import Tkinter as tk
import Participation
import tkMessageBox
import Uploader

class Main:
    def __init__(self):
        self.done = False
        self.result = False
        self.main()

    def main(self):
        self.root_win = tk.Tk()
        self.root_win.withdraw()
        try:
            #Get the default user and configuration
            prefs = UserPrefs.UserPrefs()
            #show the dialog box if this is the first time
            logging_enabled = False
            consent_refused = False
            startup_cancelled = False
            view_order = 0
            dwell_times = []
            max_scroll_values = []
            user_view_order = []
            if not prefs.have_killswitch:
                if not prefs.have_consent:
                    handler = FirstTimeHandler(Config.consent_filename, Config.info_logo_filename, Config.pictogram_filenames)
                    (got_consent, user_closed_window, view_order, dwell_times, max_scroll_values, user_view_order) = handler.get_consent()
                    if got_consent:
                        prefs.set_have_consent()
                    else:
                        consent_refused = True
                    if user_closed_window:
                        startup_cancelled = True
                elif not prefs.shown_pictograms:
                    picto_viewer = Pictoviewer(tk.Toplevel(), Config.info_logo_filename, Config.pictogram_filenames)
                    picto_viewer.show()
                    picto_viewer.destroy()
                    dwell_times = picto_viewer.dwell_times
                    max_scroll_values = picto_viewer.max_scroll_values
                    user_view_order = picto_viewer.user_view_order
            if prefs.have_consent and not startup_cancelled:
                splash = SplashScreen.SplashScreen(tk.Toplevel(), prefs)
                (startup_cancelled, logging_enabled) = splash.show()
            if not startup_cancelled:
                if consent_refused:
                    tkMessageBox.showwarning("Logging disabled", "You did not give your consent. Your usage data will NOT be reported until consent is given.")
                launcher = Launcher.Launcher(prefs, view_order, dwell_times, max_scroll_values, user_view_order, self._done_callback)
                launcher.spawn(logging_enabled)

                # Enter root window's mainloop to handle updates while the GIMP is running
                # If we don't do this, then things in Window can hang for a little while
                self.root_win.after(1000, self._done_check)
                self.root_win.mainloop()

                if self.result: # App launched and quit successfully
                    if logging_enabled:
                        print "Uploading collected data..."
                        uploader = Uploader.Uploader(prefs)
                        uploader.upload()
                        print "Finished uploading."

                    # Check for participation after quitting
                    if not prefs.have_killswitch:
                        participate = Participation.Participation(tk.Toplevel(), prefs)
                        participate.check_for_participation()
        except Exception, e:
            tkMessageBox.showerror("ingimp Error", "ingimp Error: " + str(e))
        self.root_win.destroy()
        sys.exit()
    def _done_callback(self, result):
        self.done = True
        self.result = result
    def _done_check(self):
        if not self.done:
            self.root_win.after(1000, self._done_check)
        else:
            self.root_win.quit()

if __name__ == '__main__':
    Main()
