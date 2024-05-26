#SHELL=/usr/bin/bash

VPATH = src

LIBS=-lcups -lcupsimage

#DEFS=-DLOAD_LIBCUPS
#LIBS=-ldl

rastertohtrt:	rastertohtrt.o Util0.o HppCmd.o OneJob.o ClrCvt.o HTSharpen.o MapTable.o TableMaping.o CSaturation.o 
	g++ -o rastertohtrt rastertohtrt.o Util0.o HppCmd.o OneJob.o ClrCvt.o HTSharpen.o MapTable.o TableMaping.o CSaturation.o  $(LIBS)

clean:
	@rm *.o rastertohtrt

install:
	@mkdir -p /usr/lib/cups/filter
	@chmod +x rastertohtrt
	@cp rastertohtrt /usr/lib/cups/filter
	@mkdir -p /usr/share/cups/model/hiti
	@cp ./ppd/*.gz /usr/share/cups/model/hiti
	@mkdir -p /usr/share/HiTi
	@mkdir -p /usr/share/HiTi/tables
	@mkdir -p /usr/share/HiTi/tables/P52x
	@mkdir -p /usr/share/HiTi/tables/P720
	@mkdir -p /usr/share/HiTi/tables/P750
	@cp ./tables/P52x/*.bin /usr/share/HiTi/tables/P52x
	@cp ./tables/P720/*.bin /usr/share/HiTi/tables/P720
	@cp ./tables/P750/*.bin /usr/share/HiTi/tables/P750

uninstall:
	@rm /usr/lib/cups/filter/rastertohtrt
	@rm /usr/share/cups/model/hiti/*.gz
	@rm /usr/share/HiTi/tables/P52x/*.bin
	@rm /usr/share/HiTi/tables/P720/*.bin
	@rm /usr/share/HiTi/tables/P750/*.bin

