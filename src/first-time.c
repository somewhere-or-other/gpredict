/*
    Gpredict: Real-time satellite tracking and orbit prediction program

    Copyright (C)  2001-2014  Alexandru Csete, OZ9AEC.

    Authors: Alexandru Csete <oz9aec@gmail.com>

    Comments, questions and bugreports should be submitted via
    http://sourceforge.net/projects/gpredict/
    More details can be found at the project home page:

            http://gpredict.oz9aec.net/
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
  
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
  
    You should have received a copy of the GNU General Public License
    along with this program; if not, visit http://www.fsf.org/
*/
#include <glib.h>
#include <glib/gi18n.h>
#include <glib/gstdio.h>
#ifdef HAVE_CONFIG_H
#include <build-config.h>
#endif
#include "compat.h"
#include "sat-log.h"
#include "sat-cfg.h"
#include "gpredict-utils.h"
#include "first-time.h"


/* private function prototypes */
static int      first_time_check_cfg(void);
static int      first_time_check_qth(void);
static int      first_time_check_mod(void);
static int      first_time_check_sat(void);
static int      first_time_check_cache(void);
static int      first_time_check_hwconf(void);
static int      first_time_check_trsp(void);

/* debug shortcut */
#define FTC_DEBUG(f, r) sat_log_log(SAT_LOG_LEVEL_DEBUG, _("%s: %d"), f, r)

/**
 * Perform first time checks.
 * @returns A bit field describing check statuses, see #ftc_status.
 *
 * This function is called by the main function very early during program
 * startup. It's purpose is to check the user configuration to see whether
 * the configuration is complete.
 *
 * Each step in the sequence return 0 or a flag corresponding to the check
 * (see #ftc_status). The caller can then decide what to do about it.
 *
 * First time check logic:
 *
 * 0. USER_CONF_DIR already exists because sat_log_init() initializes it.
 *
 * 1. Check for USER_CONF_DIR/gpredict.cfg and set #FTC_STATUS_NO_CFG if not
 *    found.
 * 2. Check for the existence of at least one .qth file in USER_CONF_DIR
 *    If no such file found, set #FTC_STATUS_NO_QTH flag.
 * 3. Check for the existence of at least one .mod file in the
 *    USER_CONF_DIR/modules directory. Sets the flag #FTC_STATUS_NO_MOD if
 *    no module was found.
 * 4. Check for the existence of at least one .sat and .cat file in the
 *    USER_CONF_DIR/satdata directory. Sets #FTC_STATUS_NO_SAT and
 *    #FTC_STATUS_NO_CAT if no .sat or .cat file found.
 * 5. Check for the existence of USER_CONF_DIR/satdata/cache directory. This
 *    directory is used to store temporary TLE files when updating from
 *    network.
 * 6. Check for the existence of USER_CONF_DIR/hwconf directory. This
 *    directory contains radio and rotator configurations (.rig and .rot files).
 * 7. Check for the existence of USER_CONF_DIR/trsp directory. This
 *    directory contains transponder data for satellites.
 *
 * Send both error, warning and verbose debug messages to sat-log during this
 * process.
 *
 */
int first_time_check_run()
{
    int             error = 0;

    error |= first_time_check_cfg();
    error |= first_time_check_qth();
    error |= first_time_check_mod();
    error |= first_time_check_sat();
    error |= first_time_check_cache();
    error |= first_time_check_hwconf();
    error |= first_time_check_trsp();

    return error;
}

/**
 * check whether USER_CONF_DIR/gpredict.cfg exists.
 * @retval  0   USER_CONF_DIR/gpredict.cfg found.
 * @retval  #FTC_STATUS_NO_CFG  USER_CONF_DIR/gpredict.cfg was not found.
 *
 */
static int first_time_check_cfg()
{
    gchar          *dir, *cfg;
    gboolean        foundcfg = FALSE;

    dir = get_user_conf_dir();
    cfg = g_strconcat(dir, G_DIR_SEPARATOR_S, "gpredict.cfg", NULL);
    g_free(dir);

    if (g_file_test(cfg, G_FILE_TEST_EXISTS))
    {
        /* have config file */
        foundcfg = TRUE;
    }

    FTC_DEBUG(__func__, foundcfg);
    g_free(cfg);

    return foundcfg ? 0 : FTC_STATUS_NO_CFG;
}

/**
 * Check for the existence of at least one .qth file in USER_CONF_DIR.
 * @retval  0   At least one .qth file was found.
 * @retval #FTC_STATUS_NO_QTH  No .qth file was found.
 *
 */
