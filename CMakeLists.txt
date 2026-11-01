cmake_minimum_required(VERSION 3.16)
project(utdocscan CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Gui Qml Quick Concurrent REQUIRED)

# Tesseract 4.1 and Leptonica from the Ubuntu 20.04 archive (clickable.yaml
# installs them into the build container and bundles the libraries).
find_package(PkgConfig REQUIRED)
pkg_check_modules(TESSERACT REQUIRED IMPORTED_TARGET tesseract lept)

# ---------------------------------------------------------------------------
# Translations: i18n/utdocscan_<lang>.ts -> .qm, embedded as :/i18n/*.qm.
# LanguageManager loads the one for the system or the chosen language.
# ---------------------------------------------------------------------------
find_package(Qt5LinguistTools REQUIRED)
file(GLOB UTDOCSCAN_TS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/i18n/utdocscan_*.ts)
qt5_add_translation(UTDOCSCAN_QM_FILES ${UTDOCSCAN_TS_FILES})
set(UTDOCSCAN_I18N_QRC ${CMAKE_CURRENT_BINARY_DIR}/i18n.qrc)
set(_i18n_entries "")
foreach(_qm ${UTDOCSCAN_QM_FILES})
    get_filename_component(_qm_name ${_qm} NAME)
    string(APPEND _i18n_entries "    <file alias=\"i18n/${_qm_name}\">${_qm}</file>\n")
endforeach()
file(WRITE ${UTDOCSCAN_I18N_QRC}
     "<RCC>\n  <qresource prefix=\"/\">\n${_i18n_entries}  </qresource>\n</RCC>\n")
# The .qrc references generated .qm files, so they must exist before rcc runs.
add_custom_target(utdocscan_translations DEPENDS ${UTDOCSCAN_QM_FILES})

add_executable(utdocscan
    src/main.cpp
    src/documentlibrary.cpp
    src/documentlibrary.h
    src/languagemanager.cpp
    src/languagemanager.h
    src/pageprocessor.cpp
    src/pageprocessor.h
    src/pdfexporter.cpp
    src/pdfexporter.h
    src/scansession.cpp
    src/scansession.h
    src/systemtheme.cpp
    src/systemtheme.h
    qml.qrc
    ${UTDOCSCAN_I18N_QRC})
add_dependencies(utdocscan utdocscan_translations)

target_include_directories(utdocscan PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(utdocscan
    Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick Qt5::Concurrent PkgConfig::TESSERACT)

install(TARGETS utdocscan RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES manifest.json DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES utdocscan.desktop DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES utdocscan.apparmor DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES utdocscan.svg DESTINATION ${CMAKE_INSTALL_PREFIX})

# OCR language models and the glyph-less font Tesseract needs for the
# invisible text layer of the PDF. The phone has neither, so they ship in the
# click. Languages must match LanguageManager's OCR list.
set(TESSDATA_SOURCE_DIR /usr/share/tesseract-ocr/4.00/tessdata)
set(UTDOCSCAN_OCR_LANGUAGES eng deu fra spa ita)
foreach(_lang ${UTDOCSCAN_OCR_LANGUAGES})
    install(FILES ${TESSDATA_SOURCE_DIR}/${_lang}.traineddata
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/tessdata)
endforeach()
install(FILES ${TESSDATA_SOURCE_DIR}/pdf.ttf DESTINATION ${CMAKE_INSTALL_PREFIX}/share/tessdata)
