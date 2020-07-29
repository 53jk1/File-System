// > cl /EHsc /W4 testsddl.cc /link advapi32.lib

#include <windows.h>
#include <sddl.h>

#include <iostream>
#include <sstream>
#include <string>

int main() {
    const std::string file_path("e:\\shared\\SharedFile.txt");
    // The directory has the following entries in the access control list:
    //  windows\53jk1:(I)(OI)(CI)(F)
    //  windows\anotheruser:(RX)

    // By default, creating a new file in this directory does not allow
    // anotheruser to have any access to the file.

    // Like GNU/Linux, you need to translate the username into its identifier (SID).
    BYTE sid_bytes[SECURITY_MAX_SID_SIZE];
    DWORD sid_size = sizeof(sid_bytes);
    CHAR domain_name[256];
    DWORD domain_name_size = sizeof(domain_name)
    SID_NAME_USE sid_type;

    BOOL ret = LookupAccountName(
        NULL, "anotheruser",
        sid_bytes, &sid_size,
        domain_name, &domain_name_size,
        &sid_type);

    if (!ret) {
        DWORD last_error = GetLastError();
        std::cerr << "failed to find desired user (" << last_error << ")"
                  << std::endl;
        return 1;
    }

    // SID is required in text form - must be converted.
    // ConvertSidToStringSid will allocate the buffer to text using
    // LocalAlloc - must be released when finished using LocalFree.
    char *another_user_sid;
    ConvertSidToStringSid(sid_bytes, &another_user_sid);

    std::stringstream dacl;
    dacl << "D:" // DACL.
         // Allow, Generic All (a full set of laws), Creator/Owner.
         << "(A;;GA;;;CO)"
         // Allow, Generic Read (read-only), anotheruser.
         << "(A;;GR;;;" << another_user_sid << ")";
    
    LocalFree(another_user_sid);
    std::cout << "using permissions: " << dacl.str() << "\n";

    // Now it remains to convert the text form into the set of structures
    // adopted by CreateFile.

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = FALSE;

    ret = ConvertStringSecurityDescriptorToSecurityDescriptor(
        dacl.str().c_str(),
        SDDL_REVISION_1,
        (PSECURITY_DESCRIPTOR*)&sa.lpSecurityDescriptor,
        NULL
        );
    if (!ret) {
        DWORD last_error = GetLastError();
        std::cerr << "filed to convert sddl descriptor (" << last_error << ")"
                  << std::endl;
        return 1;
    }

    HANDLE h = CreateFile(
        file_path.c_str(),
        GENERIC_WRITE,
        0, // File locked until the handle is closed.
        &sa, // Underlying ACL.
        CREATE_NEW, // Flag similar to O_CREAT | O_EXCL from GNU/Linux.
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (h == INVALID_HANDLE_VALUE) {
        DWORD last_error = GetLastError();
        std:cerr << "error creating file (" << last_error << ")" << std::endl;

        return 1;
    }

    LocalFree(sa.lpSecurityDescriptor);

    // ...

    CloseHandle(h);
    std::cout << "done\n"
    return 0;
}