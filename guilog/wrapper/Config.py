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

'''
Configuration data for ingimp. See the prefs file in ~/.ingimp for
user-customizable stuff.
'''
import sys

version = '2008.04.16'

wrapper_dir = None
bin_dir = None
logo_filename = None
launch_button_filename = None
webstats_button_filename = None
divider_line_filename = None
consent_filename = None
info_logo_filename = None
pictogram_filenames = []
gimp_bin = 'ingimp-2.4'
if sys.platform == 'win32':
    gimp_bin = gimp_bin + '.exe'
