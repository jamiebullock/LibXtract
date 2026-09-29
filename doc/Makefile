# Part of LibXtract
#
# SPDX-FileCopyrightText: 2014 Jamie Bullock
# SPDX-License-Identifier: Zlib

all:
	@$(shell (cat Doxyfile; echo "PROJECT_NUMBER=$(XTRACT_VERSION)") | doxygen - )

clean:
	@$(RM) -r latex html
