#!/bin/sh

make all && ./manager/gameManager < setting/setting > logfile
mv logfile /vagrant 


