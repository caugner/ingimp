/* The GIMP -- an image manipulation program
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


#include "config.h"
#include <gtk/gtk.h>
#include <glib.h>        /* For G_OS_WIN32 */
#include <glib/giochannel.h>
#include <glib-object.h>
#include <glib/gprintf.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <ctype.h>
#include <zlib.h>

#include <errno.h>

#include "config/gimpbaseconfig.h"

#include "libgimpbase/gimpbase.h"
#include "libgimpcolor/gimpcolor.h"
#include "libgimpmath/gimpmath.h"

#include "core-types.h"

#include "base/gimphistogram.h"
#include "base/pixel-region.h"
#include "base/temp-buf.h"
#include "base/tile-manager.h"

#include "paint-funcs/paint-funcs.h"

#include "gimpcontainer.h"
#include "gimpdrawable-histogram.h"
#include "gimp.h"
#include "gimpimage.h"
#include "gimpimage-colorhash.h"
#include "gimpimage-merge.h"
#include "gimpimage-undo.h"
#include "gimplayer.h"
#include "gimplayermask.h"
#include "gimplist.h"
#include "gimpundo.h"
#include "gimpundostack.h"
#include "gimpcontext.h"
#include "gimptoolinfo.h"
#include "pdb/gimpprocedure.h"
#include "pdb/pdb-types.h"

#include "display/display-types.h"
#include "display/gimpdisplay.h"
#include "display/gimpdisplayshell.h"

#include "gimpinteraction-logger.h"


/*
 * Defines
 */
#define GUI_LOG_VERSION_NUM "2008.4.16"
#define GIMP_GUI_LOG_FILE "GIMP_GUI_LOG_FILE"
#define GIMP_GUI_LOG_FILE_NAME "GIMP_GUI_LOG_FILE_NAME"
#define GIMP_GUI_LOG_USER_ID "GIMP_GUI_LOG_USER_ID"
#define GIMP_GUI_LOG_TIMEZONE "GIMP_GUI_LOG_TIMEZONE"
#define GIMP_GUI_LOG_HEADER_FILE "GIMP_GUI_LOG_HEADER_FILE"
#define GIMP_GUI_LOG_RUN_NUMBER "GIMP_GUI_LOG_RUN_NUMBER"
#define GIMP_GUI_LOG_RUN_TAG "GIMP_GUI_LOG_RUN_TAG"
#define GIMP_GUI_LOG_WRAPPER_VERSION "GIMP_GUI_LOG_WRAPPER_VERSION"
#define GIMP_GUI_LOG_PLATFORM "GIMP_GUI_LOG_PLATFORM"
#define GIMP_GUI_LOG_PLATFORM_SYSTEM "GIMP_GUI_LOG_PLATFORM_SYSTEM"
#define GIMP_GUI_LOG_PLATFORM_RELEASE "GIMP_GUI_LOG_PLATFORM_RELEASE"
#define GIMP_GUI_LOG_PLATFORM_VERSION "GIMP_GUI_LOG_PLATFORM_VERSION"
#define GIMP_GUI_LOG_PLATFORM_MACHINE "GIMP_GUI_LOG_PLATFORM_MACHINE"
#define GIMP_GUI_LOG_PLATFORM_PROCESSOR "GIMP_GUI_LOG_PLATFORM_PROCESSOR"
#define GIMP_GUI_LOG_TEST_URL_PREFIX "GIMP_GUI_LOG_TEST_URL_PREFIX"
#define GIMP_GUI_LOG_COMPRESS_LOG "GIMP_GUI_LOG_COMPRESS_LOG"
#define GIMP_GUI_LOG_LOG_KEY_ACTIVITY "GIMP_GUI_LOG_LOG_KEY_ACTIVITY"
#define GIMP_GUI_LOG_LOG_BUTTON_ACTIVITY "GIMP_GUI_LOG_LOG_BUTTON_ACTIVITY"
#define GIMP_GUI_LOG_SESSION_TAGS "GIMP_GUI_LOG_SESSION_TAGS"
#define GIMP_GUI_LOG_HISTOGRAM_TIMEOUT "GIMP_GUI_LOG_HISTOGRAM_TIMEOUT"
#define GIMP_GUI_LOG_NUM_DISABLED_RUNS "GIMP_GUI_LOG_NUM_DISABLED_RUNS"
#define GIMP_GUI_LOG_CONSENT_VIEW_ORDER "GIMP_GUI_LOG_CONSENT_VIEW_ORDER"
#define GIMP_GUI_LOG_CONSENT_DWELL_TIMES "GIMP_GUI_LOG_CONSENT_DWELL_TIMES"
#define GIMP_GUI_LOG_CONSENT_MAX_SCROLL_VALUES "GIMP_GUI_LOG_CONSENT_MAX_SCROLL_VALUES"
#define GIMP_GUI_LOG_CONSENT_USER_VIEW_ORDER "GIMP_GUI_LOG_CONSENT_USER_VIEW_ORDER"
#define GIMP_GUI_LOG_ONE_TIME_PAD_FILE "GIMP_GUI_LOG_ONE_TIME_PAD_FILE"
#define MAX_LOG_HEADER_LENGTH 20000
#define MAX_TAG_LENGTH 1024

/*
 * Global data
 */
static gboolean log_events = FALSE;
static gboolean compress_log = FALSE;
static gboolean log_key_events = TRUE;
static gboolean log_button_events = TRUE;
static GIOChannel* log_file = NULL;
static gzFile compressed_log_file = NULL;

static guint    entry_num = 1;
static GTimeVal log_session_start_time;
static GTimeVal last_undo_time;
static GTimeVal log_item_start_time;
static int      histogram_timeout = -1;
static gboolean button_down = FALSE;
static gboolean key_down = FALSE;

static GSList* dead_images = NULL;
static GSList* registered_windows = NULL;
static GSList* dirty_images = NULL;
static GHashTable* last_size_map = NULL;

static GHashTable* one_time_pad_map = NULL;
static GSList* one_time_pad_keys = NULL; /* Convenience to grab keys for writing since old lib versions don't have easy access to keys */
static guint last_one_time_val = 1;
static gchar* one_time_pad_filename = NULL;

static gchar* test_url_prefix = NULL;

static GimpProcedure* last_plugin_proc = NULL;


#define timestamp_size 20
static char timestamp[timestamp_size+1]; /* YYYY_MM_DD_HH_MM_SS */

/*
 * Private functions
 */
