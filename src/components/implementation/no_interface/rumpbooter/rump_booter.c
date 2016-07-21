#include <stdio.h>
#include <string.h>
#include <cos_component.h>

#include <cobj_format.h>
#include <cos_kernel_api.h>

#include "rumpcalls.h"
#include "cos_init.h"

extern struct cos_compinfo booter_info;

void
hw_irq_alloc(void){

	int i;

	for(i = 1; i < HW_ISR_LINES; i++){
		irq_thdcap[i] = cos_thd_alloc(&booter_info, booter_info.comp_cap, cos_irqthd_handler, i);
		irq_thdid[i] = (thdid_t)cos_introspect(&booter_info, irq_thdcap[i], 9);
		irq_tcap[i] = cos_tcap_split(&booter_info, BOOT_CAPTBL_SELF_INITTCAP_BASE, 0, 0);
		irq_arcvcap[i] = cos_arcv_alloc(&booter_info, irq_thdcap[i], irq_tcap[i], booter_info.comp_cap, BOOT_CAPTBL_SELF_INITRCV_BASE);
		irq_isblocked[i] = 0;
		cos_hw_attach(BOOT_CAPTBL_SELF_INITHW_BASE, 32 + i, irq_arcvcap[i]);
	}
}

void
rump_booter_init(void)
{
	printc("\n%s\n", __func__);

	//char *json_file = "{,\"blk\":{,\"source\":\"dev\",\"path\":\"/dev/paws\",\"fstype\":\"cd9660\",\"mountpoint\":\"data\",},\"net\":{,\"if\":\"vioif0\",\"type\":\"inet\",\"method\":\"static\",\"addr\":\"10.0.120.101\",\"mask\":\"24\",},\"net\":{,\"if\":\"tun0\",\"type\":\"inet\",\"method\":\"static\",\"addr\":\"111.111.111.0\",\"mask\":\"24\",\"gw\":\"111.111.111.0\",},\"cmdline\":\"nginx.bin\",},\0";
	char *json_file = "{,\"net\":{,\"if\":\"vioif0\",\"type\":\"inet\",\"method\":\"static\",\"addr\":\"10.0.120.101\",\"mask\":\"24\",},\"cmdline\":\"paws.bin\",},\0";
	//char *json_file = "";

	printc("\nRumpKernel Boot Start.\n");
	cos2rump_setup();
	/* possibly pass in the name of the program here to see if that fixes the name bug */

	printc("\nSetting up arcv for hw irq\n");
	hw_irq_alloc();

	/* We pass in the json config string to the RK */
	printc("Json File:\n%s", json_file);
	cos_run(json_file);
	printc("\nRumpKernel Boot done.\n");

	cos_vm_exit();
	return;
}
