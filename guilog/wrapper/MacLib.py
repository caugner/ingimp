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

'''
This is a helper app for ingimp. Currently, it will give mac specific
information, such as if X11 exists, if were on a mac system, and
the size of the dock.
'''

import sys
import os
import pwd
import commands
import webbrowser 

import Tkinter as tk
import MacX11Screen
import UserPrefs

def x11_fail_display():
	macx11screen = MacX11Screen.create_screen()
	macx11screen.show_dialog()

def is_mac():
	if sys.platform == 'darwin':
			return True
	return False

def x11_installed():
	return os.path.isdir('/Applications/Utilities/X11.app')

def dock_size():
	dock_size = 60
	
	# Get the users home directory and location of dock config file
	# To my knowledge there is no other place this would be
	user_data = pwd.getpwuid(os.getuid())
	user_dock_pref_path = user_data[5] + '/Library/Preferences/com.apple.dock.plist'

	# Possible commands (see below for further explanation)
	binary_command = 'plutil -convert xml1 ' + user_dock_pref_path + ' -o /dev/stdout'
	reg_command = 'cat ' + user_dock_pref_path
	extract_pipe = ' | grep -A 1 tilesize | egrep -o "[0-9]+"'
	command = reg_command + extract_pipe	

	# Check if the dock file exists, if not assume default value
	if not(os.path.isfile(user_dock_pref_path)): return dock_size
	
	# Check to see if plist file is binary format
	# All plist files are text, but as text files are large
	# Apple, as of Mac OS X 10.2 introduced binary formatted version
  # As of 10.4 this is the default plist type for many applications, 
  # but not all
	if is_binary(user_dock_pref_path): 
		command = binary_command + extract_pipe

	ret_dock_data = commands.getstatusoutput(command)
	
	if ret_dock_data[0] == 0:
		dock_size = ret_dock_data[1]
	return dock_size

def is_binary(path):
	check_command = 'cat ' + path + ' | head -n 1 | grep -o "^bplist"'
	ret_data = commands.getstatusoutput(check_command)
	if ret_data[0] == 0:
		return True
	return False