GString* guilog_start_log_message(void);
void guilog_log_message(GString* str, gboolean include_elapsed_log_time);
gboolean doc_window_button_event(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean doc_window_key_event(GtkWidget *widget, GdkEventKey *event, gpointer data);
gboolean doc_window_focus_event(GtkWidget *widget, GdkEventFocus *event, gpointer data);
gboolean doc_window_state_change_event(GtkWidget *widget, GdkEventWindowState *event, gpointer data);
void doc_window_size_requisition_event(GtkWidget *widget, GtkRequisition *requisition, gpointer data);
void doc_window_size_allocate_event(GtkWidget *widget, GtkAllocation *allocation, gpointer data);
void doc_window_showhide_event(GtkWidget *widget, gpointer data);
gboolean generic_window_button_event(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean generic_window_key_event(GtkWidget *widget, GdkEventKey *event, gpointer data);
gboolean generic_window_focus_event(GtkWidget *widget, GdkEventFocus *event, gpointer data);
gboolean generic_window_state_change_event(GtkWidget *widget, GdkEventWindowState *event, gpointer data);
void generic_window_size_requisition_event(GtkWidget *widget, GtkRequisition *requisition, gpointer data);
void generic_window_size_allocate_event(GtkWidget *widget, GtkAllocation *allocation, gpointer data);
void generic_window_showhide_event(GtkWidget *widget, gpointer data);
gboolean generic_window_destroy_event(GtkWidget *widget, GdkEvent* event, gpointer data);
void log_window_event(const gchar *event_name, GimpDisplayShell *shell);
void action_event(GtkAction *action, gpointer data);

void context_tool_changed(GimpContext *context, GimpToolInfo *tool_info, gpointer data);
void context_image_changed(GimpContext *context, GimpImage *gimage, gpointer data);
void context_foreground_changed(GimpContext *context, const GimpRGB *fg, gpointer data);
void context_background_changed(GimpContext *context, const GimpRGB *bg, gpointer data);
void context_brush_opacity_changed(GimpContext *context, gdouble opacity, gpointer data);
void context_paint_mode_changed(GimpContext *context, GimpLayerModeEffects paint_mode, gpointer data);
void context_brush_changed(GimpContext *context, gpointer data);
void context_pattern_changed(GimpContext *context, gpointer data);
void context_gradient_changed(GimpContext *context, gpointer data);
void context_palette_changed(GimpContext *context, gpointer data);
void context_font_changed(GimpContext *context, gpointer data);
gboolean gimp_exit_callback(Gimp *gimp, gpointer data);

void guilog_image_to_string(GString* str, GimpImage* gimage, gboolean write_histogram);
void guilog_write_composite_image_histogram(GString* str, GimpImage* gimage);
void guilog_layers_to_string(GString* str, GimpImage* gimage, gboolean write_histogram);
void guilog_layer_to_string(GString* str, GimpImage* gimage, GimpLayer* layer, gint layer_num, gboolean write_histogram);
void guilog_drawable_to_string(GString* str, GimpImage* gimage, GimpDrawable* drawable, gboolean write_histogram);
void guilog_write_drawable_histogram(GString* str, GimpImage* gimage, GimpDrawable* drawable);
void guilog_log_histogram_channel(GString* str, GimpHistogram* hist, gint channel_num);
void guilog_item_to_string(GString* str, GimpItem* item);
void guilog_generate_string_stats(GString* log_str, const gchar* attrib_prefix, const gchar* string_to_analyze);
gsize write_data_to_log(const voidp buf, unsigned len);
GimpLayer* guilog_flatten_image(GimpImage* gimage);
int isforwardslash(int c);
int isbackwardslash(int c);

void init_one_time_pad(void);
guint anonymize_hash_key(guint key);

double guilog_get_elapsed_time(GTimeVal* start_time);
char* guilog_get_timestamp(void);
GString* guilog_xmlify_string(const gchar* text);

gboolean histogram_callback(gpointer data);

int guilog_get_char_count(const gchar* text, int(*charFun)(int));

/* Public functions */

int guilog_start(Gimp* gimp)
{
  GError* error = NULL;
  char* file_name = NULL;
  char* header_file_name = NULL;
  char* user_id_string = NULL;
  char* timezone_string = NULL;
  char* log_file_name_string = NULL;
  char* wrapper_version_string = NULL;
  char* platform_string = NULL;
  char* platform_system_string = NULL;
  char* platform_release_string = NULL;
  char* platform_version_string = NULL;
  char* platform_machine_string = NULL;
  char* platform_processor_string = NULL;
  char* compress_log_string = NULL;
  char* log_key_activity_string = NULL;
  char* log_button_activity_string = NULL;
  char* session_tags_string = NULL;
  char* histogram_timeout_string = NULL;
  char* num_disabled_runs_string = NULL;
  int   num_disabled_runs = 0;
  char* consent_view_order_string = NULL;
  int   consent_view_order = 0;
  char* consent_dwell_times_string = NULL;
  char* consent_max_scroll_values_string = NULL;
  char* consent_user_view_order_string = NULL;
  GString* log_start = NULL;
  GdkDisplay* display = NULL;
  GimpContext* context = NULL;

  if (log_events) {
    /* Already logging... */
    return 0;
  }

  /* Mark the start of this logging session for precise timing */
  g_get_current_time(&log_session_start_time);

  /* Get the environmental variables */
  user_id_string = (char*)g_getenv(GIMP_GUI_LOG_USER_ID);
  if (!user_id_string) {
    user_id_string = "-1";
  }

  timezone_string = (char*)g_getenv(GIMP_GUI_LOG_TIMEZONE);
  if (!timezone_string) {
    timezone_string = "";
  }

  log_file_name_string = (char*)g_getenv(GIMP_GUI_LOG_FILE_NAME);
  if (!log_file_name_string) {
    log_file_name_string = "";
  }

  platform_string = (char*)g_getenv(GIMP_GUI_LOG_PLATFORM);
  if (!platform_string) {
    platform_string = "Unknown";
  }

  wrapper_version_string = (char*)g_getenv(GIMP_GUI_LOG_WRAPPER_VERSION);
  if (!wrapper_version_string) {
    wrapper_version_string = "Unknown";
  }

  platform_system_string = (char*)g_getenv(GIMP_GUI_LOG_PLATFORM_SYSTEM);
  if (!platform_system_string) {
    platform_system_string = "Unknown";
  }

  platform_release_string = (char*)g_getenv(GIMP_GUI_LOG_PLATFORM_RELEASE);
  if (!platform_release_string) {
    platform_release_string = "Unknown";
  }

  platform_version_string = (char*)g_getenv(GIMP_GUI_LOG_PLATFORM_VERSION);
  if (!platform_version_string) {
    platform_version_string = "Unknown";
  }

  platform_machine_string = (char*)g_getenv(GIMP_GUI_LOG_PLATFORM_MACHINE);
  if (!platform_machine_string) {
    platform_machine_string = "Unknown";
  }

  platform_processor_string = (char*)g_getenv(GIMP_GUI_LOG_PLATFORM_PROCESSOR);
  if (!platform_processor_string) {
    platform_processor_string = "Unknown";
  }

  session_tags_string = (char*)g_getenv(GIMP_GUI_LOG_SESSION_TAGS);
  if (!session_tags_string) {
    session_tags_string = "";
  }

  histogram_timeout_string = (char*)g_getenv(GIMP_GUI_LOG_HISTOGRAM_TIMEOUT);
  if (histogram_timeout_string) {
    histogram_timeout = atoi(histogram_timeout_string);
  }

  num_disabled_runs_string = (char*)g_getenv(GIMP_GUI_LOG_NUM_DISABLED_RUNS);
  if (num_disabled_runs_string) {
    num_disabled_runs = atoi(num_disabled_runs_string);
  }

  consent_view_order_string = (char*)g_getenv(GIMP_GUI_LOG_CONSENT_VIEW_ORDER);
  if (consent_view_order_string) {
    consent_view_order = atoi(consent_view_order_string);
  }

  consent_dwell_times_string = (char*)g_getenv(GIMP_GUI_LOG_CONSENT_DWELL_TIMES);
  consent_max_scroll_values_string = (char*)g_getenv(GIMP_GUI_LOG_CONSENT_MAX_SCROLL_VALUES);
  consent_user_view_order_string = (char*)g_getenv(GIMP_GUI_LOG_CONSENT_USER_VIEW_ORDER);

  test_url_prefix = (char*)g_getenv(GIMP_GUI_LOG_TEST_URL_PREFIX);

  compress_log_string = (char*)g_getenv(GIMP_GUI_LOG_COMPRESS_LOG);
  if (compress_log_string && strlen(compress_log_string)>0 && compress_log_string[0] == '1') {
    compress_log = TRUE;
  } else {
    compress_log = FALSE;
  }

  log_key_activity_string = (char*)g_getenv(GIMP_GUI_LOG_LOG_KEY_ACTIVITY);
  if (log_key_activity_string && strlen(log_key_activity_string)>0 && log_key_activity_string[0] == '1') {
    log_key_events = TRUE;
  } else {
    log_key_events = FALSE;
  }

  log_button_activity_string = (char*)g_getenv(GIMP_GUI_LOG_LOG_BUTTON_ACTIVITY);
  if (log_button_activity_string && strlen(log_button_activity_string)>0 && log_button_activity_string[0] == '1') {
    log_button_events = TRUE;
  } else {
    log_button_events = FALSE;
  }

  /* init our one-time pad */
  init_one_time_pad();

  /* Open the log file */
  file_name = (char*)g_getenv(GIMP_GUI_LOG_FILE);
  if (file_name) {
    if (compress_log) {
      compressed_log_file = gzopen(file_name, "wb9");
    } else {
      log_file = g_io_channel_new_file(file_name, "w", &error);
    }
  }


  if (!log_file && !compressed_log_file) {
    /* TODO: Inform wrapper of error */
    return -1;
  }

  last_size_map = g_hash_table_new(NULL, NULL);

  log_events = TRUE;

  log_start = g_string_new("<?xml version=\"1.0\"?>\n");
  g_string_append_printf(log_start,
          "<InteractionLog version=\"%s\" wrapper_version=\"%s\" user_id=\"%s\" log_date=\"%s\" timezone=\"%s\" log_file_name=\"%s\" gimp_version=\"%s\" platform=\"%s\" platform_system=\"%s\" platform_release=\"%s\" platform_version=\"%s\" platform_machine=\"%s\" platform_processor=\"%s\" num_disabled_runs=\"%d\">\n\n",
          GUI_LOG_VERSION_NUM,
          wrapper_version_string,
          user_id_string,
          guilog_get_timestamp(),
          timezone_string,
          log_file_name_string,
          GIMP_VERSION,
          platform_string,
          platform_system_string,
          platform_release_string,
          platform_version_string,
          platform_machine_string,
          platform_processor_string,
          num_disabled_runs);

  /* Add consent stats, if available */
  if (consent_dwell_times_string && strlen(consent_dwell_times_string) &&
          consent_max_scroll_values_string && strlen(consent_max_scroll_values_string) &&
          consent_user_view_order_string && strlen(consent_user_view_order_string)) {
    g_string_append_printf(log_start, "<ConsentData consent_view_order=\"%d\" consent_dwell_times=\"%s\" consent_max_scroll_values=\"%s\" consent_user_view_order=\"%s\"/>\n\n", consent_view_order, consent_dwell_times_string, consent_max_scroll_values_string, consent_user_view_order_string);
  }

  /* Add session tags */
  g_string_append(log_start, "<SessionTags>\n");
  if (strlen(session_tags_string) > 0) {
    GString* xml_safe_string = NULL;
    GString* tags_string = g_string_new(session_tags_string);
    if ((*tags_string).len > MAX_TAG_LENGTH) {
      g_string_truncate(tags_string, MAX_TAG_LENGTH);
    }
    xml_safe_string = guilog_xmlify_string((*tags_string).str);
    g_string_append(log_start, (*xml_safe_string).str);
    g_string_free(xml_safe_string, TRUE);
    g_string_free(tags_string, TRUE);
  }
  g_string_append(log_start, "\n</SessionTags>\n\n");

  g_string_append(log_start, "<LogHeader>\n");
  /* Check for log header */
  header_file_name = (char*)g_getenv(GIMP_GUI_LOG_HEADER_FILE);
  if (header_file_name) {
    GIOChannel* header_file = NULL;
    header_file = g_io_channel_new_file(header_file_name, "r", &error);
    if (header_file) {
      gchar* contents = NULL;
      gsize  length = -1;
      g_io_channel_read_to_end(header_file, &contents, &length, &error);
      if (length > 0) {
        GString* content_string = g_string_new(contents);
        GString* xml_safe_string = NULL;
        if ((*content_string).len > MAX_LOG_HEADER_LENGTH) {
          g_string_truncate(content_string, MAX_LOG_HEADER_LENGTH);
        }
        xml_safe_string = guilog_xmlify_string((*content_string).str);
        g_string_append(log_start, (*xml_safe_string).str);
        g_string_free(xml_safe_string, TRUE);
        g_string_free(content_string, TRUE);
      }
      g_io_channel_shutdown(header_file, TRUE, &error);
      g_free(contents);
    }
  }
  g_string_append(log_start, "\n</LogHeader>\n\n");

  /* Check for screens */
  g_string_append(log_start, "<DisplayInfo>\n");
  display = gdk_display_get_default();
  if (display) {
    GdkScreen* screen = NULL;
    GdkRectangle monitor_geometry;
    gint screen_num = -1;
    gint monitor_num = -1;
    gint num_screens = -1;
    gint num_monitors = -1;

    num_screens = gdk_display_get_n_screens(display);
    for (screen_num = 0; screen_num < num_screens; screen_num++) {
      screen = gdk_display_get_screen(display, screen_num);
      g_string_append_printf(log_start, "<Screen screen_num=\"%d\" screen_width=\"%d\" screen_height=\"%d\" screen_width_mm=\"%d\" screen_height_mm=\"%d\"/>\n", gdk_screen_get_number(screen), gdk_screen_get_width(screen), gdk_screen_get_height(screen), gdk_screen_get_width_mm(screen), gdk_screen_get_height_mm(screen));
    }
    screen = gdk_screen_get_default();
    if (screen) {
      num_monitors = gdk_screen_get_n_monitors(screen);
      for (monitor_num = 0; monitor_num < num_monitors; monitor_num++) {
        gdk_screen_get_monitor_geometry(screen, monitor_num, &monitor_geometry);
        g_string_append_printf(log_start, "<Monitor monitor_num=\"%d\" monitor_x=\"%d\" monitor_y=\"%d\" monitor_width=\"%d\" monitor_height=\"%d\" />\n", monitor_num, monitor_geometry.x, monitor_geometry.y, monitor_geometry.width, monitor_geometry.height);
      }
    }
  }
  g_string_append(log_start, "</DisplayInfo>\n\n");

  /* Write log start out to file */
  write_data_to_log((*log_start).str, (*log_start).len);
  g_string_free(log_start, TRUE);


  /* Patch into the context and install signal handlers for quitting the GIMP */
  if (gimp) {
    context = gimp_get_user_context(gimp);
    if (context) {
      g_signal_connect(context, "tool_changed", G_CALLBACK (context_tool_changed), (gpointer)0);
      /* Doesn't seem to do anything, so let's ditch it for now
      g_signal_connect(context, "image_changed", G_CALLBACK (context_image_changed), (gpointer)0);
      */
      g_signal_connect(context, "foreground_changed", G_CALLBACK (context_foreground_changed), (gpointer)0);
      g_signal_connect(context, "background_changed", G_CALLBACK (context_background_changed), (gpointer)0);
      g_signal_connect(context, "opacity_changed", G_CALLBACK (context_brush_opacity_changed), (gpointer)0);
      g_signal_connect(context, "paint_mode_changed", G_CALLBACK (context_paint_mode_changed), (gpointer)0);
      g_signal_connect(context, "brush_changed", G_CALLBACK (context_brush_changed), (gpointer)0);
      g_signal_connect(context, "pattern_changed", G_CALLBACK (context_pattern_changed), (gpointer)0);
      g_signal_connect(context, "gradient_changed", G_CALLBACK (context_gradient_changed), (gpointer)0);
      g_signal_connect(context, "palette_changed", G_CALLBACK (context_palette_changed), (gpointer)0);
      g_signal_connect(context, "font_changed", G_CALLBACK (context_font_changed), (gpointer)0);
    }
    g_signal_connect_after(gimp, "exit", G_CALLBACK (gimp_exit_callback), NULL);
  }

  /* Install histogram callback function that generates histograms in idle time */
  g_timeout_add_full(G_PRIORITY_LOW, 1000, histogram_callback, NULL, NULL);

  return 0;
}

int guilog_stop(void)
{
  GError* error = NULL;
  char* log_end = "</InteractionLog>\n";
  if (log_file || compressed_log_file) {
    write_data_to_log(log_end, strlen(log_end));
  }
  if (log_file) {
    g_io_channel_shutdown(log_file, TRUE, &error);
  } else if (compressed_log_file) {
    gzclose(compressed_log_file);
  }
  log_file = NULL;
  compressed_log_file = NULL;
  log_events = FALSE;
  if (dead_images) {
    g_slist_free(dead_images);
    dead_images = NULL;
  }
  if (dirty_images) {
    g_slist_free(dirty_images);
    dirty_images = NULL;
  }
  if (registered_windows) {
    g_slist_free(registered_windows);
    registered_windows = NULL;
  }
  if (last_size_map) {
    g_hash_table_destroy(last_size_map);
    last_size_map = NULL;
  }
  if (one_time_pad_map) {
    if (one_time_pad_filename) {
      // write pad to file
      gzFile f = NULL;
      f = gzopen(one_time_pad_filename, "wb9");
      if (f) {
        GSList* cur_key = NULL;

        cur_key = one_time_pad_keys;
        for (; cur_key; cur_key = g_slist_next(cur_key)) {
          guint key = 0;
          guint val = 0;
          gsize bytes_written;

          key = (guint)cur_key->data;
          val = (guint)g_hash_table_lookup(one_time_pad_map, (gpointer)key);
          bytes_written = gzwrite(f, (const voidp)&key, sizeof(key));
          bytes_written = gzwrite(f, (const voidp)&val, sizeof(key));
        }
        gzclose(f);
      }
    }
    g_slist_free(one_time_pad_keys);
    one_time_pad_keys = NULL;
    g_hash_table_destroy(one_time_pad_map);
    one_time_pad_map = NULL;
  }
  return 0;
}

void guilog_register_display_shell   ( void *in_shell)
{
  if (log_events) {
    GimpDisplayShell *shell = (GimpDisplayShell*)in_shell;

    g_signal_connect(shell, "button-press-event", G_CALLBACK(doc_window_button_event), NULL);
    g_signal_connect(shell, "button-release-event", G_CALLBACK(doc_window_button_event), NULL);

    g_signal_connect(shell, "key-press-event", G_CALLBACK(doc_window_key_event), NULL);
    g_signal_connect(shell, "key-release-event", G_CALLBACK(doc_window_key_event), NULL);

    g_signal_connect(shell, "focus-in-event", G_CALLBACK(doc_window_focus_event), NULL);
    g_signal_connect(shell, "focus-out-event", G_CALLBACK(doc_window_focus_event), NULL);
    g_signal_connect(shell, "window-state-event", G_CALLBACK(doc_window_state_change_event), NULL);
/*    g_signal_connect(shell, "size-request", G_CALLBACK(doc_window_size_requisition_event), NULL); */
    g_signal_connect(shell, "size-allocate", G_CALLBACK(doc_window_size_allocate_event), NULL);
    g_signal_connect(shell, "show", G_CALLBACK (doc_window_showhide_event), (gpointer)1);
    g_signal_connect(shell, "hide", G_CALLBACK (doc_window_showhide_event), (gpointer)0);
  }
}

void guilog_register_action          ( void          *vp_action)
{
  GtkAction* action = vp_action;
  /* Don't check for log_events here because we may not be init'ed yet */
  if (action) {
    g_signal_connect(action, "activate", G_CALLBACK (action_event), NULL);
  }
}

void guilog_register_window          ( void          *vp_window)
{
  GtkWidget* widget = vp_window;
  /* Don't check for log_events here because we may not be init'ed yet */
  if (widget) {
    GtkWindow* window = NULL;

    window = (GtkWindow*)gtk_widget_get_toplevel(widget);
    if (!GTK_WIDGET_TOPLEVEL(window)) {
      return;
    }
    if (g_slist_find(registered_windows, window)) {
      return;
    }
    registered_windows = g_slist_append(registered_windows, window);
    g_signal_connect(window, "button-press-event", G_CALLBACK(generic_window_button_event), NULL);
    g_signal_connect(window, "button-release-event", G_CALLBACK(generic_window_button_event), NULL);
    g_signal_connect(window, "key-release-event", G_CALLBACK(generic_window_key_event), NULL);
    g_signal_connect(window, "focus-in-event", G_CALLBACK(generic_window_focus_event), NULL);
    g_signal_connect(window, "focus-out-event", G_CALLBACK(generic_window_focus_event), NULL);
    g_signal_connect(window, "window-state-event", G_CALLBACK(generic_window_state_change_event), NULL);
    g_signal_connect(window, "size-allocate", G_CALLBACK(generic_window_size_allocate_event), NULL);
    g_signal_connect(window, "show", G_CALLBACK (generic_window_showhide_event), (gpointer)1);
    g_signal_connect(window, "hide", G_CALLBACK (generic_window_showhide_event), (gpointer)0);
    g_signal_connect(window, "destroy", G_CALLBACK (generic_window_destroy_event), NULL);
  }
}

void guilog_template_create_image    ( GimpImage     *gimage)
{
  if (log_events) {
    GString* log_str = guilog_start_log_message();
    g_string_append(log_str, "<GimpTemplateCreateImage>\n");
    guilog_image_to_string(log_str, gimage, FALSE); /* New image histogram captured in an undo thaw event */
    g_string_append(log_str, "</GimpTemplateCreateImage>\n");
    guilog_log_message(log_str, FALSE);
  }
}

void guilog_create_image             ( GimpImage     *gimage,
                                       const gchar   *comment)
{
  if (log_events) {
    GString* log_str = guilog_start_log_message();
    g_string_append(log_str, "<GimpCreateImage ");
    if (comment) {
      g_string_append_printf(log_str, "comment_size=\"%d\" ", strlen(comment));
    }
    g_string_append(log_str, ">\n");
    guilog_image_to_string(log_str, gimage, FALSE); /* New image histogram captured in an undo thaw event */
    g_string_append(log_str, "</GimpCreateImage>\n");

    guilog_log_message(log_str, FALSE);
  }
}

void guilog_revert_image             ( GimpImage     *old_gimage,
                                       GimpImage     *new_gimage)
{
  if (log_events) {
    GString* log_str = guilog_start_log_message();
    g_string_append(log_str, "<GimpImageRevert>\n");

    g_string_append(log_str, "<OldImage>\n");
    guilog_image_to_string(log_str, old_gimage, FALSE);
    g_string_append(log_str, "</OldImage>\n");

    g_string_append(log_str, "<NewImage>\n");
    guilog_image_to_string(log_str, new_gimage, TRUE);
    g_string_append(log_str, "</NewImage>\n");

    g_string_append(log_str, "</GimpImageRevert>\n");

    guilog_log_message(log_str, TRUE);
  }
}

void guilog_open_image(const gchar     *uri,
                       const gchar     *entered_filename,
                       GimpImage       *gimage)
{
  if (log_events) {
    GString* log_str = guilog_start_log_message();
    gboolean write_uri = FALSE;
    g_string_append(log_str, "<GimpImageOpen ");

    if (uri && test_url_prefix) {
      write_uri = (strncmp(uri, test_url_prefix, strlen(test_url_prefix)) == 0);
    }
    if (write_uri) {
      g_string_append_printf(log_str, "test_image=\"%s\" ", uri);
    } else {
      g_string_append(log_str, "test_image=\"\" ");
    }

    guilog_generate_string_stats(log_str, "uri_", uri);
    guilog_generate_string_stats(log_str, "filename_", entered_filename);

    g_string_append(log_str, ">\n");
    guilog_image_to_string(log_str, gimage, TRUE);
    g_string_append(log_str, "</GimpImageOpen>\n");

    guilog_log_message(log_str, FALSE);
  }
}

void guilog_last_opened              ( GimpImage     *gimage,
                                       gint           index,
                                       const gchar   *filename)
{
  if (log_events) {
    GString* log_str = guilog_start_log_message();
    g_string_append_printf(log_str, "<GimpImageLastOpened image_index=\"%d\" ", index);
    guilog_generate_string_stats(log_str, "filename_", filename);
    g_string_append(log_str, ">\n");
    guilog_image_to_string(log_str, gimage, FALSE); /* Histogram captured in GimpImageOpen event */
    g_string_append(log_str, "</GimpImageLastOpened>\n");

    guilog_log_message(log_str, FALSE);
  }
}

void guilog_save_image_requested     ( GimpImage     *gimage,
                                       gboolean       is_save_as)
{
  if (log_events) {
    GString* log_str = guilog_start_log_message();
    g_string_append_printf(log_str, "<GimpImageSaveRequested save_as=\"%d\">\n", is_save_as);
    guilog_image_to_string(log_str, gimage, FALSE);
    g_string_append(log_str, "</GimpImageSaveRequested>\n");

    guilog_log_message(log_str, FALSE);
  }
}

void guilog_save_image               ( GimpImage     *gimage,
                                       const gchar   *uri,
                                       const gchar   *filename,
                                       gboolean       save_a_copy,
                                       GimpPDBStatusType status)
{
  if (log_events) {
    GString* log_str = guilog_start_log_message();

    g_string_append_printf(log_str, "<GimpImageSave save_a_copy=\"%d\" status=\"%d\" ", save_a_copy, status);
    guilog_generate_string_stats(log_str, "uri_", uri);
    guilog_generate_string_stats(log_str, "filename_", filename);
    g_string_append(log_str, ">\n");
    guilog_image_to_string(log_str, gimage, TRUE);
    g_string_append(log_str, "</GimpImageSave>\n");

    guilog_log_message(log_str, TRUE);
  }
}

void guilog_image_duplicate          ( GimpImage     *old_gimage,
                                       GimpImage     *new_gimage)
{
  if (log_events) {
    GString* log_str = guilog_start_log_message();
    g_string_append(log_str, "<GimpImageDuplicate>\n");

    /*
     * Log the histogram for the new image rather than the old image
     */
    g_string_append(log_str, "<OldImage>\n");
    guilog_image_to_string(log_str, old_gimage, FALSE);
    g_string_append(log_str, "</OldImage>\n");

    g_string_append(log_str, "<NewImage>\n");
    guilog_image_to_string(log_str, new_gimage, FALSE); /* Histogram captured by an undo thaw event */
    g_string_append(log_str, "</NewImage>\n");

    g_string_append(log_str, "</GimpImageDuplicate>\n");

    guilog_log_message(log_str, FALSE);
  }
}

void guilog_image_dispose(GimpImage       *gimage)
{
  if (log_events) {
    GString* log_str = guilog_start_log_message();
    g_string_append(log_str, "<GimpImageDispose>\n");
    guilog_image_to_string(log_str, gimage, TRUE);
    g_string_append(log_str, "</GimpImageDispose>\n");

    guilog_log_message(log_str, TRUE);
  }
  dead_images = g_slist_append(dead_images, (gpointer)(*gimage).ID);
  dirty_images = g_slist_remove(dirty_images, (gpointer)gimage);
}

void guilog_undo_event               ( GimpImage     *gimage,
                                       GimpUndoEvent  event,
                                       GimpUndo      *undo)
{
  if (log_events) {
    GString* log_str = guilog_start_log_message();
    g_string_append(log_str, "<GimpUndoEvent ");
    g_string_append_printf(log_str, "undo_event_type_id=\"%d\" ", event);
    if (undo) {
      GimpViewable gimpViewable = (*undo).parent_instance;
      GimpObject gimpObject = gimpViewable.parent_instance;
      GString* xml_name = NULL;
      g_string_append_printf(log_str, "undo_time=\"%u\" ", (*undo).time);
      g_string_append_printf(log_str, "undo_command_type_id=\"%d\" ", (*undo).undo_type);
      g_string_append_printf(log_str, "undo_size=\"-1\" ");
      g_string_append_printf(log_str, "dirty_mask=\"%d\" ", (*undo).dirty_mask);
      g_string_append(log_str, ">\n");

      xml_name = guilog_xmlify_string(gimpObject.name);
      g_string_append_printf(log_str, "<UndoName>%s</UndoName>\n", (*xml_name).str);
      g_string_free(xml_name, TRUE);
    } else {
      g_string_append(log_str, ">\n");
    }
    guilog_image_to_string(log_str, gimage, event == GIMP_UNDO_EVENT_UNDO_THAW);
    g_string_append(log_str, "</GimpUndoEvent>\n");
    guilog_log_message(log_str, FALSE);

    /*
     * Only record histograms for undo pushed events (event 0).
     */
    if (event == GIMP_UNDO_EVENT_UNDO_PUSHED && undo) {
      switch (undo->undo_type) {
        case GIMP_UNDO_GROUP_IMAGE_CROP:
        case GIMP_UNDO_GROUP_IMAGE_QUICK_MASK:
        case GIMP_UNDO_GROUP_DRAWABLE:
        case GIMP_UNDO_GROUP_DRAWABLE_MOD:
        case GIMP_UNDO_GROUP_ITEM_VISIBILITY:
        case GIMP_UNDO_GROUP_LAYER_ADD_MASK:
        case GIMP_UNDO_GROUP_FS_TO_LAYER:
        case GIMP_UNDO_GROUP_FS_ANCHOR:
        case GIMP_UNDO_GROUP_FS_REMOVE:
        case GIMP_UNDO_GROUP_EDIT_PASTE:
        case GIMP_UNDO_GROUP_EDIT_CUT:
        case GIMP_UNDO_GROUP_TEXT:
        case GIMP_UNDO_GROUP_TRANSFORM:
        case GIMP_UNDO_GROUP_PAINT:
        case GIMP_UNDO_GROUP_MISC:
        case GIMP_UNDO_DRAWABLE:
        case GIMP_UNDO_DRAWABLE_MOD:
        case GIMP_UNDO_LAYER_REMOVE:
        case GIMP_UNDO_LAYER_MASK_ADD:
        case GIMP_UNDO_LAYER_MASK_REMOVE:
        case GIMP_UNDO_LAYER_REPOSITION:
        case GIMP_UNDO_LAYER_MODE:
        case GIMP_UNDO_LAYER_OPACITY:
        case GIMP_UNDO_LAYER_LOCK_ALPHA:
        case GIMP_UNDO_TEXT_LAYER:
        case GIMP_UNDO_TEXT_LAYER_MODIFIED:
        case GIMP_UNDO_CHANNEL_REMOVE:
        case GIMP_UNDO_CHANNEL_REPOSITION:
        case GIMP_UNDO_CHANNEL_COLOR:
        case GIMP_UNDO_VECTORS_REMOVE:
        case GIMP_UNDO_VECTORS_MOD:
        case GIMP_UNDO_FS_TO_LAYER:
        case GIMP_UNDO_FS_RIGOR:
        case GIMP_UNDO_FS_RELAX:
        case GIMP_UNDO_TRANSFORM:
        case GIMP_UNDO_PAINT:
        case GIMP_UNDO_INK:
          g_get_current_time(&last_undo_time);
          if (!g_slist_find(dirty_images, gimage)) {
            dirty_images = g_slist_append(dirty_images, (gpointer)gimage);
          }
          break;
        default:
          ;
      }
    }
  }
}
void guilog_plugin_invoked(GimpProcedure    *proc,
                           gboolean       reshow_last)
{
  if (log_events && proc) {
    GString* log_str = guilog_start_log_message();
    g_string_append(log_str, "<GimpPlugInInvoked ");
    g_string_append_printf(log_str, "name=\"%s\" type=\"%d\" reshow_last=\"%d\"", proc->original_name, proc->proc_type, reshow_last);
    g_string_append(log_str, "/>\n");
    last_plugin_proc = proc;
    guilog_log_message(log_str, FALSE);
  }
}

/*
 * Two different ways of handling plug-ins returning. Scripts return after completing
 * and can thus be recorded. Regular plug-ins return immediately and are caught when
 * the plug-in closes
 */
void guilog_plugin_returned(GimpProcedure    *proc)
{
  if (log_events && proc) {
    if (proc->proc_type == GIMP_TEMPORARY && proc == last_plugin_proc) {
      GString* log_str = guilog_start_log_message();
      g_string_append_printf(log_str, "<GimpPlugInCompleted name=\"%s\" type=\"%d\" />\n", proc->original_name, proc->proc_type);
      guilog_log_message(log_str, FALSE);
      last_plugin_proc = NULL;
    }
  }
}

void guilog_plugin_closed(GimpProcedure    *proc)
{
  if (log_events && proc) {
    if (proc == last_plugin_proc) {
      GString* log_str = guilog_start_log_message();
      g_string_append_printf(log_str, "<GimpPlugInCompleted name=\"%s\" type=\"%d\" />\n", proc->original_name, proc->proc_type);
      guilog_log_message(log_str, FALSE);
      last_plugin_proc = NULL;
    }
  }
}

/* Private functions */

gboolean doc_window_button_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  if (event) {
    button_down = (event->state == 0);
  }
  if (log_events && log_button_events) {
    GString *log_str = guilog_start_log_message();
    GimpDisplayShell *shell = (GimpDisplayShell*)widget;
    GimpDisplay *gdisp = NULL;
    GimpImage *gimage = NULL;
    int image_id = -1;
    int event_type = -1;
    int button_state = -1;
    int button = -1;

    if (shell) {
      gdisp = shell->display;
    }
    if (gdisp) {
      gimage = gdisp->image;
    }
    if (gimage) {
      image_id = gimage->ID;
    }
    if (event) {
      event_type = event->type;
      button_state = event->state;
      button = event->button;
    }
    g_string_append_printf(log_str, "<DocWindowButtonEvent image_id=\"%d\" event_type=\"%d\" button=\"%d\" button_state=\"%d\"/>\n", image_id, event_type, button, button_state);
    guilog_log_message(log_str, FALSE);
  }
  return FALSE;
}

gboolean doc_window_key_event(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  gboolean log_this_event = FALSE;

  if (!key_down) {
    log_this_event = TRUE;
  } else {
    log_this_event = (event && event->state == 0);
  }

  if (event) {
    key_down = (event->state != 0);
  }

  if (log_events && log_key_events && log_this_event) {
    GString *log_str = guilog_start_log_message();
    GimpDisplayShell *shell = (GimpDisplayShell*)widget;
    GimpDisplay *gdisp = NULL;
    GimpImage *gimage = NULL;
    int image_id = -1;
    int event_type = -1;
    int modifier_state = -1;

    if (shell) {
      gdisp = shell->display;
    }
    if (gdisp) {
      gimage = gdisp->image;
    }
    if (gimage) {
      image_id = gimage->ID;
    }
    if (event) {
      event_type = event->type;
      modifier_state = event->state;
    }
    g_string_append_printf(log_str, "<DocWindowKeyEvent image_id=\"%d\" event_type=\"%d\" modifier_state=\"%d\"/>\n", image_id, event_type, modifier_state);
    guilog_log_message(log_str, FALSE);
  }
  return FALSE;
}

void doc_window_size_allocate_event(GtkWidget *widget, GtkAllocation *allocation, gpointer data)
{
  if (log_events) {
    GString *log_str = NULL;
    GimpDisplayShell *shell = (GimpDisplayShell*)widget;
    GimpDisplay *gdisp = NULL;
    GimpImage *gimage = NULL;
    GdkScreen *screen = NULL;
    int image_id = -1;
    int width = -1;
    int height = -1;
    int x = -1;
    int y = -1;
    int screen_num = -1;
    int last_value = -1;

    if (!last_size_map) {
      return;
    }
    if (shell) {
      gdisp = shell->display;
      gtk_window_get_position((GtkWindow*)widget, &x, &y);
      screen = gtk_window_get_screen((GtkWindow*)widget);
      if (screen) {
        screen_num = gdk_screen_get_number(screen);
      }
    }
    if (gdisp) {
      gimage = gdisp->image;
    }
    if (gimage) {
      image_id = gimage->ID;
    }
    if (allocation) {
      width = allocation->width;
      height = allocation->height;
    }
    last_value = (int)g_hash_table_lookup(last_size_map, (gconstpointer)widget);
    if (last_value != (width*height)) {
      g_hash_table_insert(last_size_map, (gpointer)widget, (gpointer)(width*height));
      log_str = guilog_start_log_message();
      g_string_append_printf(log_str, "<DocWindowSizeEvent image_id=\"%d\" width=\"%d\" height=\"%d\" x=\"%d\" y=\"%d\" screen_num=\"%d\"/>\n", image_id, width, height, x, y, screen_num);
      guilog_log_message(log_str, FALSE);
    }
  }
}

void doc_window_size_requisition_event(GtkWidget *widget, GtkRequisition *requisition, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    GimpDisplayShell *shell = (GimpDisplayShell*)widget;
    GimpDisplay *gdisp = NULL;
    GimpImage *gimage = NULL;
    int image_id = -1;
    int width = -1;
    int height = -1;

    if (shell) {
      gdisp = shell->display;
    }
    if (gdisp) {
      gimage = gdisp->image;
    }
    if (gimage) {
      image_id = gimage->ID;
    }
    if (requisition) {
      width = requisition->width;
      height = requisition->height;
    }
    g_string_append_printf(log_str, "<DocWindowRequisitionEvent image_id=\"%d\" width=\"%d\" height=\"%d\" />\n", image_id, width, height);
    guilog_log_message(log_str, FALSE);
  }
}

