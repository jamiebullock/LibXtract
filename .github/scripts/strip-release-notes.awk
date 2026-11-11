# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

# Removes the release notes that CodeRabbit writes into a pull request
# description, with the two markers that enclose them. An opening marker with
# no closing marker after it removes nothing.

BEGIN {
    opening = "<!-- This is an auto-generated comment: release notes by coderabbit.ai -->"
    closing = "<!-- end of auto-generated comment: release notes by coderabbit.ai -->"
    held = 0
}

!held && index($0, opening) {
    if (index($0, closing))
        next
    held = 1
    block = $0
    next
}

held {
    if (index($0, closing))
        held = 0
    else
        block = block "\n" $0
    next
}

{
    print
}

END {
    if (held)
        print block
}
