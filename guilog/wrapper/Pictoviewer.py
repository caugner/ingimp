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
import tkFont
import tkMessageBox
import Utils
import sys
import os
import time
import re

class Pictoviewer:
    def __init__(self, window, info_logo_filename, image_filenames):
        self.max_scroll_values = [0.0] * len(image_filenames)
        self.dwell_times = [0.0] * len(image_filenames)
        self.user_view_order = []
        self.start_time = 0
        self.min_canvas_width = 960
        self.min_canvas_height = 650
        self.cur_image = None
        self.cur_image_num = 0
        self.cur_image_id = None
        self.image_filenames = image_filenames
        self._build_UI(window, info_logo_filename)
    def _build_UI(self, window, info_logo_filename):
        self.window = window
        window.protocol('WM_DELETE_WINDOW', self._handle_close)
        window.title("ingimp Overview")        

        self.logo_image = tk.PhotoImage(file=info_logo_filename)

        if self.logo_image:
            logo = tk.Label(window, image=self.logo_image)
        else:
            logo = tk.Label(window, text="ingimp")

        picto_panel = tk.Frame(window)
        picto_canvas = tk.Canvas(picto_panel, width=self.min_canvas_width+100, height=self.min_canvas_height, bg='white', relief='sunken')
        self.picto_panel = picto_panel
        self.picto_canvas = picto_canvas

        ver_scrollbar = tk.Scrollbar(picto_panel)
        hor_scrollbar = tk.Scrollbar(picto_panel, orient=tk.HORIZONTAL)
        self.ver_scrollbar = ver_scrollbar
        self.hor_scrollbar = hor_scrollbar
        self.ver_scrollbar.config(command=self._handle_ver_scroll)
        self.hor_scrollbar.config(command=self._handle_hor_scroll)
        picto_canvas.config(yscrollcommand=ver_scrollbar.set, scrollregion=picto_canvas.bbox(tk.ALL))
        picto_canvas.config(xscrollcommand=hor_scrollbar.set, scrollregion=picto_canvas.bbox(tk.ALL))
        picto_canvas.grid(row=0, column=0, columnspan=3, rowspan=3, sticky='NSWE')
        ver_scrollbar.grid(row=0, column=3, rowspan=3, sticky='NSE')
        hor_scrollbar.grid(row=3, column=0, columnspan=3, sticky='SWE')
        picto_canvas.yview_moveto(0.0)
        picto_canvas.xview_moveto(0.0)

        next_button = tk.Button(window, command=self._handle_next, text="Next")
        next_button.config(padx=40)
        self.next_button = next_button
        prev_button = tk.Button(window, command=self._handle_prev, text="Previous")
        prev_button.config(padx=40)
        self.prev_button = prev_button

        logo.grid(row=0, column=0, columnspan=3, sticky='NWE', pady=15)
        picto_panel.grid(sticky='NSWE')
        picto_panel.grid(row=2, column=0, columnspan=3, sticky='NWSE')
        prev_button.grid(row=3, column=0, sticky=tk.W, padx=20, pady=20)
        next_button.grid(row=3, column=2, sticky=tk.E, padx=20, pady=20)

        # These options ensure that the picto_panel and its main contents, the canvas, resize appropriately.
        # These options are poorly documented and no doubt a cause for many hours of lost
        # productivity trying to figure out why the grid geometry manager does not properly resize its contents
        # when resizing a window.
        # </rant>
        picto_panel.grid_rowconfigure(0, weight=1) # Make picto_canvas resizable
        picto_panel.grid_columnconfigure(0, weight=1) # Make picto_canvas resizable
        window.grid_rowconfigure(2, weight=1) # Make picto_panel resizable
        window.grid_columnconfigure(0, weight=1) # Make picto_panel resizable

        self._show_image(0)
    def _load_image(self, image_num):
        del self.cur_image
        self.cur_image = tk.PhotoImage(file=self.image_filenames[image_num])
        if not self.cur_image:
            tkMessageBox.showerror("Installation Error", "Could not find picto graphics. Try reinstalling. Exiting.")
            sys.exit()
    def _show_image(self, image_num):
        if self.cur_image_id:
            self.picto_canvas.delete(tk.ALL)
            self.cur_image_id = None
            self._update_dwell_time()
        self.user_view_order.append(image_num)
        self.start_time = time.time()
        self._load_image(image_num)
        x = 0
        if self.cur_image.width() < self.cur_image.height():
            x = (self.picto_panel.winfo_width()- self.cur_image.width())/2
        # It would be absolutely fantastic if Canvas had some sort of predictable logic for where it actually places an image on its
        # canvas. Really. It would be *great*. But for now, the following code seems to be the only
        # way to approximate centering of the image. Why draw the line? Without it, the canvas won't actually center
        # the image. Why not? I have no idea. I defy you to tell me why I need to specify an anchor for the image and
        # why I can't simply draw it *exactly where I want it*. This is an example of why Tk enjoys a miserable existence
        # among other GUI toolkits and why Tk programmers are miserable themselves.
        # Wait. I'm getting ahead of myself. This code *only* works after the window has been resized. AND, it only works
        # then if the line has been drawn. Tkinter you are the bomb. You are Avril Lavigne to The Clash.
        self.cur_image_id = self.picto_canvas.create_image((x,0), image=self.cur_image, anchor="w")
        self.picto_canvas.create_line((0,0,0,0))
        self.cur_image_num = image_num
        if self.cur_image_num > 0:
            self.prev_button.config(state=tk.NORMAL)
        else:
            self.prev_button.config(state=tk.DISABLED)
        self.picto_canvas.config(scrollregion=self.picto_canvas.bbox(tk.ALL))
        self.picto_canvas.yview_moveto(0.0)
        self.picto_canvas.xview_moveto(0.0)
    def _update_dwell_time(self):
        now = time.time()
        self.dwell_times[self.cur_image_num] = self.dwell_times[self.cur_image_num] + (now - self.start_time)
    def _handle_ver_scroll(self, mode, *args):
        if mode == 'moveto':
            self.picto_canvas.yview(mode, args[0])
        else:
            self.picto_canvas.yview(mode, args[0], args[1])
        self.max_scroll_values[self.cur_image_num] = max(self.max_scroll_values[self.cur_image_num], self.ver_scrollbar.get()[1])
    def _handle_hor_scroll(self, mode, *args):
        if mode == 'moveto':
            self.picto_canvas.xview(mode, args[0])
        else:
            self.picto_canvas.xview(mode, args[0], args[1])
        self.max_scroll_values[self.cur_image_num] = max(self.max_scroll_values[self.cur_image_num], self.hor_scrollbar.get()[1])
    def _handle_next(self):
        if (self.cur_image_num + 1) >= len(self.image_filenames):
            self._handle_close()
        else:
            self._show_image(self.cur_image_num+1)
    def _handle_prev(self):
        if self.cur_image_num > 0:
            self._show_image(self.cur_image_num-1)
    def _handle_close(self):
        self._update_dwell_time()
        self.window.withdraw()
        self.window.quit()
    def show(self):
        self.window.state("normal")
        self.window.lift()
        Utils.center_window(self.window)
        self.window.mainloop()
    def destroy(self):
        self.window.destroy()

if __name__ == '__main__':
    picto_screen = Pictoviewer(tk.Tk(), r'c:\data\ingimp-2.2.17\guilog\wrapper\images\ingimp_info_logo.gif',
                                        [r'c:\data\ingimp-2.2.17\guilog\wrapper\images\pictogram-0.gif',
                                         r'c:\data\ingimp-2.2.17\guilog\wrapper\images\pictogram-1.gif',
                                         r'c:\data\ingimp-2.2.17\guilog\wrapper\images\pictogram-2.gif',
                                         r'c:\data\ingimp-2.2.17\guilog\wrapper\images\pictogram-3.gif',
                                         r'c:\data\ingimp-2.2.17\guilog\wrapper\images\pictogram-4.gif',])
    picto_screen.show()
    print picto_screen.dwell_times
    print picto_screen.max_scroll_values
    print picto_screen.user_view_order