gboolean doc_window_state_change_event(GtkWidget *widget, GdkEventWindowState *event, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    GimpDisplayShell *shell = (GimpDisplayShell*)widget;
    GimpDisplay *gdisp = NULL;
    GimpImage *gimage = NULL;
    int image_id = -1;
    int changed_mask = -1;
    int new_window_state = -1;

    if (shell) {
      gdisp = shell->display;
    }
    if (gdisp) {
      gimage = gdisp->image;
    }
    if (gimage) {
      image_id = gimage->ID;
    }
    if (event) {
      changed_mask = event->changed_mask;
      new_window_state = event->new_window_state;
    }
    g_string_append_printf(log_str, "<DocWindowStateEvent image_id=\"%d\" changed_mask=\"%d\" new_window_state=\"%d\"/>\n", image_id, changed_mask, new_window_state);
    guilog_log_message(log_str, FALSE);
  }
  return FALSE;
}

gboolean doc_window_focus_event(GtkWidget *widget, GdkEventFocus *event, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    GimpDisplayShell *shell = (GimpDisplayShell*)widget;
    GimpDisplay *gdisp = NULL;
    GimpImage *gimage = NULL;
    GdkScreen *screen = NULL;
    int image_id = -1;
    int event_type = -1;
    int focus_in = -1;
    int x = -1;
    int y = -1;
    int screen_num = -1;

    if (shell) {
      gdisp = shell->display;
      gtk_window_get_position((GtkWindow*)widget, &x, &y);
      screen = gtk_window_get_screen((GtkWindow*)widget);
      if (screen) {
        screen_num = gdk_screen_get_number(screen);
      }
    }
    if (gdisp) {
      gimage = gdisp->image;
    }
    if (gimage) {
      image_id = gimage->ID;
    }
    if (event) {
      event_type = event->type;
      focus_in = event->in;
    }
    g_string_append_printf(log_str, "<DocWindowFocusEvent image_id=\"%d\" event_type=\"%d\" focus_in=\"%d\" x=\"%d\" y=\"%d\" screen_num=\"%d\"/>\n", image_id, event_type, focus_in, x, y, screen_num);
    guilog_log_message(log_str, FALSE);
  }
  return FALSE;
}

