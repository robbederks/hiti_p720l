#!/bin/bash

sudo mkdir -p /usr/lib/cups/filter
sudo chmod +x ./rastertohtrt
sudo cp ./rastertohtrt /usr/lib/cups/filter
sudo mkdir -p /usr/share/cups/model/hiti
sudo cp ./ppd/*.gz /usr/share/cups/model/hiti
sudo mkdir -p /usr/share/HiTi
sudo mkdir -p /usr/share/HiTi/tables
sudo mkdir -p /usr/share/HiTi/tables/P52x
sudo mkdir -p /usr/share/HiTi/tables/P720
sudo mkdir -p /usr/share/HiTi/tables/P750
sudo cp ./tables/P52x/*.bin /usr/share/HiTi/tables/P52x
sudo cp ./tables/P720/*.bin /usr/share/HiTi/tables/P720
sudo cp ./tables/P750/*.bin /usr/share/HiTi/tables/P750
