source .config/setup.sh

target=$(dialog --keep-tite --inputbox "Target Name" 0 $DIALOGWIDTH "code" 2>&1 1>/dev/tty)
if [ $? != 0 ]; then
	exit 1
fi

folder=$(dialog --keep-tite --inputbox "Output Folder Name" 0 $DIALOGWIDTH "debug" 2>&1 1>/dev/tty)
if [ $? != 0 ]; then
	exit 1
fi

compiler=$(dialog --keep-tite --inputbox "C++ Compiler" 0 $DIALOGWIDTH "g++" 2>&1 1>/dev/tty)
if [ $? != 0 ]; then
	exit 1
fi

compiler_flags=$(dialog --keep-tite --inputbox "C++ Compiler Flags" 0 $DIALOGWIDTH "-std=c++23 -g -Og -D DEBUG -MMD -MP -Wall" 2>&1 1>/dev/tty)
if [ $? != 0 ]; then
	exit 1
fi

imgui_flags=$(dialog --keep-tite --inputbox "C++ Compiler Flags for imgui" 0 $DIALOGWIDTH "-std=c++23 -O3 -MMD -MP" 2>&1 1>/dev/tty)
if [ $? != 0 ]; then
	exit 1
fi

linker=$(dialog --keep-tite --inputbox "Linker" 0 $DIALOGWIDTH "g++" 2>&1 1>/dev/tty)
if [ $? != 0 ]; then
	exit 1
fi

linker_flags=$(dialog --keep-tite --inputbox "Linker Flags" 0 $DIALOGWIDTH "-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi" 2>&1 1>/dev/tty)
if [ $? != 0 ]; then
	exit 1
fi

echo "# Custom Build Configuration" > $BUILDCONFIG
echo "" >> $BUILDCONFIG
echo "TARGET     := $target" >> $BUILDCONFIG
echo "BLDDIR     := $folder" >> $BUILDCONFIG
echo "" >> $BUILDCONFIG
echo "CPP        := $compiler" >> $BUILDCONFIG
echo "CPPFLAGS   := $compiler_flags" >> $BUILDCONFIG
echo "IMGUIFLAGS := $imgui_flags" >> $BUILDCONFIG
echo "LD         := $linker" >> $BUILDCONFIG
echo "LDFLAGS    := $linker_flags" >> $BUILDCONFIG
