xgettext --force-po -C -T --no-location --no-wrap -d escomm-gui -k_ -k_i -o locale/escomm-gui.SRC.pot *.cpp *.h
msgcat -s --no-wrap --no-location -o locale/escomm-gui.pot locale/escomm-gui.SRC.pot locale/escomm-gui.GUI.pot locale/escomm-gui.MAN.pot