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
import ConfigParser
import random
import sys
import tempfile

USER_SETTINGS = 'UserSettings'

AUTOSTART_DELAY = 'autostart_delay_in_secs' # autostart_delay_in_secs is splash screen delay before it automatically starts 
REPORTING_URL = 'reporting_url'
REDIRECT_URL = 'redirect_url'
KEEP_LOCAL_COPY = 'keep_local_copy_of_log'
UPLOADED_PREFIX = 'uploaded_prefix'
PARTICIPATION_URL = 'participation_url'
LOGFILE_PREFIX = 'logfile_prefix'
LOG_DIR = 'log_dir'
LOG_HEADER_FILE = 'log_header_file'
WEBSTATS_REDIRECT_URL = 'webstats_redirect_url'
TEST_URL_PREFIX = 'test_url_prefix'
COMPRESS_LOG = 'compress_log'
LOG_KEY_ACTIVITY = 'log_key_activity'
LOG_BUTTON_ACTIVITY = 'log_button_activity'
SESSION_TAGS = 'session_tags'
HISTOGRAM_TIMEOUT = 'histogram_timeout'
NUM_DISABLED_RUNS = 'num_disabled_runs'

PRIVATE_SETTINGS = 'PrivateSettings'
USER_ID = 'user_id'
CONSENT_ID = 'consent_id'
KILLSWITCH = 'kill_switch'
SHOWN_PICTOGRAMS = 'shown_pictograms'

'''
Participation URL is the URL queried to see if there are any
opportunities for additional participation

reporting_url is the URL all log files are sent to.

If reporting_url is empty, redirect_url causes the wrapper to contact
the redirect URL to get the "real" site to submit log files to.

If both are None, then no uploads will take place and all log files will
be stored in log_dir, defined below.

The log_header_file is an optional file that will be included at the
top of a log file. There is a limit to how much text can be included
Example:
    log_header_file = 'my_unique_data.txt'

To ensure a local copy of the log is kept, even after it is uploaded, set keep_local_copy
to True. After the log file is uploaded, its name will be changed to have
uploaded_prefix prepended to it.
'''

_default_user_prefs = {
    AUTOSTART_DELAY : -1, # Turn off autostart by default
    REPORTING_URL : '',
    REDIRECT_URL : 'http://www.ingimp.org/redirects/logserver.html',
    KEEP_LOCAL_COPY : '0',
    UPLOADED_PREFIX : 'uploaded_',
    PARTICIPATION_URL : 'http://www.ingimp.org/redirects/participation.html',
    LOGFILE_PREFIX : 'ingimp_',
    LOG_DIR : str(tempfile.gettempdir()),
    LOG_HEADER_FILE : '',
    WEBSTATS_REDIRECT_URL : 'http://www.ingimp.org/redirects/webstats.html',
    TEST_URL_PREFIX : 'http://www.ingimp.org/reality_checks',
    COMPRESS_LOG : '1',
    LOG_KEY_ACTIVITY : '1',
    LOG_BUTTON_ACTIVITY : '1',
    SESSION_TAGS : '',
    HISTOGRAM_TIMEOUT : '-1',
    NUM_DISABLED_RUNS : '0',
}

class UserPrefs:
    def __init__(self):
        self.user_id = ""
        self.have_consent = False
        self.have_killswitch = False
        self.shown_pictograms = True

        prefs_dir = os.getenv('APPDATA')
        if not prefs_dir:
            prefs_dir = os.getenv('USERPROFILE')
        if not prefs_dir:
            prefs_dir = os.getenv('HOME')
        if not prefs_dir:
            prefs_dir = '.'
        prefs_dir = os.path.join(prefs_dir, '.ingimp')
        if not os.path.exists(prefs_dir):
            os.mkdir(prefs_dir)

        self.prefs_dir = prefs_dir
        self.prefs_file = os.path.join(prefs_dir, 'ingimp.ini')
        if not os.path.exists(self.prefs_file):
            self._create_prefs_file()

        self.config = ConfigParser.ConfigParser()
        self.config.read(self.prefs_file)
        self._check_defaults()

        self.user_id = self.config.get(PRIVATE_SETTINGS, USER_ID)
        self.have_consent = int(self.config.get(PRIVATE_SETTINGS, CONSENT_ID)) != 0
        self.have_killswitch = int(self.config.get(PRIVATE_SETTINGS, KILLSWITCH)) != 0

    def get(self, key):
        return self.config.get(USER_SETTINGS, key)
    def set(self, key, value):
        self.config.set(USER_SETTINGS, key, value)
        self._write_prefs()
    def get_one_time_pad_file(self):
        return os.path.join(self.prefs_dir, 'one_time.bin.gz')

    def _check_defaults(self):
        write_prefs = False
        for section in [USER_SETTINGS, PRIVATE_SETTINGS]:
            if not self.config.has_section(section):
                self.config.add_section(section)
                write_prefs = True
        if not self.config.has_option(PRIVATE_SETTINGS, CONSENT_ID):
            self.config.set(PRIVATE_SETTINGS, CONSENT_ID, '0')
            write_prefs = True
        if not self.config.has_option(PRIVATE_SETTINGS, KILLSWITCH):
            self.config.set(PRIVATE_SETTINGS, KILLSWITCH, '0')
            write_prefs = True
        if not self.config.has_option(PRIVATE_SETTINGS, SHOWN_PICTOGRAMS):
            self.config.set(PRIVATE_SETTINGS, SHOWN_PICTOGRAMS, '1')
            self.shown_pictograms = False
            write_prefs = True
        for key in _default_user_prefs:
            if not self.config.has_option(USER_SETTINGS, key):
                self.config.set(USER_SETTINGS, key, str(_default_user_prefs[key]))
                write_prefs = True
        if write_prefs:
            self._write_prefs()
    def _create_prefs_file(self):
        # Generate random ID for user
        # There is a small chance that two people will get identical
        # IDs, but I'd rather we generate the IDs locally than require
        # a network connection the first time we start up
        random.seed()
        user_id = str(random.randint(0,sys.maxint))
        random.seed()
        user_id = user_id + '_' + str(random.randint(0,sys.maxint))
        config = ConfigParser.ConfigParser()
        config.add_section(PRIVATE_SETTINGS)
        config.add_section(USER_SETTINGS)
        config.set(PRIVATE_SETTINGS, USER_ID, user_id)
        config.set(PRIVATE_SETTINGS, CONSENT_ID, '0')
        config.set(PRIVATE_SETTINGS, KILLSWITCH, '0')
        f = open(self.prefs_file, 'w')
        config.write(f)
        f.close()
        del config
    def _write_prefs(self):
        f = open(self.prefs_file, 'w')
        self.config.write(f)
        f.close()
    def set_have_consent(self):
        self.config.set(PRIVATE_SETTINGS, CONSENT_ID, '1')
        self.have_consent = True
        self._write_prefs()
    def set_have_killswitch(self):
        self.config.set(PRIVATE_SETTINGS, KILLSWITCH, '1')
        self.have_killswitch = True
        self._write_prefs()

if __name__ == '__main__':
    prefs = UserPrefs()
    print "ID:", prefs.user_id, "Have consent:", prefs.have_consent, "Kill switch:", prefs.have_killswitch
