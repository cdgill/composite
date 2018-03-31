# cp llboot_comp.o llboot.o
# cp resmgr.o mm.o
# ./cos_linker 'llboot.o, ;mm.o, ;*cFE_booter.o, ;sample_lib.o, ;sample_app.o, ;sch_lab.o, :cFE_booter.o-mm.o;sample_app.o-cFE_booter.o;sample_lib.o-cFE_booter.o;sch_lab.o-cFE_booter.o' ./gen_client_stub

cp llboot_comp.o llboot.o
cp cFE_booter.o boot.o
./cos_linker 'llboot.o, ;sample_lib.o, ;capmgr.o, ;sample_app.o, ;*boot.o, ;sch_lab.o, :boot.o-capmgr.o;sample_app.o-boot.o;sample_lib.o-boot.o;sch_lab.o-boot.o;sample_app.o-capmgr.o;sample_lib.o-capmgr.o;sch_lab.o-capmgr.o' ./gen_client_stub