void doc_window_showhide_event(GtkWidget *widget, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    GimpDisplayShell *shell = (GimpDisplayShell*)widget;
    GimpDisplay *gdisp = NULL;
    GimpImage *gimage = NULL;
    int image_id = -1;
    int show = (int)data;

    if (shell) {
      gdisp = shell->display;
    }
    if (gdisp) {
      gimage = gdisp->image;
    }
    if (gimage) {
      image_id = gimage->ID;
    }

    g_string_append_printf(log_str, "<DocWindowShowHideEvent image_id=\"%d\" showwin=\"%d\"/>\n", image_id, show);
    guilog_log_message(log_str, FALSE);
  }
}

void action_event(GtkAction *action, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    char* name = NULL;
    GString* xml_safe_string = NULL;

    if (action) {
      name = (char*)gtk_action_get_name(action);
    }
    if (name == NULL) {
      name = "";
    }
    xml_safe_string = guilog_xmlify_string(name);
    g_string_append_printf(log_str, "<ActionEvent action_name=\"%s\"/>\n", (*xml_safe_string).str);
    guilog_log_message(log_str, FALSE);
    g_string_free(xml_safe_string, TRUE);
  }
}

gboolean generic_window_button_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  if (log_events && log_button_events) {
    GString *log_str = guilog_start_log_message();
    GtkWindow *window = NULL;
    int event_type = -1;
    int button_state = -1;
    int button = -1;
    char* window_name = NULL;
    char* window_role = NULL;
    GString* xml_safe_name = NULL;
    GString* xml_safe_role = NULL;

    if (!widget) {
      return FALSE;
    }
    window = (GtkWindow*)gtk_widget_get_toplevel(widget);
    if (!GTK_WIDGET_TOPLEVEL(window)) {
      return FALSE;
    }
    /* Just to be absolutely sure... */
    if (!GIMP_IS_DISPLAY_SHELL(widget)) {
      window_name = (char*)gtk_window_get_title(window);
      window_role = (char*)gtk_window_get_role(window);
    }
    if (!window_name) {
      window_name = "";
    }
    if (!window_role) {
      window_role = "";
    }
    xml_safe_name = guilog_xmlify_string(window_name);
    xml_safe_role = guilog_xmlify_string(window_role);

    if (event) {
      event_type = event->type;
      button_state = event->state;
      button = event->button;
    }
    g_string_append_printf(log_str, "<WindowButtonEvent window_name=\"%s\" window_role=\"%s\" event_type=\"%d\" button=\"%d\" button_state=\"%d\"/>\n", xml_safe_name->str, xml_safe_role->str, event_type, button, button_state);
    guilog_log_message(log_str, FALSE);
    g_string_free(xml_safe_name, TRUE);
    g_string_free(xml_safe_role, TRUE);
  }
  return FALSE;
}

