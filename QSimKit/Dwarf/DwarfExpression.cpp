/**
 * QSimKit - MSP430 simulator
 * Copyright (C) 2013 Jan "HanzZ" Kaluza (hanzz.k@gmail.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 **/

/**
 * DwarfExpression::getValue(...) is inspired by Wine code:
 * File dwarf.c - read dwarf2 information from the ELF modules
 *
 * Copyright (C) 2005, Raphael Junqueira
 * Copyright (C) 2006-2011, Eric Pouech
 * Copyright (C) 2010, Alexandre Julliard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 **/

#include "DwarfExpression.h"
#include "DwarfSubprogram.h"
#include "QSimKit/MCU/RegisterSet.h"
#include "QSimKit/MCU/Register.h"
#include "QSimKit/MCU/Memory.h"

#include "dwarf.h"

#include <QFile>
#include <QProcess>
#include <QDir>
#include <QDebug>

#include "QSimKit/MCU/MCU.h"

DwarfExpression::DwarfExpression(const QString &expression) {
	parse(expression);
}

DwarfExpression::~DwarfExpression() {
	
}

DwarfExpression::Instruction DwarfExpression::getInstruction(const QString &expr) {
	Instruction inst;

	if (expr.startsWith("DW_OP_addr")) inst.op = DW_OP_addr;
	else if (expr.startsWith("DW_OP_const1u")) inst.op = DW_OP_const1u;
	else if (expr.startsWith("DW_OP_const1s")) inst.op = DW_OP_const1s;
	else if (expr.startsWith("DW_OP_const2u")) inst.op = DW_OP_const2u;
	else if (expr.startsWith("DW_OP_const2s")) inst.op = DW_OP_const2s;
	else if (expr.startsWith("DW_OP_const4u")) inst.op = DW_OP_const4u;
	else if (expr.startsWith("DW_OP_const4s")) inst.op = DW_OP_const4s;
	else if (expr.startsWith("DW_OP_const8u")) inst.op = DW_OP_const8u;
	else if (expr.startsWith("DW_OP_const8s")) inst.op = DW_OP_const8s;
	else if (expr.startsWith("DW_OP_constu")) inst.op = DW_OP_constu;
	else if (expr.startsWith("DW_OP_consts")) inst.op = DW_OP_consts;
	else if (expr.startsWith("DW_OP_dup")) inst.op = DW_OP_dup;
	else if (expr.startsWith("DW_OP_drop")) inst.op = DW_OP_drop;
	else if (expr.startsWith("DW_OP_over")) inst.op = DW_OP_over;
	else if (expr.startsWith("DW_OP_pick")) inst.op = DW_OP_pick;
	else if (expr.startsWith("DW_OP_swap")) inst.op = DW_OP_swap;
	else if (expr.startsWith("DW_OP_rot")) inst.op = DW_OP_rot;
	else if (expr.startsWith("DW_OP_xderef")) inst.op = DW_OP_xderef;
	else if (expr.startsWith("DW_OP_abs")) inst.op = DW_OP_abs;
	else if (expr.startsWith("DW_OP_and")) inst.op = DW_OP_and;
	else if (expr.startsWith("DW_OP_div")) inst.op = DW_OP_div;
	else if (expr.startsWith("DW_OP_minus")) inst.op = DW_OP_minus;
	else if (expr.startsWith("DW_OP_mod")) inst.op = DW_OP_mod;
	else if (expr.startsWith("DW_OP_mul")) inst.op = DW_OP_mul;
	else if (expr.startsWith("DW_OP_neg")) inst.op = DW_OP_neg;
	else if (expr.startsWith("DW_OP_not")) inst.op = DW_OP_not;
	else if (expr.startsWith("DW_OP_or")) inst.op = DW_OP_or;
	else if (expr.startsWith("DW_OP_plus")) inst.op = DW_OP_plus;
	else if (expr.startsWith("DW_OP_plus_uconst")) inst.op = DW_OP_plus_uconst;
	else if (expr.startsWith("DW_OP_shl")) inst.op = DW_OP_shl;
	else if (expr.startsWith("DW_OP_shr")) inst.op = DW_OP_shr;
	else if (expr.startsWith("DW_OP_shra")) inst.op = DW_OP_shra;
	else if (expr.startsWith("DW_OP_xor")) inst.op = DW_OP_xor;
	else if (expr.startsWith("DW_OP_bra")) inst.op = DW_OP_bra;
	else if (expr.startsWith("DW_OP_eq")) inst.op = DW_OP_eq;
	if (expr.startsWith("DW_OP_ge")) inst.op = DW_OP_ge;
	else if (expr.startsWith("DW_OP_gt")) inst.op = DW_OP_gt;
	else if (expr.startsWith("DW_OP_le")) inst.op = DW_OP_le;
	else if (expr.startsWith("DW_OP_lt")) inst.op = DW_OP_lt;
	else if (expr.startsWith("DW_OP_ne")) inst.op = DW_OP_ne;
	else if (expr.startsWith("DW_OP_skip")) inst.op = DW_OP_skip;
	else if (expr.startsWith("DW_OP_lit10")) inst.op = DW_OP_lit10;
	else if (expr.startsWith("DW_OP_lit11")) inst.op = DW_OP_lit11;
	else if (expr.startsWith("DW_OP_lit12")) inst.op = DW_OP_lit12;
	else if (expr.startsWith("DW_OP_lit13")) inst.op = DW_OP_lit13;
	else if (expr.startsWith("DW_OP_lit14")) inst.op = DW_OP_lit14;
	else if (expr.startsWith("DW_OP_lit15")) inst.op = DW_OP_lit15;
	else if (expr.startsWith("DW_OP_lit16")) inst.op = DW_OP_lit16;
	else if (expr.startsWith("DW_OP_lit17")) inst.op = DW_OP_lit17;
	else if (expr.startsWith("DW_OP_lit18")) inst.op = DW_OP_lit18;
	else if (expr.startsWith("DW_OP_lit19")) inst.op = DW_OP_lit19;
	else if (expr.startsWith("DW_OP_lit20")) inst.op = DW_OP_lit20;
	else if (expr.startsWith("DW_OP_lit21")) inst.op = DW_OP_lit21;
	else if (expr.startsWith("DW_OP_lit22")) inst.op = DW_OP_lit22;
	else if (expr.startsWith("DW_OP_lit23")) inst.op = DW_OP_lit23;
	else if (expr.startsWith("DW_OP_lit24")) inst.op = DW_OP_lit24;
	else if (expr.startsWith("DW_OP_lit25")) inst.op = DW_OP_lit25;
	else if (expr.startsWith("DW_OP_lit26")) inst.op = DW_OP_lit26;
	else if (expr.startsWith("DW_OP_lit27")) inst.op = DW_OP_lit27;
	else if (expr.startsWith("DW_OP_lit28")) inst.op = DW_OP_lit28;
	else if (expr.startsWith("DW_OP_lit29")) inst.op = DW_OP_lit29;
	else if (expr.startsWith("DW_OP_lit30")) inst.op = DW_OP_lit30;
	else if (expr.startsWith("DW_OP_lit31")) inst.op = DW_OP_lit31;
	else if (expr.startsWith("DW_OP_lit0")) inst.op = DW_OP_lit0;
	else if (expr.startsWith("DW_OP_lit1")) inst.op = DW_OP_lit1;
	else if (expr.startsWith("DW_OP_lit2")) inst.op = DW_OP_lit2;
	else if (expr.startsWith("DW_OP_lit3")) inst.op = DW_OP_lit3;
	else if (expr.startsWith("DW_OP_lit4")) inst.op = DW_OP_lit4;
	else if (expr.startsWith("DW_OP_lit5")) inst.op = DW_OP_lit5;
	else if (expr.startsWith("DW_OP_lit6")) inst.op = DW_OP_lit6;
	else if (expr.startsWith("DW_OP_lit7")) inst.op = DW_OP_lit7;
	else if (expr.startsWith("DW_OP_lit8")) inst.op = DW_OP_lit8;
	else if (expr.startsWith("DW_OP_lit9")) inst.op = DW_OP_lit9;
	else if (expr.startsWith("DW_OP_reg10")) inst.op = DW_OP_reg10;
	else if (expr.startsWith("DW_OP_reg11")) inst.op = DW_OP_reg11;
	else if (expr.startsWith("DW_OP_reg12")) inst.op = DW_OP_reg12;
	else if (expr.startsWith("DW_OP_reg13")) inst.op = DW_OP_reg13;
	else if (expr.startsWith("DW_OP_reg14")) inst.op = DW_OP_reg14;
	else if (expr.startsWith("DW_OP_reg15")) inst.op = DW_OP_reg15;
	else if (expr.startsWith("DW_OP_reg16")) inst.op = DW_OP_reg16;
	else if (expr.startsWith("DW_OP_reg17")) inst.op = DW_OP_reg17;
	else if (expr.startsWith("DW_OP_reg18")) inst.op = DW_OP_reg18;
	else if (expr.startsWith("DW_OP_reg19")) inst.op = DW_OP_reg19;
	else if (expr.startsWith("DW_OP_reg20")) inst.op = DW_OP_reg20;
	else if (expr.startsWith("DW_OP_reg21")) inst.op = DW_OP_reg21;
	else if (expr.startsWith("DW_OP_reg22")) inst.op = DW_OP_reg22;
	else if (expr.startsWith("DW_OP_reg23")) inst.op = DW_OP_reg23;
	else if (expr.startsWith("DW_OP_reg24")) inst.op = DW_OP_reg24;
	else if (expr.startsWith("DW_OP_reg25")) inst.op = DW_OP_reg25;
	else if (expr.startsWith("DW_OP_reg26")) inst.op = DW_OP_reg26;
	else if (expr.startsWith("DW_OP_reg27")) inst.op = DW_OP_reg27;
	else if (expr.startsWith("DW_OP_reg28")) inst.op = DW_OP_reg28;
	else if (expr.startsWith("DW_OP_reg29")) inst.op = DW_OP_reg29;
	else if (expr.startsWith("DW_OP_reg30")) inst.op = DW_OP_reg30;
	if (expr.startsWith("DW_OP_reg31")) inst.op = DW_OP_reg31;
	else if (expr.startsWith("DW_OP_reg0")) inst.op = DW_OP_reg0;
	else if (expr.startsWith("DW_OP_reg1")) inst.op = DW_OP_reg1;
	else if (expr.startsWith("DW_OP_reg2")) inst.op = DW_OP_reg2;
	else if (expr.startsWith("DW_OP_reg3")) inst.op = DW_OP_reg3;
	else if (expr.startsWith("DW_OP_reg4")) inst.op = DW_OP_reg4;
	else if (expr.startsWith("DW_OP_reg5")) inst.op = DW_OP_reg5;
	else if (expr.startsWith("DW_OP_reg6")) inst.op = DW_OP_reg6;
	else if (expr.startsWith("DW_OP_reg7")) inst.op = DW_OP_reg7;
	else if (expr.startsWith("DW_OP_reg8")) inst.op = DW_OP_reg8;
	else if (expr.startsWith("DW_OP_reg9")) inst.op = DW_OP_reg9;
	else if (expr.startsWith("DW_OP_breg10")) inst.op = DW_OP_breg10;
	else if (expr.startsWith("DW_OP_breg11")) inst.op = DW_OP_breg11;
	else if (expr.startsWith("DW_OP_breg12")) inst.op = DW_OP_breg12;
	else if (expr.startsWith("DW_OP_breg13")) inst.op = DW_OP_breg13;
	else if (expr.startsWith("DW_OP_breg14")) inst.op = DW_OP_breg14;
	else if (expr.startsWith("DW_OP_breg15")) inst.op = DW_OP_breg15;
	else if (expr.startsWith("DW_OP_breg16")) inst.op = DW_OP_breg16;
	else if (expr.startsWith("DW_OP_breg17")) inst.op = DW_OP_breg17;
	else if (expr.startsWith("DW_OP_breg18")) inst.op = DW_OP_breg18;
	else if (expr.startsWith("DW_OP_breg19")) inst.op = DW_OP_breg19;
	else if (expr.startsWith("DW_OP_breg20")) inst.op = DW_OP_breg20;
	else if (expr.startsWith("DW_OP_breg21")) inst.op = DW_OP_breg21;
	else if (expr.startsWith("DW_OP_breg22")) inst.op = DW_OP_breg22;
	else if (expr.startsWith("DW_OP_breg23")) inst.op = DW_OP_breg23;
	else if (expr.startsWith("DW_OP_breg24")) inst.op = DW_OP_breg24;
	else if (expr.startsWith("DW_OP_breg25")) inst.op = DW_OP_breg25;
	else if (expr.startsWith("DW_OP_breg26")) inst.op = DW_OP_breg26;
	else if (expr.startsWith("DW_OP_breg27")) inst.op = DW_OP_breg27;
	else if (expr.startsWith("DW_OP_breg28")) inst.op = DW_OP_breg28;
	else if (expr.startsWith("DW_OP_breg29")) inst.op = DW_OP_breg29;
	else if (expr.startsWith("DW_OP_breg30")) inst.op = DW_OP_breg30;
	else if (expr.startsWith("DW_OP_breg31")) inst.op = DW_OP_breg31;
	else if (expr.startsWith("DW_OP_breg0")) inst.op = DW_OP_breg0;
	else if (expr.startsWith("DW_OP_breg1")) inst.op = DW_OP_breg1;
	else if (expr.startsWith("DW_OP_breg2")) inst.op = DW_OP_breg2;
	else if (expr.startsWith("DW_OP_breg3")) inst.op = DW_OP_breg3;
	else if (expr.startsWith("DW_OP_breg4")) inst.op = DW_OP_breg4;
	else if (expr.startsWith("DW_OP_breg5")) inst.op = DW_OP_breg5;
	else if (expr.startsWith("DW_OP_breg6")) inst.op = DW_OP_breg6;
	else if (expr.startsWith("DW_OP_breg7")) inst.op = DW_OP_breg7;
	else if (expr.startsWith("DW_OP_breg8")) inst.op = DW_OP_breg8;
	else if (expr.startsWith("DW_OP_breg9")) inst.op = DW_OP_breg9;
	else if (expr.startsWith("DW_OP_regx")) inst.op = DW_OP_regx;
	else if (expr.startsWith("DW_OP_fbreg")) inst.op = DW_OP_fbreg;
	else if (expr.startsWith("DW_OP_bregx")) inst.op = DW_OP_bregx;
	if (expr.startsWith("DW_OP_piece")) inst.op = DW_OP_piece;
	else if (expr.startsWith("DW_OP_deref_size")) inst.op = DW_OP_deref_size;
	else if (expr.startsWith("DW_OP_xderef_size")) inst.op = DW_OP_xderef_size;
	else if (expr.startsWith("DW_OP_nop")) inst.op = DW_OP_nop;
	else if (expr.startsWith("DW_OP_push_object_address")) inst.op = DW_OP_push_object_address;
	else if (expr.startsWith("DW_OP_call2")) inst.op = DW_OP_call2;
	else if (expr.startsWith("DW_OP_call4")) inst.op = DW_OP_call4;
	else if (expr.startsWith("DW_OP_call_ref")) inst.op = DW_OP_call_ref;
	else if (expr.startsWith("DW_OP_form_tls_address")) inst.op = DW_OP_form_tls_address;
	else if (expr.startsWith("DW_OP_call_frame_cfa")) inst.op = DW_OP_call_frame_cfa;
	else if (expr.startsWith("DW_OP_bit_piece")) inst.op = DW_OP_bit_piece;
	else if (expr.startsWith("DW_OP_implicit_value")) inst.op = DW_OP_implicit_value;
	else if (expr.startsWith("DW_OP_stack_value")) inst.op = DW_OP_stack_value;
	else if (expr.startsWith("DW_OP_deref")) inst.op = DW_OP_deref;
// 	else { qDebug() << "UNKNOWN OP" << expr; }

	int i = expr.lastIndexOf(':');
	if (i > 0) {
		QString str = expr.mid(i + 1).trimmed().remove(')');
		bool ok;
		inst.arg = (uint16_t) str.toInt(&ok);
		if (!ok) {
			qDebug() << "Cannot convert " << str;
		}
	}
	else {
		inst.arg = 0;
	}

	return inst;
}

