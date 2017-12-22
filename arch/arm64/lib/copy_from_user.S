/*
 * Copyright (C) 2012 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/linkage.h>

<<<<<<< HEAD
#include <asm/alternative.h>
#include <asm/assembler.h>
#include <asm/cache.h>
#include <asm/cpufeature.h>
#include <asm/sysreg.h>
=======
#include <asm/cache.h>
#include <asm/uaccess.h>
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

/*
 * Copy from user space to a kernel buffer (alignment handled by the hardware)
 *
 * Parameters:
 *	x0 - to
 *	x1 - from
 *	x2 - n
 * Returns:
 *	x0 - bytes not copied
 */

	.macro ldrb1 ptr, regB, val
<<<<<<< HEAD
	USER(9998f, ldrb  \ptr, [\regB], \val)
=======
	uao_user_alternative 9998f, ldrb, ldtrb, \ptr, \regB, \val
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	.endm

	.macro strb1 ptr, regB, val
	strb \ptr, [\regB], \val
	.endm

	.macro ldrh1 ptr, regB, val
<<<<<<< HEAD
	USER(9998f, ldrh  \ptr, [\regB], \val)
=======
	uao_user_alternative 9998f, ldrh, ldtrh, \ptr, \regB, \val
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	.endm

	.macro strh1 ptr, regB, val
	strh \ptr, [\regB], \val
	.endm

	.macro ldr1 ptr, regB, val
<<<<<<< HEAD
	USER(9998f, ldr \ptr, [\regB], \val)
=======
	uao_user_alternative 9998f, ldr, ldtr, \ptr, \regB, \val
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	.endm

	.macro str1 ptr, regB, val
	str \ptr, [\regB], \val
	.endm

	.macro ldp1 ptr, regB, regC, val
<<<<<<< HEAD
	USER(9998f, ldp \ptr, \regB, [\regC], \val)
=======
	uao_ldp 9998f, \ptr, \regB, \regC, \val
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	.endm

	.macro stp1 ptr, regB, regC, val
	stp \ptr, \regB, [\regC], \val
	.endm

end	.req	x5
<<<<<<< HEAD
ENTRY(__copy_from_user)
ALTERNATIVE("nop", __stringify(SET_PSTATE_PAN(0)), ARM64_HAS_PAN, \
	    CONFIG_ARM64_PAN)
	add	end, x0, x2
#include "copy_template.S"
ALTERNATIVE("nop", __stringify(SET_PSTATE_PAN(1)), ARM64_HAS_PAN, \
	    CONFIG_ARM64_PAN)
	mov	x0, #0				// Nothing to copy
	ret
ENDPROC(__copy_from_user)
=======
ENTRY(__arch_copy_from_user)
	uaccess_enable_not_uao x3, x4
	add	end, x0, x2
#include "copy_template.S"
	uaccess_disable_not_uao x3
	mov	x0, #0				// Nothing to copy
	ret
ENDPROC(__arch_copy_from_user)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	.section .fixup,"ax"
	.align	2
9998:
	sub	x0, end, dst
9999:
	strb	wzr, [dst], #1			// zero remaining buffer space
	cmp	dst, end
	b.lo	9999b
	ret
	.previous
