# Copyright (C) 2000 Sergey A. Dolin <dsa@glug.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

BUILT_SOURCES = guile-fcgi.x
INCLUDES = $(GUILE_CFLAGS) $(FCGI_CFLAGS)

bin_PROGRAMS = guile-fcgi
guile_fcgi_SOURCES = guile-fcgi.c 
guile_fcgi_CFLAGS = $(CFLAGS) $(GUILE_CFLAGS) $(FCGI_CFLAGS)
guile_fcgi_LDFLAGS = $(CFLAGS) $(GUILE_LDFLAGS) $(FCGI_LDFLAGS) 


SUFFIXES = .x
.c.x:
	guile-snarf $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $< > $@

MKDEP = gcc -M -MG $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) 

MOSTLYCLEANFILES = *~
DISTCLEANFILES = *.x 

TESTS = test.scm
