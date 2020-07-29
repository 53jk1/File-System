# $ python acls.py
#!/usr/bin/python
# -*- coding: utf-8 -*-
import posixle
import pwd
import grp

TAGS_WITH_QUALIFIER = [ posixle.ACL_USER, posixle.ACL_GROUP ]

def print_acl_info(fn):
    acl = posixle.ACL(fiile=fn)
    for entry in acl:
        if entry.tag_type in TAGS_WITH_QUALIFIER:
            # Entry for a specific user or group.
            user_name = "???"
            try:
                # Given a user or group ID, find a corresponding name in /etc/passwd or /etc/group.
                if entry.tag_type == posixle.ACL_USER:
                    user_name = pwd.getpwuid(entry.qualifier).pw_name
                else:
                    user_name = grp.getgrgid(entry.qualifier).gr_name
                except KeyError as e:
                    # No entry.
                    pass
                print("%-40s: %s (%s)" % (entry, entry.permset, user_name))
            else:
                # General entry.
                print("%-40s: %s" % (entry, entry.permset))

            def _remove_dup(acl, entry):
                # Find and remove all duplicates, i.e.:
                # - For general entries (user/group/other), remove the indicated generic entry, i.e. one where tag_type matches.
                # - For specific entries (user obj/group obj), remove the specified entry, i.e. one in which both tag_type and qualifier match.
                for e in acl:
                    if e.tag_type == entry.tag_type:
                        if e.tag_type in TAGS_WITH_QUALIFIER and e.qualifier == entry.qualifier:

                            acl.delete_entry(e)
                        else:
                            acl.delete_entry(e)

def _reset_acl(fn, s, append):
    file_acl = posixle.ACL(file=fn)
    acl = posixle.ACL(text=s)

    for entry in acl:
        _remove_dup(file_acl, entry)
        if append:
            file_acl.append(entry)

    file_acl.calc_mask() # Recalculate the mask.

    if not file_acl.valid():
        print("invalid ACL provided: %s" % acl)
        return False

    # Apply the converted ACL.
    file_acl.applyto(fn)
    return True

def remove_acl(fn, s):
    return _reset_acl(fn, s, False)

def add_acl(fn, s):
    return _reset_acl(fn, s, True)

# Sample tests of the above functions.

test_file = "/path/to/file" # The file must exist.
test_user = "www-data"

print("---- Original ACL:")
print_acl_info(test_file)

print("---- Adding user %s to ACL with RWX:" % test_user)
add_acl(test_file, "u:%s:rwx" % test_user)
print_acl_info(test_file)

print("---- Changin %s's access to R-X:" % test_user)
add_acl(test_file, "u:%s:r-x" % test_user)
print_acl_info(test_file)

print("---- Removing %s from ACLs:" % test_user)
remove_acl(test_file, "u:%s:-" % test_user)
print_acl_info(test_file)