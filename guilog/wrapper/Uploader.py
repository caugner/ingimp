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
import shutil
import Config
import urllib
import urllib2
import UploadDialog
import UserPrefs
import Utils
import Tkinter as tk
import tkMessageBox
import gzip
import StringIO
import base64
from threading import Thread

class Uploader(Thread):
    def __init__(self, prefs):
        Thread.__init__(self)
        self.prefs = prefs
        self.log_files = []
    def upload(self):
        # Get all log files in the log directory
        self.log_dir = self.prefs.get(UserPrefs.LOG_DIR)
        self.logfile_prefix = self.prefs.get(UserPrefs.LOGFILE_PREFIX)
        self.reporting_url = self.prefs.get(UserPrefs.REPORTING_URL)
        self.redirect_url = self.prefs.get(UserPrefs.REDIRECT_URL)
        self.keep_local_copy = int(self.prefs.get(UserPrefs.KEEP_LOCAL_COPY))
        self.uploaded_prefix = self.prefs.get(UserPrefs.UPLOADED_PREFIX)

        self.log_files = os.listdir(self.log_dir)
        self.log_files = filter(lambda x: x.startswith(self.logfile_prefix), self.log_files)
        if self.log_files:
            # If no URL is available, we won't try to upload and will
            # just leave the log files on disk
            if not (self.reporting_url or self.redirect_url):
                return
            # Check to see if we need to get a redirected URL
            if self.reporting_url:
                self.server_url = self.reporting_url
            else:
                self.server_url = Utils.get_redirect_url(self.redirect_url)
                if not self.server_url:
                    return
            self.progress_dialog = UploadDialog.UploadDialog(tk.Toplevel())
            # Spawn ourselves as a thread then show the dialog
            self.start()
# Don't show a progress dialog per our finding that it is likely to be confusing to users when they see it.
# Also, in most cases, it is difficult to cancel uploads in time and the semantics
# of "cancel" are unclear -- does it completely cancel upload of that log file?
# Or does it just cancel this one, single upload?
#            self.progress_dialog.show()
    def run(self):
        for (this_log, file_num) in zip(self.log_files, range(len(self.log_files))):
            if self.progress_dialog.user_cancelled:
                self.progress_dialog.close()
                return
            try:
                self.progress_dialog.set_current_file_num(file_num+1, len(self.log_files)+1)
                fullname = os.path.join(self.log_dir, this_log)
                source_file = None
                if not fullname.endswith('.gz'):
                    string_file = StringIO.StringIO()
                    f = open(fullname)
                    gzip_file = gzip.GzipFile(fileobj=string_file, mode='wb')
                    line = f.readline()
                    while line:
                        gzip_file.write(line)
                        line = f.readline()
                    gzip_file.close()
                    f.close()
                    source_file = string_file
                else:
                    f = open(fullname, 'rb')
                    source_file = f
                source_file.seek(0)
                dest_file = StringIO.StringIO()
                base64.encode(source_file, dest_file)
                source_file.close()
                dest_file.seek(0)
                request = urllib2.Request(self.server_url, dest_file.getvalue())
                connection = urllib2.urlopen(request)
                if self.keep_local_copy:
                    newname = os.path.join(self.log_dir, self.uploaded_prefix+this_log)
                    try:
                        shutil.move(fullname, newname)
                    except Exception, e:
                        tkMessageBox.showerror("Upload Error", "Could not rename uploaded log file: "+str(e))
                else:
                    os.remove(fullname)
                if connection:
                    connection.close()
            except Exception, e:
                tkMessageBox.showerror("Upload Error", "Could not upload files: "+str(e))
                break
        self.progress_dialog.close()
                
if __name__ == '__main__':
    prefs = UserPrefs.UserPrefs()
    u = Uploader(prefs)
    u.upload()
