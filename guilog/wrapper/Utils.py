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

import urllib2
import Tkinter as tk
import Config
import sys

_logo_image = None

def center_window(window):
    window.update_idletasks()
    screenwidth  = window.winfo_screenwidth()
    screenheight = window.winfo_screenheight()
    width  = window.winfo_reqwidth()
    height = window.winfo_reqheight()
    too_small = False
    if width > screenwidth or height > screenheight:
        width = min(width, screenwidth) - 30
        height = min(height, screenheight) - 30
        too_small = True
        # TODO: Quick hack until more robust code available
        if sys.platform.lower().startswith('darwin'):
            height = height - 60
    x = (screenwidth-width)/2
    y = (screenheight-height)/3
    if too_small:
        y = 0
    if sys.platform.lower().startswith('darwin'):
        # Put it below the menubar
        if y < 30:
            height = height - (30 - y)
            y = 30
    window.geometry('%dx%d+%d+%d' % (width, height, x, y))

def get_logo_image():
    global _logo_image
    if not _logo_image:
        try:
            _logo_image = tk.PhotoImage(file=Config.logo_filename)
        except:
            _logo_image = None
    return _logo_image

def get_redirect_url(url):
    try:
        f = urllib2.urlopen(url)
        new_url = f.read()
        f.close()
        return new_url.strip()
    except Exception, e:
        return None