gboolean generic_window_key_event(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  if (log_events && log_key_events) {
    GString *log_str = guilog_start_log_message();
    GtkWindow *window = NULL;
    int event_type = -1;
    int modifier_state = -1;
    char* window_name = NULL;
    char* window_role = NULL;
    GString* xml_safe_name = NULL;
    GString* xml_safe_role = NULL;

    if (!widget) {
      return FALSE;
    }
    window = (GtkWindow*)gtk_widget_get_toplevel(widget);
    if (!GTK_WIDGET_TOPLEVEL(window)) {
      return FALSE;
    }
    /* Just to be absolutely sure... */
    if (!GIMP_IS_DISPLAY_SHELL(widget)) {
      window_name = (char*)gtk_window_get_title(window);
      window_role = (char*)gtk_window_get_role(window);
    }
    if (!window_name) {
      window_name = "";
    }
    if (!window_role) {
      window_role = "";
    }
    xml_safe_name = guilog_xmlify_string(window_name);
    xml_safe_role = guilog_xmlify_string(window_role);

    if (event) {
      event_type = event->type;
      modifier_state = event->state;
    }
    g_string_append_printf(log_str, "<WindowKeyEvent window_name=\"%s\" window_role=\"%s\" event_type=\"%d\" modifier_state=\"%d\"/>\n", xml_safe_name->str, xml_safe_role->str, event_type, modifier_state);
    guilog_log_message(log_str, FALSE);
    g_string_free(xml_safe_name, TRUE);
    g_string_free(xml_safe_role, TRUE);
  }
  return FALSE;
}

void generic_window_size_allocate_event(GtkWidget *widget, GtkAllocation *allocation, gpointer data)
{
  if (log_events) {
    GString *log_str = NULL;
    GtkWindow *window = NULL;
    GdkScreen *screen = NULL;
    int width = -1;
    int height = -1;
    int x = -1;
    int y = -1;
    int screen_num = -1;
    int last_value = -1;
    char* window_name = NULL;
    char* window_role = NULL;
    GString* xml_safe_name = NULL;
    GString* xml_safe_role = NULL;

    if (!widget) {
      return;
    }
    window = (GtkWindow*)gtk_widget_get_toplevel(widget);
    if (!GTK_WIDGET_TOPLEVEL(window)) {
      return;
    }
    screen = gtk_window_get_screen(window);
    if (screen) {
      screen_num = gdk_screen_get_number(screen);
    }
    /* Just to be absolutely sure... */
    if (!GIMP_IS_DISPLAY_SHELL(widget)) {
      window_name = (char*)gtk_window_get_title(window);
      window_role = (char*)gtk_window_get_role(window);
    }
    if (!window_name) {
      window_name = "";
    }
    if (!window_role) {
      window_role = "";
    }
    xml_safe_name = guilog_xmlify_string(window_name);
    xml_safe_role = guilog_xmlify_string(window_role);

    if (!last_size_map) {
      return;
    }
    gtk_window_get_position(window, &x, &y);
    if (allocation) {
      width = allocation->width;
      height = allocation->height;
    }
    last_value = (int)g_hash_table_lookup(last_size_map, (gconstpointer)window);
    if (last_value != (width*height)) {
      g_hash_table_insert(last_size_map, (gpointer)window, (gpointer)(width*height));
      log_str = guilog_start_log_message();
      g_string_append_printf(log_str, "<WindowSizeEvent window_name=\"%s\" window_role=\"%s\" width=\"%d\" height=\"%d\" x=\"%d\" y=\"%d\" screen_num=\"%d\"/>\n", xml_safe_name->str, xml_safe_role->str, width, height, x, y, screen_num);
      guilog_log_message(log_str, FALSE);
      g_string_free(xml_safe_name, TRUE);
      g_string_free(xml_safe_role, TRUE);
    }
  }
}

void generic_window_size_requisition_event(GtkWidget *widget, GtkRequisition *requisition, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    GtkWindow *window = NULL;
    GdkScreen *screen = NULL;
    int width = -1;
    int height = -1;
    int screen_num = -1;
    char* window_name = NULL;
    char* window_role = NULL;
    GString* xml_safe_name = NULL;
    GString* xml_safe_role = NULL;

    if (!widget) {
      return;
    }
    window = (GtkWindow*)gtk_widget_get_toplevel(widget);
    if (!GTK_WIDGET_TOPLEVEL(window)) {
      return;
    }
    screen = gtk_window_get_screen(window);
    if (screen) {
      screen_num = gdk_screen_get_number(screen);
    }
    /* Just to be absolutely sure... */
    if (!GIMP_IS_DISPLAY_SHELL(widget)) {
      window_name = (char*)gtk_window_get_title(window);
      window_role = (char*)gtk_window_get_role(window);
    }
    if (!window_name) {
      window_name = "";
    }
    if (!window_role) {
      window_role = "";
    }
    xml_safe_name = guilog_xmlify_string(window_name);
    xml_safe_role = guilog_xmlify_string(window_role);

    if (requisition) {
      width = requisition->width;
      height = requisition->height;
    }
    g_string_append_printf(log_str, "<WindowRequisitionEvent window_name=\"%s\" window_role=\"%s\" width=\"%d\" height=\"%d\" screen_num=\"%d\"/>\n", xml_safe_name->str, xml_safe_role->str, width, height, screen_num);
    guilog_log_message(log_str, FALSE);
    g_string_free(xml_safe_name, TRUE);
    g_string_free(xml_safe_role, TRUE);
  }
}

