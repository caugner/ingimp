'''
ingimp -- Instrumented GIMP
Copyright (C) 2007 Michael Terry

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
import tkFont
import tkMessageBox
import Utils
import sys
import random

from ConsentDialog import ConsentDialog
from Pictoviewer import Pictoviewer

VIEW_ORDER_PICTO_CONSENT = 1
VIEW_ORDER_CONSENT_PICTO = 2
VIEW_ORDER_CONSENT = 3

class FirstTimeHandler:
    def __init__(self, consent_filename, info_logo_filename, image_filenames):
        self.consent_filename = consent_filename
        self.info_logo_filename = info_logo_filename
        self.image_filenames = image_filenames
        self.view_order = random.randint(1,3)
#        self.view_order = 1
    def get_consent(self):
        w1 = tk.Toplevel()
        w1.withdraw()
        w2 = tk.Toplevel()
        w2.withdraw()
        consent_dialog = ConsentDialog(w1, self.consent_filename, (self.view_order == VIEW_ORDER_PICTO_CONSENT))
        pictoviewer = Pictoviewer(w2, self.info_logo_filename, self.image_filenames)
        if self.view_order == VIEW_ORDER_PICTO_CONSENT:
            done = False
            while not done:
                pictoviewer.show()
                (consent_granted, consent_max_scroll_value, consent_dwell_time) = consent_dialog.show()
                if consent_granted or consent_dialog.user_closed_window:
                    done = True
        else:
            (consent_granted, consent_max_scroll_value, consent_dwell_time) = consent_dialog.show()
            if consent_granted and self.view_order == VIEW_ORDER_CONSENT_PICTO:
                pictoviewer.show()
        pictoviewer.destroy()
        consent_dialog.destroy()
        return (consent_granted, consent_dialog.user_closed_window, self.view_order, [consent_dwell_time] + pictoviewer.dwell_times, [consent_max_scroll_value] + pictoviewer.max_scroll_values, pictoviewer.user_view_order)

if __name__ == '__main__':
    # No need to use automake on these; these are only for testing
    consent_file = r'c:\ingimp-2.2.17\share\ingimp\consent.txt'
    image_files = [r'c:\data\ingimp-2.2.17\guilog\wrapper\images\picto_image.gif',
                     r'c:\data\ingimp-2.2.17\guilog\wrapper\images\picto_image-2.gif',
                     r'c:\data\ingimp-2.2.17\guilog\wrapper\images\picto_image-3.gif',]
    handler = FirstTimeHandler(consent_file, image_files)
    print handler.get_consent()

