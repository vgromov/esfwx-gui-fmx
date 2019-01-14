#pragma message "Auto-linking to escore_gui"
#ifndef ES_MONOBUILD

# if __CODEGEARC__ < 0x0710
#   ifdef USEPACKAGES
#     if ES_OS == ES_OS_MAC
#       pragma link "bplescore_ecc22.dylib"
#     else
#       pragma link "escore_ecc22.bpi"
#     endif
#   else
#     if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
          (ES_OS != ES_OS_WINDOWS)
#       if ES_OS == ES_OS_ANDROID
#         pragma link "libescore_ecc22.a"
#       else
#         pragma link "escore_ecc22.a"
#       endif
#     else
#       pragma link "escore_ecc22.lib"
#     endif
#   endif
# elif __CODEGEARC__ < 0x0720
#   ifdef USEPACKAGES
#     if ES_OS == ES_OS_MAC
#       pragma link "bplescore_ecc23.dylib"
#     else
#       pragma link "escore_ecc23.bpi"
#     endif
#   else
#     if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
          (ES_OS != ES_OS_WINDOWS)
#       if ES_OS == ES_OS_ANDROID
#         pragma link "libescore_ecc23.a"
#       else
#         pragma link "escore_ecc23.a"
#       endif
#     else
#       pragma link "escore_ecc23.lib"
#     endif
#   endif
# elif __CODEGEARC__ < 0x0730
#   ifdef USEPACKAGES
#     if ES_OS == ES_OS_MAC
#       pragma link "bplescore_ecc24.dylib"
#     else
#       pragma link "escore_ecc24.bpi"
#     endif
#   else
#     if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
          (ES_OS != ES_OS_WINDOWS)
#       if ES_OS == ES_OS_ANDROID
#         pragma link "libescore_ecc24.a"
#       else
#         pragma link "escore_ecc24.a"
#       endif
#     else
#       pragma link "escore_ecc24.lib"
#     endif
#   endif
# elif __CODEGEARC__ < 0x0740
#   ifdef USEPACKAGES
#     if ES_OS == ES_OS_MAC
#       pragma link "bplescore_ecc25.dylib"
#     else
#       pragma comment(lib, "escore_ecc25.bpi")
#     endif
#   else
#     if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
          (ES_OS != ES_OS_WINDOWS)
#       if ES_OS == ES_OS_ANDROID
#         pragma link "libescore_ecc25.a"
#       else
#         pragma link "escore_ecc25.a"
#       endif
#     else
#       pragma link "escore_ecc25.lib"
#     endif
#   endif
# else
#   error Autolink is not supported!
# endif

#endif // ES_MONOBUILD

