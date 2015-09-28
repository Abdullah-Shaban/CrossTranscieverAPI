#!/bin/csh
#

./hardware/FWLoader -c -v 0x4b4 0x8613 -f -uu ./hardware/main_firmware.ihx
./hardware/FWLoader -d 0 -f -uf ./hardware/spider_fpga_lx45.v2_warp

