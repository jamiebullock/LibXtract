# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

# Removes the release notes that CodeRabbit writes into a pull request
# description, with the two markers that enclose them. An opening marker with
# no closing marker after it removes nothing. Text that shares a line with a
# marker is kept, and notes that span lines leave one line break in their place.

BEGIN {
    opening = "<!-- This is an auto-generated comment: release notes by coderabbit.ai -->"
    closing = "<!-- end of auto-generated comment: release notes by coderabbit.ai -->"
}

{
    text = text $0 "\n"
}

END {
    while ((start = index(text, opening)) > 0) {
        rest = substr(text, start + length(opening))
        end = index(rest, closing)
        if (end == 0)
            break
        kept = kept substr(text, 1, start - 1)
        if (index(substr(rest, 1, end - 1), "\n") > 0)
            kept = kept "\n"
        text = substr(rest, end + length(closing))
    }
    printf "%s", kept text
}
