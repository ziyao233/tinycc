/*************************************************************/
/*
 *  LoongArch64 assembler for TCC
 *
 */

#ifdef TARGET_DEFS_ONLY

#define CONFIG_TCC_ASM

#define NB_ASM_REGS 64

ST_FUNC void g(int c);
ST_FUNC void gen_le16(int c);
ST_FUNC void gen_le32(int c);

#else

#define USING_GLOBALS
#include "tcc.h"

/* XXX: make it faster ? */
ST_FUNC void g(int c)
{
    int ind1;
    if (nocode_wanted)
        return;
    ind1 = ind + 1;
    if (ind1 > cur_text_section->data_allocated)
        section_realloc(cur_text_section, ind1);
    cur_text_section->data[ind] = c;
    ind = ind1;
}

ST_FUNC void gen_le16 (int i)
{
    g(i);
    g(i>>8);
}

ST_FUNC void gen_le32 (int i)
{
    int ind1;
    if (nocode_wanted)
        return;
    ind1 = ind + 4;
    if (ind1 > cur_text_section->data_allocated)
        section_realloc(cur_text_section, ind1);
    cur_text_section->data[ind++] = i & 0xFF;
    cur_text_section->data[ind++] = (i >> 8) & 0xFF;
    cur_text_section->data[ind++] = (i >> 16) & 0xFF;
    cur_text_section->data[ind++] = (i >> 24) & 0xFF;
}

ST_FUNC void gen_expr32(ExprValue *pe)
{
	gen_le32(pe->v);
}

ST_FUNC void asm_opcode(TCCState *s1, int token)
{
	tcc_error("%s: not implemented", __func__);
}

ST_FUNC void asm_compute_constraints(ASMOperand *operands,
				     int nb_operands, int nb_outputs,
				     const uint8_t *clobber_regs,
				     int *pout_reg)
{
	tcc_error("%s: not implemented", __func__);
}

ST_FUNC void asm_clobber(uint8_t *clobber_regs, const char *str)
{
	tcc_error("%s: not implemented", __func__);
}

ST_FUNC int asm_parse_regvar(int t)
{
	tcc_error("%s: not implemented", __func__);
}

#endif /* ndef TARGETS_DEFS_ONLY */
