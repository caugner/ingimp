#! /usr/bin/env python

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

'''
This is a helper app for ingimp. Currently, it retrieves URLs to make
the URL plug-in work for the Windows version of the GIMP.
'''

import urllib
import sys

def retrieve_file(uri, local_file):
    try:
        remote_file = urllib.urlopen(uri)
        local_file = open(local_file, "wb")
        for line in remote_file.readlines():
            local_file.write(line)
        remote_file.close()
        local_file.close()
    except Exception, e:
        print "Error in retrieve_file:", str(e)

if __name__ == '__main__':
    for argnum, arg in zip(range(len(sys.argv)), sys.argv):
        if arg == "--retrieve":
            remote_file = sys.argv[argnum+1]
            # Temporary path can have spaces in it, so we need to assume
            # every argument after the remote file is part of the local
            # file name. Hence, we add spaces
            local_file = reduce(lambda x,y: x+" "+y, sys.argv[argnum+2:])
            retrieve_file(remote_file, local_file)