gboolean generic_window_state_change_event(GtkWidget *widget, GdkEventWindowState *event, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    GtkWindow *window = NULL;
    int changed_mask = -1;
    int new_window_state = -1;
    char* window_name = NULL;
    char* window_role = NULL;
    GString* xml_safe_name = NULL;
    GString* xml_safe_role = NULL;

    if (!widget) {
      return FALSE;
    }
    window = (GtkWindow*)gtk_widget_get_toplevel(widget);
    if (!GTK_WIDGET_TOPLEVEL(window)) {
      return FALSE;
    }
    /* Just to be absolutely sure... */
    if (!GIMP_IS_DISPLAY_SHELL(widget)) {
      window_name = (char*)gtk_window_get_title(window);
      window_role = (char*)gtk_window_get_role(window);
    }
    if (!window_name) {
      window_name = "";
    }
    if (!window_role) {
      window_role = "";
    }
    xml_safe_name = guilog_xmlify_string(window_name);
    xml_safe_role = guilog_xmlify_string(window_role);

    if (event) {
      changed_mask = event->changed_mask;
      new_window_state = event->new_window_state;
    }
    g_string_append_printf(log_str, "<WindowStateEvent window_name=\"%s\" window_role=\"%s\" changed_mask=\"%d\" new_window_state=\"%d\"/>\n", xml_safe_name->str, xml_safe_role->str, changed_mask, new_window_state);
    guilog_log_message(log_str, FALSE);
    g_string_free(xml_safe_name, TRUE);
    g_string_free(xml_safe_role, TRUE);
  }
  return FALSE;
}

gboolean generic_window_focus_event(GtkWidget *widget, GdkEventFocus *event, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    GtkWindow *window = NULL;
    GdkScreen *screen = NULL;
    int event_type = -1;
    int focus_in = -1;
    int x = -1;
    int y = -1;
    int screen_num = -1;
    char* window_name = NULL;
    char* window_role = NULL;
    GString* xml_safe_name = NULL;
    GString* xml_safe_role = NULL;

    if (!widget) {
      return FALSE;
    }
    window = (GtkWindow*)gtk_widget_get_toplevel(widget);
    if (!GTK_WIDGET_TOPLEVEL(window)) {
      return FALSE;
    }
    screen = gtk_window_get_screen(window);
    if (screen) {
      screen_num = gdk_screen_get_number(screen);
    }
    /* Just to be absolutely sure... */
    if (!GIMP_IS_DISPLAY_SHELL(widget)) {
      window_name = (char*)gtk_window_get_title(window);
      window_role = (char*)gtk_window_get_role(window);
    }
    if (!window_name) {
      window_name = "";
    }
    if (!window_role) {
      window_role = "";
    }
    xml_safe_name = guilog_xmlify_string(window_name);
    xml_safe_role = guilog_xmlify_string(window_role);

    gtk_window_get_position((GtkWindow*)widget, &x, &y);

    if (event) {
      event_type = event->type;
      focus_in = event->in;
    }
    g_string_append_printf(log_str, "<WindowFocusEvent window_name=\"%s\" window_role=\"%s\" event_type=\"%d\" focus_in=\"%d\" x=\"%d\" y=\"%d\" screen_num=\"%d\"/>\n", xml_safe_name->str, xml_safe_role->str, event_type, focus_in, x, y, screen_num);
    guilog_log_message(log_str, FALSE);
    g_string_free(xml_safe_name, TRUE);
    g_string_free(xml_safe_role, TRUE);
  }
  return FALSE;
}

void generic_window_showhide_event(GtkWidget *widget, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    GtkWindow *window = NULL;
    int show = (int)data;
    char* window_name = NULL;
    char* window_role = NULL;
    GString* xml_safe_name = NULL;
    GString* xml_safe_role = NULL;

    if (!widget) {
      return;
    }
    window = (GtkWindow*)gtk_widget_get_toplevel(widget);
    if (!GTK_WIDGET_TOPLEVEL(window)) {
      return;
    }
    /* Just to be absolutely sure... */
    if (!GIMP_IS_DISPLAY_SHELL(widget)) {
      window_name = (char*)gtk_window_get_title(window);
      window_role = (char*)gtk_window_get_role(window);
    }
    if (!window_name) {
      window_name = "";
    }
    if (!window_role) {
      window_role = "";
    }
    xml_safe_name = guilog_xmlify_string(window_name);
    xml_safe_role = guilog_xmlify_string(window_role);

    g_string_append_printf(log_str, "<WindowShowHideEvent window_name=\"%s\" window_role=\"%s\" showwin=\"%d\"/>\n", xml_safe_name->str, xml_safe_role->str, show);
    guilog_log_message(log_str, FALSE);
    g_string_free(xml_safe_name, TRUE);
    g_string_free(xml_safe_role, TRUE);
  }
}
gboolean generic_window_destroy_event(GtkWidget *widget, GdkEvent* event, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    GtkWindow *window = NULL;
    char* window_name = NULL;
    char* window_role = NULL;
    GString* xml_safe_name = NULL;
    GString* xml_safe_role = NULL;

    if (!widget) {
      return FALSE;
    }
    window = (GtkWindow*)gtk_widget_get_toplevel(widget);
    if (!GTK_WIDGET_TOPLEVEL(window)) {
      return FALSE;
    }
    /* Just to be absolutely sure... */
    if (!GIMP_IS_DISPLAY_SHELL(widget)) {
      window_name = (char*)gtk_window_get_title(window);
      window_role = (char*)gtk_window_get_role(window);
    }
    if (!window_name) {
      window_name = "";
    }
    if (!window_role) {
      window_role = "";
    }
    xml_safe_name = guilog_xmlify_string(window_name);
    xml_safe_role = guilog_xmlify_string(window_role);

    g_string_append_printf(log_str, "<WindowDestroyEvent window_name=\"%s\" window_role=\"%s\" />\n", xml_safe_name->str, xml_safe_role->str);
    guilog_log_message(log_str, FALSE);
    g_string_free(xml_safe_name, TRUE);
    g_string_free(xml_safe_role, TRUE);

    g_signal_handlers_disconnect_by_func(window, G_CALLBACK(generic_window_button_event), NULL);
    g_signal_handlers_disconnect_by_func(window, G_CALLBACK(generic_window_key_event), NULL);
    g_signal_handlers_disconnect_by_func(window, G_CALLBACK(generic_window_focus_event), NULL);
    g_signal_handlers_disconnect_by_func(window, G_CALLBACK(generic_window_state_change_event), NULL);
    g_signal_handlers_disconnect_by_func(window, G_CALLBACK(generic_window_size_allocate_event), NULL);
    g_signal_handlers_disconnect_by_func(window, G_CALLBACK (generic_window_showhide_event), (gpointer)1);
    g_signal_handlers_disconnect_by_func(window, G_CALLBACK (generic_window_showhide_event), (gpointer)0);
    g_signal_handlers_disconnect_by_func(window, G_CALLBACK (generic_window_destroy_event), NULL);
    registered_windows = g_slist_remove(registered_windows, window);
  }
  return FALSE;
}


void context_tool_changed(GimpContext *context, GimpToolInfo *tool_info, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    int tool_type = -1;
    int tool_options_type = -1;
    const gchar* tool_type_name = "";
    int visible = -1;
    
    if (tool_info) {
      tool_type = tool_info->tool_type;
      tool_options_type = tool_info->tool_options_type;
      tool_type_name = g_type_name(tool_type);
      if (!tool_type_name) {
        tool_type_name = "";
      }
      visible = tool_info->visible;
    }

    g_string_append_printf(log_str, "<AppContextToolChanged tool_type=\"%d\" tool_type_name=\"%s\" tool_options_type=\"%d\" visible=\"%d\"/>\n", tool_type, tool_type_name, tool_options_type, visible);
    guilog_log_message(log_str, FALSE);
  }
}

void context_image_changed(GimpContext *context, GimpImage *gimage, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    int image_id = -1;
    
    if (gimage) {
      image_id = gimage->ID;
    }

    g_string_append_printf(log_str, "<AppContextImageChanged image_id=\"%d\" />\n", image_id);
    guilog_log_message(log_str, FALSE);
  }
}

void context_foreground_changed(GimpContext *context, const GimpRGB *fg, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    g_string_append(log_str, "<AppContextForegroundChanged />\n");
    guilog_log_message(log_str, FALSE);
  }
}

void context_background_changed(GimpContext *context, const GimpRGB *bg, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    g_string_append(log_str, "<AppContextBackgroundChanged />\n");
    guilog_log_message(log_str, FALSE);
  }
}

void context_brush_opacity_changed(GimpContext *context, gdouble opacity, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    g_string_append(log_str, "<AppContextBrushOpacityChanged />\n");
    guilog_log_message(log_str, FALSE);
  }
}

void context_paint_mode_changed(GimpContext *context, GimpLayerModeEffects paint_mode, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    g_string_append(log_str, "<AppContextPaintModeChanged />\n");
    guilog_log_message(log_str, FALSE);
  }
}

void context_brush_changed(GimpContext *context, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    g_string_append(log_str, "<AppContextBrushChanged />\n");
    guilog_log_message(log_str, FALSE);
  }
}

void context_pattern_changed(GimpContext *context, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    g_string_append(log_str, "<AppContextPatternChanged />\n");
    guilog_log_message(log_str, FALSE);
  }
}

void context_gradient_changed(GimpContext *context, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    g_string_append(log_str, "<AppContextGradientChanged />\n");
    guilog_log_message(log_str, FALSE);
  }
}

void context_palette_changed(GimpContext *context, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    g_string_append(log_str, "<AppContextPaletteChanged />\n");
    guilog_log_message(log_str, FALSE);
  }
}

void context_font_changed(GimpContext *context, gpointer data)
{
  if (log_events) {
    GString *log_str = guilog_start_log_message();
    g_string_append(log_str, "<AppContextFontChanged />\n");
    guilog_log_message(log_str, FALSE);
  }
}
gboolean gimp_exit_callback(Gimp *gimp, gpointer data)
{
  guilog_stop();
  return FALSE;
}


char* guilog_get_timestamp(void)
{
  time_t cur_time;
  cur_time = time(NULL);
  strftime(timestamp, timestamp_size, "%Y_%m_%d_%H_%M_%S", localtime(&cur_time));
  return timestamp;
}

double guilog_get_elapsed_time(GTimeVal* start_time)
{
  GTimeVal cur_time;
  double   cur_time_usec;
  double   last_time_usec;

  g_get_current_time(&cur_time);
  cur_time_usec = (double)cur_time.tv_sec * 1000000.0 + (double)cur_time.tv_usec;
  last_time_usec = (double)(*start_time).tv_sec * 1000000.0 + (double)(*start_time).tv_usec;
  return cur_time_usec - last_time_usec;
}


