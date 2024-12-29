echo "/*  -- Do not commit this file --" > ./build_no.h
echo " *" >> ./build_no.h
echo " *  project   :  CAN - Controller Area Network" >> ./build_no.h
echo " *" >> ./build_no.h
echo " *  purpose   :  CAN Tester (based on macOS Library for PCAN-USB Interfaces)" >> ./build_no.h
echo " *" >> ./build_no.h
echo " *  copyright :  (C) 2019,2023 UV Software, Berlin" >> ./build_no.h
echo " *" >> ./build_no.h
echo " *  author(s) :  Uwe Vogt, UV Software" >> ./build_no.h
echo " *" >> ./build_no.h
echo " *  e-mail    :  info@mac-can.com" >> ./build_no.h
echo " *" >> ./build_no.h
echo " */" >> ./build_no.h
echo "#ifndef BUILD_NO_H_INCLUDED" >> ./build_no.h
echo "#define BUILD_NO_H_INCLUDED" >> ./build_no.h
git log -1 --pretty=format:%h > /dev/null 2> /dev/null
if [ $? -eq 0 ]
then
    echo "#define BUILD_NO 0x"$(git log -1 --pretty=format:%h) >> ./build_no.h
else
    echo "#define BUILD_NO 0xDEADC0DE" >> ./build_no.h
fi
echo "#define STRINGIFY(X) #X" >> ./build_no.h
echo "#define TOSTRING(X) STRINGIFY(X)" >> ./build_no.h
echo "#define SVN_REV_INT (BUILD_NO)" >> ./build_no.h
echo "#define SVN_REV_STR TOSTRING(BUILD_NO)" >> ./build_no.h
echo "#endif" >> ./build_no.h
echo "/*  ----------------------------------------------------------------------" >> ./build_no.h
echo " *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany" >> ./build_no.h
echo " *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903" >> ./build_no.h
echo " *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/" >> ./build_no.h
echo " */" >> ./build_no.h
