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
import webbrowser
import tkMessageBox
import UserPrefs

def create_screen():
	macx11screen = MacX11Screen(tk.Tk(), UserPrefs.UserPrefs())
	return macx11screen

class MacX11Screen:
	def __init__(self, window, prefs):
		self.window = window
		self.prefs = prefs
		self.x11_support_url = 'http://www.apple.com/support/downloads/x11formacosx.html'
		self.message_text = "Sorry, it appears that X11 is not properly installed"
		self.find_text = "You can find a copy of X11 on your Installation DVDs"
		self.help_text = "You can also go to the Apple X11 Support Site"

	def _build_UI(self):
		self.window.protocol('WM_DELETE_WINDOW', self._handle_quit)
		self.window.title("ingimp")
		self.window.resizable(width=0, height=0)
		
		message_label = tk.Label(self.window, text=self.message_text)
		message_label.pack()
	
		find_label = tk.Label(self.window, text=self.find_text)
		find_label.pack()
	
		help_label = tk.Label(self.window, text=self.help_text)
		help_label.pack()
		
		box = tk.Frame(self.window)
		web_button = tk.Button(box, text="Go to X11 Apple Support Page", width=30, command=self._handle_web, default=tk.ACTIVE)
		web_button.pack()
		
		quit_button = tk.Button(box, text="Quit", width=10, command=self._handle_quit)
		quit_button.pack()
		
		box.pack()
		self.window.bind("<Return>", self._handle_web)
		self.window.bind("<Escape>", self._handle_quit)
	def _handle_web(self, *args):
		webbrowser.open_new(self.x11_support_url)
		self._handle_quit()
	def _handle_quit(self, *args):
		self.window.quit()
	def _show_dialog(self):
		self._build_UI()
		Utils.center_window(self.window)
		self.window.mainloop()
		self.window.destroy()
	def show_dialog(self):
		self._show_dialog()
if __name__ == '__main__':
	macx11screen = MacX11Screen(tk.Tk(), UserPrefs.UserPrefs())
	macx11screen.show_dialog()