#pragma message "Auto-linking to escomm_gui"
#ifndef ES_MONOBUILD

# if __CODEGEARC__ < 0x0710
#   ifdef USEPACKAGES
#     pragma link "escore_ecc22.bpi"
#     pragma link "escomm_ecc22.bpi"
#     pragma link "escore_gui_ecc22.bpi"
#   else
#     if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
          (ES_OS != ES_OS_WINDOWS)
#       if ES_OS == ES_OS_ANDROID
#         pragma link "libescore_ecc22.a"
#         pragma link "libescomm_ecc22.a"
#         pragma link "libescore_gui_ecc22.a"
#       else
#         pragma link "escore_ecc22.a"
#         pragma link "escomm_ecc22.a"
#         pragma link "escore_gui_ecc22.a"
#       endif
#     else
#       pragma link "escore_ecc22.lib"
#       pragma link "escomm_ecc22.lib"
#       pragma link "escore_gui_ecc22.lib"
#     endif
#   endif
# elif __CODEGEARC__ < 0x0720
#   ifdef USEPACKAGES
#     pragma link "escore_ecc23.bpi"
#     pragma link "escomm_ecc23.bpi"
#     pragma link "escore_gui_ecc23.bpi"
#   else
#     if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
          (ES_OS != ES_OS_WINDOWS)
#       if ES_OS == ES_OS_ANDROID
#         pragma link "libescore_ecc23.a"
#         pragma link "libescomm_ecc23.a"
#         pragma link "libescore_gui_ecc23.a"
#       else
#         pragma link "escore_ecc23.a"
#         pragma link "escomm_ecc23.a"
#         pragma link "escore_gui_ecc23.a"
#       endif
#     else
#       pragma link "escore_ecc23.lib"
#       pragma link "escomm_ecc23.lib"
#       pragma link "escore_gui_ecc23.lib"
#     endif
#   endif
# elif __CODEGEARC__ < 0x0730
#   ifdef USEPACKAGES
#     pragma link "escore_ecc24.bpi"
#     pragma link "escomm_ecc24.bpi"
#     pragma link "escore_gui_ecc24.bpi"
#   else
#     if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
          (ES_OS != ES_OS_WINDOWS)
#       if ES_OS == ES_OS_ANDROID
#         pragma link "libescore_ecc24.a"
#         pragma link "libescomm_ecc24.a"
#         pragma link "libescore_gui_ecc24.a"
#       else
#         pragma link "escore_ecc24.a"
#         pragma link "escomm_ecc24.a"
#         pragma link "escore_gui_ecc24.a"
#       endif
#     else
#       pragma link "escore_ecc24.lib"
#       pragma link "escomm_ecc24.lib"
#       pragma link "escore_gui_ecc24.lib"
#     endif
#   endif
# elif __CODEGEARC__ < 0x0740
#   ifdef USEPACKAGES
#     pragma comment(lib, "escore_ecc25.bpi")
#     pragma comment(lib, "escomm_ecc25.bpi")
#     pragma comment(lib, "escore_gui_ecc25.bpi")
#   else
#     if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
          (ES_OS != ES_OS_WINDOWS)
#       if ES_OS == ES_OS_ANDROID
#         pragma link "libescore_ecc25.a"
#         pragma link "libescomm_ecc25.a"
#         pragma link "libescore_gui_ecc25.a"
#       else
#         pragma link "escore_ecc25.a"
#         pragma link "escomm_ecc25.a"
#         pragma link "escore_gui_ecc25.a"
#       endif
#     else
#       pragma link "escore_ecc25.lib"
#       pragma link "escomm_ecc25.lib"
#       pragma link "escore_gui_ecc25.lib"
#     endif
#   endif
# else
#   error Autolink is not supported!
# endif

#endif // ES_MONOBUILD

