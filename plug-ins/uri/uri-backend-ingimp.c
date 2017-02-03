/* GIMP - The GNU Image Manipulation Program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Author: Josh MacDonald and Michael Terry. */

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>

#ifndef WIN32
#include <sys/wait.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "uri-backend.h"

#include "libgimp/stdplugins-intl.h"


#define TIMEOUT  300
#define BUFSIZE 1024


gboolean
uri_backend_init (const gchar  *plugin_name,
                  gboolean      run,
                  GimpRunMode   run_mode,
                  GError      **error)
{
  return TRUE;
}

void
uri_backend_shutdown (void)
{
}

const gchar *
uri_backend_get_load_help (void)
{
  return "Loads a file using ingimp's helper app";
}

const gchar *
uri_backend_get_save_help (void)
{
  return NULL;
}

const gchar *
uri_backend_get_load_protocols (void)
{
  return "http:,https:,ftp:";
}

const gchar *
uri_backend_get_save_protocols (void)
{
  return NULL;
}

gboolean
uri_backend_load_image (const gchar  *uri,
                        const gchar  *tmpname,
                        GimpRunMode   run_mode,
                        GError      **error)
{
  gint pid;
  gchar    *helper = NULL;

  helper = (char*)g_getenv("GIMP_GUI_LOG_HELPER");
  if (!helper)
    {
      g_set_error (error, 0, 0, "Could not get ingimp helper app name");
      return FALSE;
    }

/* For ingimp, don't rely on wget to get files -- use helper app instead */
#ifdef G_OS_WIN32

  spawnl (P_WAIT,
          helper,
          helper, "--retrieve", uri, tmpname, NULL);

#else /* G_OS_WIN32 */

  if ((pid = fork()) < 0)
    {
      g_set_error (error, 0, 0, "fork() failed: %s", g_strerror (errno));
      return FALSE;
    }
  else if (pid == 0)
    {
      execlp (helper,
              helper, "--retrieve", uri, tmpname, NULL);
      g_set_error (error, 0, 0, "exec() failed: ingimp: %s", g_strerror (errno));
      _exit (127);
    }
  else
    {
	  int stat_loc;
	  waitpid(pid, &stat_loc, 0);
    }
#endif /* G_OS_WIN32 */

  return TRUE;
}

gboolean
uri_backend_save_image (const gchar  *uri,
                        const gchar  *tmpname,
                        GimpRunMode   run_mode,
                        GError      **error)
{
  g_set_error (error, 0, 0, "EEK");

  return FALSE;
}