bool DwarfExpression::parse(const QString &expression) {
	// "DW_OP_breg13 (r13): 0; DW_OP_breg14 (r14): 0; DW_OP_or; DW_OP_stack_value"
	// "DW_OP_reg15 (r15)"

	QStringList cmds = expression.split(";", QString::SkipEmptyParts);
	foreach(const QString &cmd, cmds) {
		Instruction inst = getInstruction(cmd.trimmed());
		m_instructions.append(inst);
	}

	return true;
}

uint16_t DwarfExpression::getValue(RegisterSet *r, Memory *m, DwarfSubprogram *s, uint16_t pc, bool &isAddress) {
	unsigned long tmp, stack[64];
	unsigned sp = 0;

	isAddress = true;
	foreach(const Instruction &inst, m_instructions) {
		unsigned char opcode = inst.op;

		qDebug() << "EXPR OP =" << opcode;

		if (opcode >= DW_OP_lit0 && opcode <= DW_OP_lit31) {
			stack[++sp] = opcode - DW_OP_lit0;
			isAddress = false;
		}
		else if (opcode >= DW_OP_reg0 && opcode <= DW_OP_reg31) {
			stack[++sp] = r->get(opcode - DW_OP_reg0)->getBigEndian();
			isAddress = false;
		}
		else if (opcode >= DW_OP_breg0 && opcode <= DW_OP_breg31) {
			stack[++sp] = r->get(opcode - DW_OP_breg0)->getBigEndian() + inst.arg;
			isAddress = true;
		}
		else switch (opcode)
		{
		case DW_OP_nop: break;
		case DW_OP_addr: stack[++sp] = inst.arg; break;
		case DW_OP_const1u: stack[++sp] = inst.arg; break;
		case DW_OP_const1s: stack[++sp] = (signed char)inst.arg; break;
		case DW_OP_const2u: stack[++sp] = inst.arg; break;
		case DW_OP_const2s: stack[++sp] = (short)inst.arg; break;
		case DW_OP_const4u: stack[++sp] = inst.arg; break;
		case DW_OP_const4s: stack[++sp] = (signed int)inst.arg; break;
		case DW_OP_const8u: stack[++sp] = inst.arg; break;
		case DW_OP_const8s: stack[++sp] = inst.arg; break;
		case DW_OP_constu: stack[++sp] = inst.arg; break;
		case DW_OP_consts: stack[++sp] = inst.arg; break;
		case DW_OP_deref:
			isAddress = false;
			stack[sp] = m->getBigEndian(stack[sp], false);
			break;
		case DW_OP_dup: stack[sp + 1] = stack[sp]; sp++; break;
		case DW_OP_drop: sp--; break;
		case DW_OP_over: stack[sp + 1] = stack[sp - 1]; sp++; break;
		case DW_OP_pick: stack[sp + 1] = stack[sp - inst.arg]; sp++; break;
		case DW_OP_swap: tmp = stack[sp]; stack[sp] = stack[sp-1]; stack[sp-1] = tmp; break;
		case DW_OP_rot: tmp = stack[sp]; stack[sp] = stack[sp-1]; stack[sp-1] = stack[sp-2]; stack[sp-2] = tmp; break;
		case DW_OP_abs: stack[sp] = labs(stack[sp]); break;
		case DW_OP_neg: stack[sp] = -stack[sp]; break;
		case DW_OP_not: stack[sp] = ~stack[sp]; break;
		case DW_OP_and: stack[sp-1] &= stack[sp]; sp--; break;
		case DW_OP_or: stack[sp-1] |= stack[sp]; sp--; break;
		case DW_OP_minus: stack[sp-1] -= stack[sp]; sp--; break;
		case DW_OP_mul: stack[sp-1] *= stack[sp]; sp--; break;
		case DW_OP_plus: stack[sp-1] += stack[sp]; sp--; break;
		case DW_OP_xor: stack[sp-1] ^= stack[sp]; sp--; break;
		case DW_OP_shl: stack[sp-1] <<= stack[sp]; sp--; break;
		case DW_OP_shr: stack[sp-1] >>= stack[sp]; sp--; break;
		case DW_OP_plus_uconst: stack[sp] += inst.arg; break;
		case DW_OP_shra: stack[sp-1] = (long)stack[sp-1] / (1 << stack[sp]); sp--; break;
		case DW_OP_div: stack[sp-1] = (long)stack[sp-1] / (long)stack[sp]; sp--; break;
		case DW_OP_mod: stack[sp-1] = (long)stack[sp-1] % (long)stack[sp]; sp--; break;
		case DW_OP_ge: stack[sp-1] = ((long)stack[sp-1] >= (long)stack[sp]); sp--; break;
		case DW_OP_gt: stack[sp-1] = ((long)stack[sp-1] > (long)stack[sp]); sp--; break;
		case DW_OP_le: stack[sp-1] = ((long)stack[sp-1] <= (long)stack[sp]); sp--; break;
		case DW_OP_lt: stack[sp-1] = ((long)stack[sp-1] < (long)stack[sp]); sp--; break;
		case DW_OP_eq: stack[sp-1] = (stack[sp-1] == stack[sp]); sp--; break;
		case DW_OP_ne: stack[sp-1] = (stack[sp-1] != stack[sp]); sp--; break;
// 		case DW_OP_skip: tmp = (short)inst.arg; ctx.data += tmp; break;
// 		case DW_OP_bra: tmp = (short)dwarf2_parse_u2(&ctx); if (!stack[sp--]) ctx.data += tmp; break;
// 		case DW_OP_GNU_encoded_addr:
// 			tmp = dwarf2_parse_byte(&ctx);
// 			stack[++sp] = dwarf2_parse_augmentation_ptr(&ctx, tmp);
// 			break;
		case DW_OP_regx:
			isAddress = false;
			stack[++sp] = r->get(inst.arg)->getBigEndian();
			break;
		case DW_OP_bregx:
			isAddress = true;
			stack[++sp] = r->get(inst.arg)->getBigEndian() + inst.arg2;
			break;
		case DW_OP_fbreg:
			isAddress = true;
			stack[++sp] = s->getFrameBase(r, m, pc) + inst.arg;
			break;
// TODO
// 		case DW_OP_deref_size:
// 			sz = dwarf2_parse_byte(&ctx);
// 			if (!sw_read_mem(csw, stack[sp], &tmp, sz))
// 			{
// 				ERR("Couldn't read memory at %lx\n", stack[sp]);
// 				tmp = 0;
// 			}
// 			/* do integral promotion */
// 			switch (sz)
// 			{
// 			case 1: stack[sp] = *(unsigned char*)&tmp; break;
// 			case 2: stack[sp] = *(unsigned short*)&tmp; break;
// 			case 4: stack[sp] = *(unsigned int*)&tmp; break;
// 			case 8: stack[sp] = *(Ulong*)&tmp; break; /* FIXME: won't work on 32bit platform */
// 			default: FIXME("Unknown size for deref 0x%lx\n", sz);
// 			}
// 			break;
		default:
			qDebug() << "unhandled opcode " << opcode;
		}
	}

	return stack[sp];

}