void guilog_image_to_string(GString* str, GimpImage* gimage, gboolean write_histogram)
{
  g_string_append(str, "<GimpImage ");
  if (!gimage) {
    g_string_append(str, ">\n");
  } else {
    GimpContainer* layers = NULL;
    layers = (*gimage).layers;

    g_string_append_printf(str, "id=\"%d\" ",                 (*gimage).ID);
    g_string_append_printf(str, "width=\"%d\" ",              (*gimage).width);
    g_string_append_printf(str, "height=\"%d\" ",             (*gimage).height);
    g_string_append_printf(str, "xresolution=\"%f\" ",        (*gimage).xresolution);
    g_string_append_printf(str, "yresolution=\"%f\" ",        (*gimage).yresolution);
    g_string_append_printf(str, "resolution_unit=\"%d\" ",    (*gimage).resolution_unit);
    g_string_append_printf(str, "base_type=\"%d\" ",          (*gimage).base_type);
    g_string_append_printf(str, "num_cols=\"%d\" ",           (*gimage).num_cols);
    g_string_append_printf(str, "dirty=\"%d\" ",              (*gimage).dirty);
    g_string_append_printf(str, "dirty_time=\"%u\" ",         (*gimage).dirty_time);
    g_string_append_printf(str, "instance_count=\"%d\" ",     (*gimage).instance_count);
    g_string_append_printf(str, "disp_count=\"%d\" ",         (*gimage).disp_count);
    g_string_append_printf(str, "tattoo_state=\"%d\" ",       (*gimage).tattoo_state);
    g_string_append_printf(str, "qmask_state=\"%d\" ",        (*gimage).quick_mask_state);
    g_string_append_printf(str, "qmask_inverted=\"%d\" ",     (*gimage).quick_mask_inverted);
    g_string_append_printf(str, "group_count=\"%d\" ",        (*gimage).group_count);
    g_string_append_printf(str, "pushing_undo_group=\"%d\" ", (*gimage).pushing_undo_group);
    g_string_append_printf(str, "comp_preview_valid=\"-1\" ");
    g_string_append(str, ">\n");

    /*
     * If we should write the histogram, then check whether we have multiple,
     * visible layers. If so, we should create a composite of this image that
     * is written out.
     */
    if (write_histogram && layers) {
      GList  *list;
      int    num_visible_layers = 0;

      for (list = GIMP_LIST (gimage->layers)->list;
           list;
           list = g_list_next (list))
      {
        GimpLayer* layer = NULL;
        layer = (GimpLayer *) list->data;

        if (gimp_item_get_visible (GIMP_ITEM (layer))) {
          num_visible_layers++;
        }
      }
      if (num_visible_layers > 1) {
        guilog_write_composite_image_histogram(str, gimage);
      }
    }
    guilog_layers_to_string(str, gimage, write_histogram);
  }
  g_string_append(str, "</GimpImage>\n");
}

void guilog_layers_to_string(GString* str, GimpImage* gimage, gboolean write_histogram)
{
  GimpContainer* layers = NULL;
  gint i;

  if (!gimage)
    return;

  layers = (*gimage).layers;
  if (layers) {
    for (i = 0; i < (*layers).num_children; i++) {
      GimpLayer* this_layer = (GimpLayer*)gimp_container_get_child_by_index(layers, i);
      guilog_layer_to_string(str, gimage, this_layer, i, write_histogram);
    }
  }
}

void guilog_layer_to_string(GString* str, GimpImage* gimage, GimpLayer* layer, gint layer_num, gboolean write_histogram)
{
  GimpDrawable* drawable;

  g_string_append_printf(str, "<GimpLayer layer_num=\"%d\" ", layer_num);
  if (layer) {
    g_string_append_printf(str, "opacity=\"%f\" ",        (*layer).opacity);
    g_string_append_printf(str, "layer_mode=\"%d\" ",           (*layer).mode);
    g_string_append_printf(str, "preserve_trans=\"%d\" ", (*layer).lock_alpha);
    g_string_append_printf(str, ">\n");

    drawable = &(*layer).parent_instance;
    guilog_drawable_to_string(str, gimage, drawable, write_histogram);
  }
  g_string_append(str, "</GimpLayer>\n");
}

void guilog_drawable_to_string(GString* str, GimpImage* gimage, GimpDrawable* drawable, gboolean write_histogram)
{
  GimpItem* item;

  g_string_append(str, "<GimpDrawable ");
  if (drawable) {
    g_string_append_printf(str, "bytes=\"%d\" ",         (*drawable).bytes);
    g_string_append_printf(str, "drawable_type=\"%d\" ",          (*drawable).type);
    g_string_append_printf(str, "has_alpha=\"%d\" ",     (*drawable).has_alpha);
    g_string_append_printf(str, "preview_valid=\"%d\" ", (*drawable).preview_valid);
    g_string_append(str, ">\n");

    if (write_histogram) {
        guilog_write_drawable_histogram(str, gimage, drawable);
    }
    item = &(*drawable).parent_instance;
    guilog_item_to_string(str, item);
  }
  g_string_append(str, "</GimpDrawable>\n");
}
  
void guilog_write_drawable_histogram(GString* str, GimpImage* gimage, GimpDrawable* drawable)
{
  GimpHistogram *hist;
  gint    hist_n_channels;
  gint    channel_num;

  if (!(gimage && drawable)) {
      return;
  }

  hist = gimp_histogram_new();
  gimp_drawable_calculate_histogram(drawable, hist);
  hist_n_channels = gimp_histogram_n_channels(hist);

  guilog_log_histogram_channel(str, hist, GIMP_HISTOGRAM_VALUE);

  for (channel_num = 0; channel_num < hist_n_channels; channel_num++) {
    guilog_log_histogram_channel(str, hist, GIMP_HISTOGRAM_RED + channel_num);
  }

  if (hist_n_channels >= 3) {
    guilog_log_histogram_channel(str, hist, GIMP_HISTOGRAM_RGB);
  }

  gimp_histogram_free(hist);
}

void guilog_log_histogram_channel(GString* str, GimpHistogram* hist, gint channel)
{
  gint bin;
  gdouble hist_max,
          hist_count,
          hist_mean,
          hist_median,
          hist_std_dev;

  hist_count   = gimp_histogram_get_count   (hist, channel, 0, 255);
  hist_max     = gimp_histogram_get_maximum (hist, channel);
  hist_mean    = gimp_histogram_get_mean    (hist, channel, 0, 255);
  hist_median  = gimp_histogram_get_median  (hist, channel, 0, 255);
  hist_std_dev = gimp_histogram_get_std_dev (hist, channel, 0, 255);

  g_string_append(str, "<GimpHistogram ");
  g_string_append_printf(str, "channel=\"%d\" ", channel);
  g_string_append_printf(str, "count=\"%f\" ",   hist_count);
  g_string_append_printf(str, "max=\"%f\" ",     hist_max);
  g_string_append_printf(str, "mean=\"%f\" ",    hist_mean);
  g_string_append_printf(str, "median=\"%f\" ",  hist_median);
  g_string_append_printf(str, "std_dev=\"%f\" ", hist_std_dev);
  g_string_append(str, ">\n");
  g_string_append(str, "<HistogramData>\n");
  for (bin = 0; bin < 256; bin++) {
    g_string_append_printf(str, "%f\t", gimp_histogram_get_value(hist, channel, bin));
  }
  g_string_append(str, "\n");
  g_string_append(str, "</HistogramData>\n");
  g_string_append(str, "</GimpHistogram>\n");
}

void guilog_item_to_string(GString* str, GimpItem* item)
{
  g_string_append(str, "<GimpItem ");
  if (item) {
    gchar *name = NULL;

    g_string_append_printf(str, "id=\"%d\" ",       (*item).ID);
    g_string_append_printf(str, "width=\"%d\" ",    (*item).width);
    g_string_append_printf(str, "height=\"%d\" ",   (*item).height);
    g_string_append_printf(str, "visible=\"%d\" ",  (*item).visible);
    g_string_append_printf(str, "linked=\"%d\" ",   (*item).linked);
    g_string_append_printf(str, "floating=\"-1\" ");
    g_string_append_printf(str, "removed=\"%d\" ",  (*item).removed);

    name = (char*)gimp_object_get_name(GIMP_OBJECT(item));
    guilog_generate_string_stats(str, "name_", name);
  }
  g_string_append(str, ">\n");
  g_string_append(str, "</GimpItem>\n");
}

void guilog_write_composite_image_histogram(GString* str, GimpImage* gimage)
{

  if (gimage) {
    GimpLayer* merged_layer = NULL;

    
    merged_layer = guilog_flatten_image(gimage);
    if (merged_layer) {
      g_string_append(str, "<CompositeHistogram>\n");
      /* Need to temporarily add the layer for the histogram to work */
      gimp_container_insert(gimage->layers, GIMP_OBJECT(merged_layer), 0);
      guilog_write_drawable_histogram(str, gimage, (GimpDrawable*)merged_layer);
    /* Remove the layer */
      gimp_container_remove(gimage->layers, GIMP_OBJECT(merged_layer));
      g_string_append(str, "</CompositeHistogram>\n");
      g_object_unref (merged_layer);
    }
  }
}

/*
 * This function ripped from core/gimpimage-merge.c
 */
