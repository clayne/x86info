/*
 *  $Id: powernow.c,v 1.5 2002/12/04 18:56:53 davej Exp $
 *  This file is part of x86info.
 *  (C) 2001 Dave Jones.
 *
 *  Licensed under the terms of the GNU GPL License version 2.
 *
 *  AMD-specific information
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "../x86info.h"
#include "AMD.h"
#include "powernow.h"

static double mobile_vid_table[32] = {
	2.000, 1.950, 1.900, 1.850, 1.800, 1.750, 1.700, 1.650,
	1.600, 1.550, 1.500, 1.450, 1.400, 1.350, 1.300, 0.000,
	1.275, 1.250, 1.225, 1.200, 1.175, 1.150, 1.125, 1.100,
	1.075, 1.050, 1.024, 1.000, 0.975, 0.950, 0.925, 0.000,
};

static double fid_codes[32] = {
	11.0, 11.5, 12.0, 12.5, 5.0, 5.5, 6.0, 6.5,
	7.0, 7.5, 8.0, 8.5, 9.0, 9.5, 10, 10.5,
	3.0, 19.0, 4.0, 20.0, 13.0, 13.5, 14.0, 21.0,
	15.0, 22.5, 16.0, 16.5, 17.0, 18.0, -1, -1,
};

void decode_powernow(struct cpudata *cpu)
{
	unsigned long eax, ebx, ecx, edx;
	struct msr_vidctl vidctl;
	struct msr_fidvidstatus fidvidstatus;

	if (cpu->maxei < 0x80000007)
		return;

	cpuid(cpu->number, 0x80000007, &eax, &ebx, &ecx, &edx);
	printf("PowerNOW! Technology information\n");
	printf("Available features:");
	if (edx & 1 << 0)
		printf("\n\tTemperature sensing diode present.");
	if (edx & 1 << 1)
		printf("\n\tBus divisor control");
	if (edx & 1 << 2)
		printf("\n\tVoltage ID control\n");
	if (!(edx & (1 << 0 | 1 << 1 | 1 << 2)))
		printf(" None\n");
	printf("\n");

	dumpmsr(cpu->number, MSR_FID_VID_CTL, 64);
	dumpmsr(cpu->number, MSR_FID_VID_STATUS, 64);
	printf("\n");

	if (read_msr(cpu->number, MSR_FID_VID_CTL, &vidctl.val) != 1) {
		printf ("Something went wrong reading MSR_FID_VID_CTL\n");
		return;
	}

	printf ("FID changes %s happen\n", vidctl.FIDC ? "will" : "won't");
	printf ("VID changes %s happen\n", vidctl.VIDC ? "will" : "won't");

	if (vidctl.VIDC)
		printf ("Current VID multiplier code: %0.3f\n", mobile_vid_table[vidctl.VID]);
	if (vidctl.FIDC)
		printf ("Current FSB multiplier code: %.1f\n", fid_codes[vidctl.FID]);

	/* Now dump the status */

	if (read_msr(cpu->number, MSR_FID_VID_STATUS, &fidvidstatus.val) != 1) {
		printf ("Something went wrong reading MSR_FID_VID_STATUS\n");
		return;
	}

	printf ("Voltage ID codes: Maximum=%0.3fV Startup=%0.3fV Currently=%0.3fV\n",
		mobile_vid_table[fidvidstatus.MVID],
		mobile_vid_table[fidvidstatus.SVID],
		mobile_vid_table[fidvidstatus.CVID]);

	printf ("Frequency ID codes: Maximum=%.1fx Startup=%.1fx Currently=%.1fx\n",
		fid_codes[fidvidstatus.MFID],
		fid_codes[fidvidstatus.SFID],
		fid_codes[fidvidstatus.CFID]);

//	printf ("Voltage ID codes: Maximum=0x%x Startup=0x%x Currently=0x%x\n",
//		fidvidstatus.MVID, fidvidstatus.SVID, fidvidstatus.CVID);
//	printf ("Frequency ID codes: Maximum=0x%x Startup=0x%x Currently=0x%x\n",
//		fidvidstatus.MFID, fidvidstatus.SFID, fidvidstatus.CFID);

}
