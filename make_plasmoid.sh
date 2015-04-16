# !/bin/bash
PROJECT_NAME='edgewheel'
echo ${PROJECT_NAME}
echo "==================CMAKE=================="
cd build
cmake -DCMAKE_INSTALL_PREFIX='kde4-config --prefix' ".."
echo "==================MAKE=================="
make
cd ..
echo "==================make install=================="
cp ./build/lib/plasma_applet_${PROJECT_NAME}.so /usr/lib/kde4
cp ./plasma-applet-${PROJECT_NAME}.desktop /usr/share/kde4/services/
cp -r ./png/* "/home/ogcc/.config/Obywatel GCC/EdgeWheel/"
echo "==================kbuildsycoca4=================="
kbuildsycoca4
echo "==================END=================="
echo "plasmoidviewer ${PROJECT_NAME}"
