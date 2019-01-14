#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# if __CODEGEARC__ < 0x0720
#   ifdef USEPACKAGES
#     pragma link "escore_ecc23.bpi"
#     pragma link "escomm_ecc23.bpi"
#     pragma link "esmath_ecc23.bpi"
#     pragma link "esscript_ecc23.bpi"
#     pragma link "escore_gui_ecc23.bpi"
#     pragma link "escomm_gui_ecc23.bpi"
#   else
#     if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
          (ES_OS != ES_OS_WINDOWS)
#       if ES_OS == ES_OS_ANDROID
#         pragma link "libescore_ecc23.a"
#         pragma link "libescomm_ecc23.a"
#         pragma link "libesmath_ecc23.a"
#         pragma link "libesscript_ecc23.a"
#         pragma link "libescore_gui_ecc23.a"
#         pragma link "libescomm_gui_ecc23.a"
#       else
#         pragma link "escore_ecc23.a"
#         pragma link "escomm_ecc23.a"
#         pragma link "esmath_ecc23.a"
#         pragma link "esscript_ecc23.a"
#         pragma link "escore_gui_ecc23.a"
#         pragma link "escomm_gui_ecc23.a"
#       endif
#     else
#       pragma link "escore_ecc23.lib"
#       pragma link "escomm_ecc23.lib"
#       pragma link "esmath_ecc23.lib"
#       pragma link "esscript_ecc23.lib"
#       pragma link "escore_gui_ecc23.lib"
#       pragma link "escomm_gui_ecc23.lib"
#     endif
#   endif
# elif __CODEGEARC__ < 0x0730
#   ifdef USEPACKAGES
#     pragma link "escore_ecc24.bpi"
#     pragma link "escomm_ecc24.bpi"
#     pragma link "esmath_ecc24.bpi"
#     pragma link "esscript_ecc24.bpi"
#     pragma link "escore_gui_ecc24.bpi"
#     pragma link "escomm_gui_ecc24.bpi"
#   else
#     if ((ES_OS == ES_OS_WINDOWS) && defined(_WIN64)) || \
          (ES_OS != ES_OS_WINDOWS)
#       if ES_OS == ES_OS_ANDROID
#         pragma link "libescore_ecc24.a"
#         pragma link "libescomm_ecc24.a"
#         pragma link "libesmath_ecc24.a"
#         pragma link "libesscript_ecc24.a"
#         pragma link "libescore_gui_ecc24.a"
#         pragma link "libescomm_gui_ecc24.a"
#       else
#         pragma link "escore_ecc24.a"
#         pragma link "escomm_ecc24.a"
#         pragma link "esmath_ecc24.a"
#         pragma link "esscript_ecc24.a"
#         pragma link "escore_gui_ecc24.a"
#         pragma link "escomm_gui_ecc24.a"
#       endif
#     else
#       pragma link "escore_ecc24.lib"
#       pragma link "escomm_ecc24.lib"
#       pragma link "esmath_ecc24.lib"
#       pragma link "esscript_ecc24.lib"
#       pragma link "escore_gui_ecc24.lib"
#       pragma link "escomm_gui_ecc24.lib"
#     endif
#   endif
# else
#   error Autolink is not supported!
# endif
#endif
