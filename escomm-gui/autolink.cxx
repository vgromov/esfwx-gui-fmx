#pragma message "Auto-linking required libraries to escomm_gui"
#ifndef ES_MONOBUILD

# if __CODEGEARC__ < 0x0730
#   error ECC Version is not supported!
# elif __CODEGEARC__ < 0x0740
#   ifdef USEPACKAGES
#     pragma link "escore_gui_ecc25.bpi"
#   endif
#   if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
        (ES_OS != ES_OS_WINDOWS)
#     if ES_OS == ES_OS_ANDROID
#       pragma link "libescore_ecc25.a"
#       pragma link "libecomm_ecc25.a"
#       ifndef USEPACKAGES
#         pragma link "libescore_gui_ecc25.a"
#       endif
#     else
#       pragma link "escore_ecc25.a"
#       pragma link "escomm_ecc25.a"
#       ifndef USEPACKAGES
#         pragma link "escore_gui_ecc25.a"
#       endif
#     endif
#   else
#     pragma link "escore_ecc25.lib"
#     pragma link "escomm_ecc25.lib"
#     ifndef USEPACKAGES
#       pragma link "escore_gui_ecc25.lib"
#     endif
#   endif
# elif __CODEGEARC__ < 0x0750
#   ifdef USEPACKAGES
#     pragma link "escore_gui_ecc26.bpi"
#   endif
#   if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
        (ES_OS != ES_OS_WINDOWS)
#     if ES_OS == ES_OS_ANDROID
#       pragma link "libescore_ecc26.a"
#       pragma link "libescomm_ecc26.a"
#       ifndef USEPACKAGES
#         pragma link "libescore_gui_ecc26.a"
#       endif
#     else
#       pragma link "escore_ecc26.a"
#       pragma link "escomm_ecc26.a"
#       ifndef USEPACKAGES
#         pragma link "escore_gui_ecc26.a"
#       endif
#     endif
#   else
#     pragma link "escore_ecc26.lib"
#     pragma link "escomm_ecc26.lib"
#     ifndef USEPACKAGES
#       pragma link "escore_gui_ecc26.lib"
#     endif
#   endif
# else
#   error Autolink is not supported!
# endif

#endif // ES_MONOBUILD