GimpLayer* guilog_flatten_image(GimpImage* gimage)
{
  GList           *list;
  GSList          *merge_list = NULL;
  GSList          *reverse_list = NULL;
  PixelRegion      src1PR, src2PR, maskPR;
  PixelRegion     *mask;
  GimpLayer       *merge_layer;
  GimpLayer       *layer;
  GimpLayer       *bottom_layer;
  guchar           bg[4] = {255, 255, 255, 255};
  GimpImageType    type;
  gint             x1, y1, x2, y2;
  gint             x3, y3, x4, y4;
  CombinationMode  operation;
  gint             position;
  gboolean         active[MAX_CHANNELS] = { TRUE, TRUE, TRUE, TRUE };
  gint             off_x, off_y;

  g_return_val_if_fail (GIMP_IS_IMAGE (gimage), NULL);

  for (list = GIMP_LIST (gimage->layers)->list;
       list;
       list = g_list_next (list))
    {
      layer = (GimpLayer *) list->data;

      if (gimp_item_get_visible (GIMP_ITEM (layer)))
        merge_list = g_slist_append (merge_list, layer);
    }

  layer        = NULL;
  type         = GIMP_RGBA_IMAGE;
  x1 = y1      = 0;
  x2 = y2      = 0;
  bottom_layer = NULL;

  /*  Get the layer extents  */
  while (merge_list)
    {
      layer = (GimpLayer *) merge_list->data;

      gimp_item_offsets (GIMP_ITEM (layer), &off_x, &off_y);

      if (merge_list->next == NULL)
        {
          x1 = 0;
          y1 = 0;
          x2 = gimage->width;
          y2 = gimage->height;
        }

      reverse_list = g_slist_prepend (reverse_list, layer);
      merge_list = g_slist_next (merge_list);
    }

  if ((x2 - x1) == 0 || (y2 - y1) == 0)
    return NULL;

  type = GIMP_IMAGE_TYPE_FROM_BASE_TYPE (gimp_image_base_type (gimage));

  merge_layer = gimp_layer_new (gimage, (x2 - x1), (y2 - y1),
                                type,
                                gimp_object_get_name (GIMP_OBJECT (layer)),
                                GIMP_OPACITY_OPAQUE, GIMP_NORMAL_MODE);
  if (!merge_layer)
    {
      return NULL;
    }

  GIMP_ITEM (merge_layer)->offset_x = x1;
  GIMP_ITEM (merge_layer)->offset_y = y1;

  /*  init the pixel region  */
  pixel_region_init (&src1PR,
                     gimp_drawable_get_tiles (GIMP_DRAWABLE (merge_layer)),
                     0, 0,
                     gimage->width, gimage->height,
                     TRUE);

  /*  set the region to the background color  */
  color_region (&src1PR, bg);

  position = 0;

  bottom_layer = layer;

  while (reverse_list)
    {
      GimpLayerModeEffects  mode;

      layer = (GimpLayer *) reverse_list->data;

      /*  determine what sort of operation is being attempted and
       *  if it's actually legal...
       */
      operation =
        gimp_image_get_combination_mode (gimp_drawable_type (GIMP_DRAWABLE (merge_layer)),
                                         gimp_drawable_bytes (GIMP_DRAWABLE (layer)));

      if (operation == -1)
        {
            /* TODO: Verify this is the right way to free layer */
          g_object_unref (merge_layer);
          g_slist_free (reverse_list);
          g_slist_free (merge_list);
          return NULL;
        }

      gimp_item_offsets (GIMP_ITEM (layer), &off_x, &off_y);

      x3 = CLAMP (off_x, x1, x2);
      y3 = CLAMP (off_y, y1, y2);
      x4 = CLAMP (off_x + gimp_item_width  (GIMP_ITEM (layer)), x1, x2);
      y4 = CLAMP (off_y + gimp_item_height (GIMP_ITEM (layer)), y1, y2);

      /* configure the pixel regions  */
      pixel_region_init (&src1PR,
                         gimp_drawable_get_tiles (GIMP_DRAWABLE (merge_layer)),
                         (x3 - x1), (y3 - y1), (x4 - x3), (y4 - y3),
                         TRUE);
      pixel_region_init (&src2PR,
                         gimp_drawable_get_tiles (GIMP_DRAWABLE (layer)),
                         (x3 - off_x), (y3 - off_y),
                         (x4 - x3), (y4 - y3),
                         FALSE);

      if (layer->mask && layer->mask->apply_mask)
        {
          pixel_region_init (&maskPR,
                             gimp_drawable_get_tiles (GIMP_DRAWABLE (layer->mask)),
                             (x3 - off_x), (y3 - off_y),
                             (x4 - x3), (y4 - y3),
                             FALSE);
          mask = &maskPR;
        }
      else
        {
          mask = NULL;
        }

      /* DISSOLVE_MODE is special since it is the only mode that does not
       *  work on the projection with the lower layer, but only locally on
       *  the layers alpha channel.
       */
      mode = layer->mode;
      if (layer == bottom_layer && mode != GIMP_DISSOLVE_MODE)
        mode = GIMP_NORMAL_MODE;

      combine_regions (&src1PR, &src2PR, &src1PR, mask, NULL,
                       layer->opacity * 255.999,
                       mode,
                       active,
                       operation);

      reverse_list = g_slist_next (reverse_list);
    }

  g_slist_free (reverse_list);
  g_slist_free (merge_list);

  return merge_layer;
}

GString* guilog_start_log_message(void)
{
  GString* log_str   = NULL;

  g_get_current_time(&log_item_start_time);
  log_str = g_string_new("");
  g_string_append_printf(log_str, "<LogEntry entry_num=\"%u\" event_date=\"%s\" elapsed_time_usec=\"%f\">\n", entry_num++, guilog_get_timestamp(), guilog_get_elapsed_time(&log_session_start_time));
  return log_str;
}

void guilog_log_message(GString* log_str, gboolean include_elapsed_log_time)
{
  double   time_diff;

  time_diff = guilog_get_elapsed_time(&log_item_start_time);

  if (include_elapsed_log_time) {
    g_string_append_printf(log_str, "<ElapsedLogTime elapsed_recording_time_usec=\"%f\"/>\n", time_diff);
  }
  g_string_append_printf(log_str, "</LogEntry>\n\n");
  write_data_to_log((*log_str).str, (*log_str).len);
  g_string_free(log_str, TRUE);
  if (compress_log && compressed_log_file) {
    /* Flush in case the GIMP crashes */
    gzflush(compressed_log_file, Z_SYNC_FLUSH);
  }
}

gsize write_data_to_log(const voidp buf, unsigned len)
{
  gsize    bytes_written;
  GError*  error = NULL;

  if (compress_log) {
    bytes_written = gzwrite(compressed_log_file, buf, len);
  } else {
    g_io_channel_write_chars(log_file, buf, len, &bytes_written, &error);
  }
  return bytes_written;
}

gboolean histogram_callback(gpointer data)
{
  GimpImage* gimage = NULL;
  double   time_diff = 0.0;
  time_diff = guilog_get_elapsed_time(&last_undo_time) / 1000000;

  /* Remove us if timeout is less than 0 */
  if (histogram_timeout < 0) {
    return FALSE;
  }

  if (time_diff > histogram_timeout && dirty_images && !button_down && !key_down) {
    gimage = (GimpImage*)dirty_images->data;
    dirty_images = g_slist_remove(dirty_images, dirty_images->data);
    /*
     * Make sure we don't have a disposed image
     */
    if (log_events && !g_slist_find(dead_images, (gpointer)(*gimage).ID)) {
      GString* log_str = guilog_start_log_message();
      g_string_append(log_str, "<NewImageState>\n");
      guilog_image_to_string(log_str, gimage, TRUE);
      g_string_append(log_str, "</NewImageState>\n");
      guilog_log_message(log_str, TRUE);
    }
  }
  return log_events;
}

int guilog_get_char_count(const gchar* text, int(*charFun)(int))
{
  int count = 0;
  if (!text) {
    return 0;
  }
  while (*text) {
    if (charFun(*text)) {
      count++;
    }
    text++;
  }
  return count;
}
int isforwardslash(int c)
{
  return c == '/';
}
int isbackwardslash(int c)
{
  return c == '\\';
}

void init_one_time_pad(void)
{
  one_time_pad_map = g_hash_table_new(NULL, NULL);
  one_time_pad_filename = (char*)g_getenv(GIMP_GUI_LOG_ONE_TIME_PAD_FILE);
  if (one_time_pad_filename && strlen(one_time_pad_filename)) {
    if (g_file_test(one_time_pad_filename, G_FILE_TEST_EXISTS)) {
      gzFile f = NULL;
      f = gzopen(one_time_pad_filename, "r");
      if (f) {
        int bytes_read = 0;
        do {
          guint key = 0;
          guint val = 0;

          bytes_read = gzread(f, (voidp)&key, sizeof(key));
          if (bytes_read > 0) {
            bytes_read = gzread(f, (voidp)&val, sizeof(val));
            if (bytes_read > 0) {
              g_hash_table_insert(one_time_pad_map, (gpointer)key, (gpointer)val);
              one_time_pad_keys = g_slist_prepend(one_time_pad_keys, (gpointer)key);
              last_one_time_val = MAX(last_one_time_val, val);
            }
          }
        } while (bytes_read > 0);
        gzclose(f);
      }
    }
  }
}

guint anonymize_hash_key(guint key)
{
  if (one_time_pad_map) {
    if (!g_hash_table_lookup(one_time_pad_map, (gpointer)key)) {
      guint val = ++last_one_time_val;
      one_time_pad_keys = g_slist_prepend(one_time_pad_keys, (gpointer)key);
      g_hash_table_insert(one_time_pad_map, (gpointer)key, (gpointer)val);
    }
    return (guint)g_hash_table_lookup(one_time_pad_map, (gpointer)key);
  }
  return 0;
}

/*
 * Returns a newly allocated string
 */
GString* guilog_xmlify_string(const gchar* text)
{
    GString* new_string = NULL;
    new_string = g_string_new("");
    for ( ; *text; text++) {
      if (*text == '<') {
          g_string_append(new_string, "&lt;");
      } else if (*text == '>') {
          g_string_append(new_string, "&gt;");
      } else if (*text == '"') {
          g_string_append(new_string, "&quot;");
      } else if (*text == '\'') {
          g_string_append(new_string, "&#39;");
      } else if (*text == '&') {
          g_string_append(new_string, "&amp;");
      } else {
          g_string_append_c(new_string, *text);
      }
    }
    return new_string;
}
void guilog_generate_string_stats(GString* log_str, const gchar* attrib_prefix, const gchar* string_to_analyze)
{
    int length = -1;
    int alpha_count = -1;
    int digit_count = -1;
    int punct_count = -1;
    int space_count = -1;
    int forward_slash_count = -1;
    int backward_slash_count = -1;
    guint name_hash = 0;

    if (string_to_analyze) {
      length = strlen(string_to_analyze);
      alpha_count = guilog_get_char_count(string_to_analyze, isalpha);
      digit_count = guilog_get_char_count(string_to_analyze, isdigit);
      punct_count = guilog_get_char_count(string_to_analyze, ispunct);
      space_count = guilog_get_char_count(string_to_analyze, isspace);
      forward_slash_count = guilog_get_char_count(string_to_analyze, isforwardslash);
      backward_slash_count = guilog_get_char_count(string_to_analyze, isbackwardslash);
      name_hash = anonymize_hash_key(g_str_hash(string_to_analyze));
    }
    g_string_append_printf(log_str, "%slength=\"%d\" ", attrib_prefix, length);
    g_string_append_printf(log_str, "%salpha_count=\"%d\" ", attrib_prefix, alpha_count);
    g_string_append_printf(log_str, "%sdigit_count=\"%d\" ", attrib_prefix, digit_count);
    g_string_append_printf(log_str, "%spunct_count=\"%d\" ", attrib_prefix, punct_count);
    g_string_append_printf(log_str, "%sspace_count=\"%d\" ", attrib_prefix, space_count);
    g_string_append_printf(log_str, "%sforward_slash_count=\"%d\" ", attrib_prefix, forward_slash_count);
    g_string_append_printf(log_str, "%sbackward_slash_count=\"%d\" ", attrib_prefix, backward_slash_count);
    g_string_append_printf(log_str, "%shash=\"%u\" ", attrib_prefix, name_hash);
}