static int first_time_check_qth()
{
    gchar          *dirname;
    const gchar    *datafile;
    gboolean        foundqth = FALSE;
    GDir           *dir;


    dirname = get_user_conf_dir();
    dir = g_dir_open(dirname, 0, NULL);

    if (dir)
    {
        /* read files, if any until we encounter a .qth file */
        while ((datafile = g_dir_read_name(dir)))
        {
            /* note: filename is not a newly allocated gchar *,
               so we must not free it */
            if (g_str_has_suffix(datafile, ".qth"))
            {
                foundqth = TRUE;
                break;
            }
        }

        g_dir_close(dir);
    }

    g_free(dirname);
    FTC_DEBUG(__func__, foundqth);

    return foundqth ? 0 : FTC_STATUS_NO_QTH;
}

/**
 * Check for module files in USER_CONF_DIR/modules directory.
 * @retval  0   At least one .mod file was found.
 * @retval  #FTC_STATUS_NO_QTH  No .mod file was found.
 */
static int first_time_check_mod()
{
    gchar          *dirname;
    const gchar    *datafile;
    gboolean        foundmod = FALSE;
    GDir           *dir;


    dirname = get_modules_dir();
    dir = g_dir_open(dirname, 0, NULL);

    /* directory does not exist */
    if (dir)
    {
        /* read files, if any until we encounter a .mod file */
        while ((datafile = g_dir_read_name(dir)))
        {
            /* note: filename is not a newly allocated gchar *,
               so we must not free it */
            if (g_str_has_suffix(datafile, ".mod"))
            {
                foundmod = TRUE;
                break;
            }
        }

        g_dir_close(dir);
    }

    g_free(dirname);
    FTC_DEBUG(__func__, foundmod);

    return foundmod ? 0 : FTC_STATUS_NO_MOD;
}

/**
 * Check for satellite data files in USER_CONF_DIR/satdata directory.
 * @retval  0   At least one .sat and one .cat file was found.
 * @retval  #FTC_STATUS_NO_SAT  No .sat file was found.
 * @retval  #FTC_STATUS_NO_CAT  No .cat file was found.
 */
static int first_time_check_sat()
{
    gchar          *dirname;
    const gchar    *datafile;
    gboolean        foundsat = FALSE;
    gboolean        foundcat = FALSE;
    GDir           *dir;

    /** TODO **/
    sat_log_log(SAT_LOG_LEVEL_ERROR, _("%s: Remember the cache directory"),
                __func__);

    dirname = get_satdata_dir();
    dir = g_dir_open(dirname, 0, NULL);

    if (dir)
    {
        /* read files, if any until we encounter a .mod file */
        while ((datafile = g_dir_read_name(dir)))
        {
            if (g_str_has_suffix(datafile, ".sat"))
                foundsat = TRUE;
            else if (g_str_has_suffix(datafile, ".cat"))
                foundcat = TRUE;
        }

        g_dir_close(dir);
    }

    g_free(dirname);
    FTC_DEBUG(__func__, foundcat);
    FTC_DEBUG(__func__, foundsat);

    return (foundcat ? 0 : FTC_STATUS_NO_CAT) |
        (foundsat ? 0 : FTC_STATUS_NO_SAT);
}

/**
 * Check for the existence of USER_CONF_DIR/satdata/cache directory.
 * @retval  0   Directory found.
 * @retval  #FTC_STATUS_NO_CACHE  Directory not found.
 */
static int first_time_check_cache()
{
    gchar          *buff, *dir;
    gboolean        foundcache = FALSE;

    buff = get_satdata_dir();
    dir = g_strconcat(buff, G_DIR_SEPARATOR_S, "cache", NULL);
    g_free(buff);

    if (g_file_test(dir, G_FILE_TEST_IS_DIR))
        foundcache = TRUE;

    g_free(dir);
    FTC_DEBUG(__func__, foundcache);

    return foundcache ? 0 : FTC_STATUS_NO_CACHE;
}

/**
 * Check for the existence of USER_CONF_DIR/hwconf directory.
 * @retval  0   Directory found.
 * @retval  #FTC_STATUS_NO_HWCONF  Directory not found.
 */
static int first_time_check_hwconf()
{
    gchar          *confdir;
    gboolean        foundconf = FALSE;

    confdir = get_hwconf_dir();

    if (g_file_test(confdir, G_FILE_TEST_IS_DIR))
        foundconf = TRUE;

    g_free(confdir);
    FTC_DEBUG(__func__, foundconf);

    return foundconf ? 0 : FTC_STATUS_NO_HWCONF;
}

/**
 * Check for the existence of USER_CONF_DIR/trsp directory.
 * @retval  0   Directory found.
 * @retval  #FTC_STATUS_NO_TRSP  Directory not found.
 */
static int first_time_check_trsp()
{
    gchar          *confdir;
    gboolean        foundconf = FALSE;

    confdir = get_trsp_dir();

    if (g_file_test(confdir, G_FILE_TEST_IS_DIR))
        foundconf = TRUE;

    g_free(confdir);
    FTC_DEBUG(__func__, foundconf);

    return foundconf ? 0 : FTC_STATUS_NO_TRSP;
}
