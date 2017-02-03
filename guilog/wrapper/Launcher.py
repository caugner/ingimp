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
import Config
import UserPrefs
import time
import tkMessageBox
import sys
import platform
from threading import Thread

GIMP_GUI_LOG_FILE = 'GIMP_GUI_LOG_FILE'
GIMP_GUI_LOG_FILE_NAME = 'GIMP_GUI_LOG_FILE_NAME' # Is uploaded to uniquely ID log
GIMP_GUI_LOG_USER_ID = 'GIMP_GUI_LOG_USER_ID'
GIMP_GUI_LOG_TIMEZONE = 'GIMP_GUI_LOG_TIMEZONE'
GIMP_GUI_LOG_HEADER_FILE = 'GIMP_GUI_LOG_HEADER_FILE'
GIMP_GUI_LOG_WRAPPER_VERSION = 'GIMP_GUI_LOG_WRAPPER_VERSION'
GIMP_GUI_LOG_PLATFORM = 'GIMP_GUI_LOG_PLATFORM'
GIMP_GUI_LOG_PLATFORM_SYSTEM = 'GIMP_GUI_LOG_PLATFORM_SYSTEM'
GIMP_GUI_LOG_PLATFORM_RELEASE = 'GIMP_GUI_LOG_PLATFORM_RELEASE'
GIMP_GUI_LOG_PLATFORM_VERSION = 'GIMP_GUI_LOG_PLATFORM_VERSION'
GIMP_GUI_LOG_PLATFORM_MACHINE = 'GIMP_GUI_LOG_PLATFORM_MACHINE'
GIMP_GUI_LOG_PLATFORM_PROCESSOR = 'GIMP_GUI_LOG_PLATFORM_PROCESSOR'
GIMP_GUI_LOG_COMPRESS_LOG = 'GIMP_GUI_LOG_COMPRESS_LOG'
GIMP_GUI_LOG_HELPER = 'GIMP_GUI_LOG_HELPER'
GIMP_GUI_LOG_TEST_URL_PREFIX = 'GIMP_GUI_LOG_TEST_URL_PREFIX'
GIMP_GUI_LOG_LOG_KEY_ACTIVITY = 'GIMP_GUI_LOG_LOG_KEY_ACTIVITY'
GIMP_GUI_LOG_LOG_BUTTON_ACTIVITY = 'GIMP_GUI_LOG_LOG_BUTTON_ACTIVITY'
GIMP_GUI_LOG_SESSION_TAGS = 'GIMP_GUI_LOG_SESSION_TAGS'
GIMP_GUI_LOG_HISTOGRAM_TIMEOUT = 'GIMP_GUI_LOG_HISTOGRAM_TIMEOUT'
GIMP_GUI_LOG_NUM_DISABLED_RUNS = 'GIMP_GUI_LOG_NUM_DISABLED_RUNS'
GIMP_GUI_LOG_CONSENT_VIEW_ORDER = 'GIMP_GUI_LOG_CONSENT_VIEW_ORDER'
GIMP_GUI_LOG_CONSENT_DWELL_TIMES = 'GIMP_GUI_LOG_CONSENT_DWELL_TIMES'
GIMP_GUI_LOG_CONSENT_MAX_SCROLL_VALUES = 'GIMP_GUI_LOG_CONSENT_MAX_SCROLL_VALUES'
GIMP_GUI_LOG_CONSENT_USER_VIEW_ORDER = 'GIMP_GUI_LOG_CONSENT_USER_VIEW_ORDER'
GIMP_GUI_LOG_ONE_TIME_PAD_FILE = 'GIMP_GUI_LOG_ONE_TIME_PAD_FILE'


