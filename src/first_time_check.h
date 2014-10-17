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
#ifndef FIRST_TIME_CHECK_H
#define FIRST_TIME_CHECK_H 1


/** Bit fields in the returned status code */
enum ftc_status {
    FTC_STATUS_NO_CFG = 1 << 1, /*!< No gpredict.cfg found. */
    FTC_STATUS_NO_QTH = 1 << 2, /*!< No .qth file found. */
    FTC_STATUS_NO_MOD = 1 << 3, /*!< No .mod file found. */
    FTC_STATUS_NO_SAT = 1 << 4, /*!< No .sat file found. */
    FTC_STATUS_NO_CAT = 1 << 5, /*!< No .cat file found. */
    FTC_STATUS_NO_CACHE = 1 << 6,       /*!< No satdata/cache directory. */
    FTC_STATUS_NO_HWCONF = 1 << 7,      /*!< No hwconf directory. */
    FTC_STATUS_NO_TRSP = 1 << 8,        /*!< No trsp directory. */
};

int             first_time_check_run(void);

#endif
