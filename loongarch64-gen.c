#ifdef TARGET_DEFS_ONLY

/* registers available to the allocator */
/* $r4 - $r11 aka $a0 - $a7, $f0 - $f7 aka $fa0 - $fa7 */
#define NB_REGS		16

#define TREG_R(x)	(x)
#define TREG_F(x)	(x + 10)

#define RC_INT		(1 << 0)
#define RC_FLOAT	(1 << 1)
#define RC_R(x)		(1 << (2 + (x)))
#define RC_F(x)		(1 << (10 + (x)))

#define RC_IRET		(RC_R(0))	/* int return register 1 class */
#define RC_IRE2		(RC_R(1))	/* int return register 2 class */
#define RC_FRET		(RC_F(0))	/* float return register class */

#define REG_IRET	(TREG_R(0))	/* int return register 1 number */
#define REG_IRE2	(TREG_R(1))	/* int return register 2 number */
#define REG_FRET	(TREG_F(0))	/* float return register number */

#define PTR_SIZE	8

#define LDOUBLE_SIZE	16
#define LDOUBLE_ALIGN	16

#define MAX_ALIGN	16

/* we have signed chars */

#else /* !TARGET_DEFS_ONLY */

#define USING_GLOBALS
#include "tcc.h"

ST_DATA const char * const target_machine_defs =
    "__loongarch64\0";

ST_DATA const int reg_classes[NB_REGS] = {
  RC_INT | RC_R(0),
  RC_INT | RC_R(1),
  RC_INT | RC_R(2),
  RC_INT | RC_R(3),
  RC_INT | RC_R(4),
  RC_INT | RC_R(5),
  RC_INT | RC_R(6),
  RC_INT | RC_R(7),
  RC_FLOAT | RC_F(0),
  RC_FLOAT | RC_F(1),
  RC_FLOAT | RC_F(2),
  RC_FLOAT | RC_F(3),
  RC_FLOAT | RC_F(4),
  RC_FLOAT | RC_F(5),
  RC_FLOAT | RC_F(6),
  RC_FLOAT | RC_F(7)
};

// Patch all branches in list pointed to by t to branch to a:
ST_FUNC void gsym_addr(int t_, int a_)
{
    uint32_t t = t_;
    uint32_t a = a_;
    tcc_error("%s: todo", __func__);
    while (t) {
        unsigned char *ptr = cur_text_section->data + t;
        uint32_t next = read32le(ptr);
        if (a - t + 0x8000000 >= 0x10000000)
            tcc_error("branch out of range");
        write32le(ptr, (a - t == 4 ? 0xd503201f : // nop
                        0x14000000 | ((a - t) >> 2 & 0x3ffffff))); // b
        t = next;
    }
}

ST_FUNC void load(int r, SValue *sv)
{
	tcc_error("%s: todo", __func__);
}

ST_FUNC void store(int r, SValue *sv)
{
	tcc_error("%s: todo", __func__);

}

ST_FUNC void gfunc_call(int nb_args)
{
	tcc_error("%s: todo", __func__);

}

ST_FUNC void gfunc_prolog(Sym *func_sym)
{
	tcc_error("%s: todo", __func__);

}

ST_FUNC void gen_va_start(void)
{
	tcc_error("%s: todo", __func__);

}

ST_FUNC void gen_va_arg(CType *t)
{
	tcc_error("%s: todo", __func__);

}

ST_FUNC int gfunc_sret(CType *vt, int variadic, CType *ret, int *align,
		       int *regsize)
{
	tcc_error("%s: todo", __func__);
	return 0;
}

ST_FUNC void gfunc_epilog(void)
{
	tcc_error("%s: todo", __func__);
}

ST_FUNC void gen_fill_nops(int bytes)
{
	tcc_error("%s: todo", __func__);
}

// generate forward branch to a label
ST_FUNC int gjmp(int t)
{
	tcc_error("%s: todo", __func__);
	return 0;
}

// generate branch to known address
ST_FUNC void gjmp_addr(int a)
{
	tcc_error("%s: todo", __func__);
}

ST_FUNC int gjmp_cond(int op, int t)
{
	tcc_error("%s: todo", __func__);
	return 0;
}

ST_FUNC int gjmp_append(int n, int t)
{
	tcc_error("%s: todo", __func__);
	return 0;
}

ST_FUNC int gtst(int inv, int t)
{
	tcc_error("%s: todo", __func__);
	return 0;
}
ST_FUNC void gen_opi(int op)
{
	tcc_error("%s: todo", __func__);
}

ST_FUNC void gen_opl(int op)
{
	tcc_error("%s: todo", __func__);
}

ST_FUNC void gen_opf(int op)
{
	tcc_error("%s: todo", __func__);
}

ST_FUNC void gen_cvt_sxtw(void)
{
	tcc_error("%s: todo", __func__);
}

ST_FUNC void gen_cvt_itof(int t)
{
	tcc_error("%s: todo", __func__);
}

ST_FUNC void gen_cvt_ftoi(int t)
{
	tcc_error("%s: todo", __func__);
}

ST_FUNC void gen_cvt_ftof(int t)
{
	tcc_error("%s: todo", __func__);
}

ST_FUNC void ggoto(void)
{
	tcc_error("%s: todo", __func__);
}

ST_FUNC void gen_vla_sp_save(int addr)
{
	tcc_error("%s: todo", __func__);
}

ST_FUNC void gen_vla_sp_restore(int addr)
{
	tcc_error("%s: todo", __func__);
}

ST_FUNC void gen_vla_alloc(CType *type, int align)
{
	tcc_error("%s: todo", __func__);
}
#endif