class Launcher(Thread):
    def __init__(self, prefs, view_order, dwell_times, max_scroll_values, user_view_order, done_callback):
        Thread.__init__(self)
        self.prefs = prefs
        self.view_order = view_order
        self.dwell_times = dwell_times
        self.max_scroll_values = max_scroll_values
        self.user_view_order = user_view_order
        self._done_callback = done_callback

    def spawn(self, logging_enabled):
        self.logging_enabled = logging_enabled
        # Always set the helper app or else File->Open Location will fail
        os.environ[GIMP_GUI_LOG_HELPER] = Config.ingimp_helper
        if self.logging_enabled and not self.prefs.have_killswitch:
            log_dir = self.prefs.get(UserPrefs.LOG_DIR)
            logfile_prefix = self.prefs.get(UserPrefs.LOGFILE_PREFIX)
            log_header_file = self.prefs.get(UserPrefs.LOG_HEADER_FILE)
            if not os.path.exists(log_dir):
                try:
                    os.mkdir(log_dir)
                except:
                    tkMessageBox.showerror("Log Error", "Could not create log directory at " + log_dir + ".")
            date_str = time.strftime( '%Y%m%d_%H%M%S' )
            log_file_name = self.prefs.user_id + '_' + date_str
            log_file = os.path.join(log_dir, logfile_prefix + log_file_name + '.xml')
            if self.prefs.get(UserPrefs.COMPRESS_LOG) == '1':
                log_file = log_file + '.gz'
            # Set environment variables to pass to the GIMP executable
            os.environ[GIMP_GUI_LOG_FILE] = os.path.abspath(log_file)
            os.environ[GIMP_GUI_LOG_FILE_NAME] = log_file_name
            os.environ[GIMP_GUI_LOG_USER_ID] = str(self.prefs.user_id)
            os.environ[GIMP_GUI_LOG_TIMEZONE] = str(time.timezone)
            os.environ[GIMP_GUI_LOG_WRAPPER_VERSION] = str(Config.version)
            os.environ[GIMP_GUI_LOG_PLATFORM] = sys.platform
            os.environ[GIMP_GUI_LOG_PLATFORM_SYSTEM] = platform.system()
            os.environ[GIMP_GUI_LOG_PLATFORM_RELEASE] = platform.release()
            os.environ[GIMP_GUI_LOG_PLATFORM_VERSION] = platform.version()
            os.environ[GIMP_GUI_LOG_PLATFORM_MACHINE] = platform.machine()
            os.environ[GIMP_GUI_LOG_PLATFORM_PROCESSOR] = platform.processor()
            os.environ[GIMP_GUI_LOG_TEST_URL_PREFIX] = self.prefs.get(UserPrefs.TEST_URL_PREFIX)
            os.environ[GIMP_GUI_LOG_COMPRESS_LOG] = self.prefs.get(UserPrefs.COMPRESS_LOG)
            os.environ[GIMP_GUI_LOG_LOG_KEY_ACTIVITY] = self.prefs.get(UserPrefs.LOG_KEY_ACTIVITY)
            os.environ[GIMP_GUI_LOG_LOG_BUTTON_ACTIVITY] = self.prefs.get(UserPrefs.LOG_BUTTON_ACTIVITY)
            os.environ[GIMP_GUI_LOG_SESSION_TAGS] = self.prefs.get(UserPrefs.SESSION_TAGS)
            os.environ[GIMP_GUI_LOG_HISTOGRAM_TIMEOUT] = self.prefs.get(UserPrefs.HISTOGRAM_TIMEOUT)
            os.environ[GIMP_GUI_LOG_NUM_DISABLED_RUNS] = self.prefs.get(UserPrefs.NUM_DISABLED_RUNS)
            os.environ[GIMP_GUI_LOG_CONSENT_VIEW_ORDER] = str(self.view_order)
            os.environ[GIMP_GUI_LOG_CONSENT_DWELL_TIMES] = '|'.join(map(str, self.dwell_times))
            os.environ[GIMP_GUI_LOG_CONSENT_MAX_SCROLL_VALUES] = '|'.join(map(str, self.max_scroll_values))
            os.environ[GIMP_GUI_LOG_CONSENT_USER_VIEW_ORDER] = '|'.join(map(str, self.user_view_order))
            os.environ[GIMP_GUI_LOG_ONE_TIME_PAD_FILE] = self.prefs.get_one_time_pad_file()
            if log_header_file:
                os.environ[GIMP_GUI_LOG_HEADER_FILE] = os.path.abspath(log_header_file)

        # Now spawn the thread to start us up
        self.start()

    def run(self):
        result = False
        try:
            # We need to set the PATH so dll's can be found in Windows (and potentialy Unix)
            # We used to cd into the bin directory and run from there, but that screws up
            # the ability to pass in file names on the command line
            old_path = os.environ["PATH"]
            os.environ["PATH"] = Config.bin_dir + os.path.pathsep + old_path
            gimp_bin = os.path.join(Config.bin_dir, Config.gimp_bin)
            if sys.platform == 'win32':
                self.__pid = os.spawnv(os.P_WAIT, gimp_bin, sys.argv[1:])
            else:
                self.__pid = os.spawnv(os.P_WAIT, gimp_bin, [gimp_bin] + sys.argv[1:])
        except Exception, e:
            tkMessageBox.showerror("Launch Error", "Could not launch the GIMP.")            
        else:
            result = True
        os.environ["PATH"] = old_path
        self._clean_environ()
        self._done_callback(result)

    def _clean_environ(self):
        for var in [GIMP_GUI_LOG_FILE,
                    GIMP_GUI_LOG_FILE_NAME,
                    GIMP_GUI_LOG_USER_ID,
                    GIMP_GUI_LOG_TIMEZONE,
                    GIMP_GUI_LOG_HEADER_FILE,
                    GIMP_GUI_LOG_WRAPPER_VERSION,
                    GIMP_GUI_LOG_PLATFORM,
                    GIMP_GUI_LOG_PLATFORM_SYSTEM,
                    GIMP_GUI_LOG_PLATFORM_RELEASE,
                    GIMP_GUI_LOG_PLATFORM_VERSION,
                    GIMP_GUI_LOG_PLATFORM_MACHINE,
                    GIMP_GUI_LOG_PLATFORM_PROCESSOR,
                    GIMP_GUI_LOG_HELPER,
                    GIMP_GUI_LOG_TEST_URL_PREFIX,
                    GIMP_GUI_LOG_COMPRESS_LOG,
                    GIMP_GUI_LOG_LOG_KEY_ACTIVITY,
                    GIMP_GUI_LOG_LOG_BUTTON_ACTIVITY,
                    GIMP_GUI_LOG_SESSION_TAGS,
                    GIMP_GUI_LOG_HISTOGRAM_TIMEOUT,
                    GIMP_GUI_LOG_NUM_DISABLED_RUNS,
                    GIMP_GUI_LOG_CONSENT_VIEW_ORDER,
                    GIMP_GUI_LOG_CONSENT_DWELL_TIMES,
                    GIMP_GUI_LOG_CONSENT_MAX_SCROLL_VALUES,
                    GIMP_GUI_LOG_CONSENT_USER_VIEW_ORDER,
                    GIMP_GUI_LOG_ONE_TIME_PAD_FILE,
                    ]:
            if var in os.environ:
                del os.environ[var]

if __name__ == '__main__':
    import UserPrefs
    prefs = UserPrefs.UserPrefs()
    launcher = Launcher(prefs)
    launcher.spawn(True)
    
