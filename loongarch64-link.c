#ifdef TARGET_DEFS_ONLY

#define EM_TCC_TARGET EM_LOONGARCH

#define R_DATA_32	R_LARCH_32
#define R_DATA_PTR	R_LARCH_64
#define R_JMP_SLOT	R_LARCH_JUMP_SLOT
#define R_GLOB_DAT	R_LARCH_64
#define R_COPY		R_LARCH_COPY
#define R_RELATIVE	R_LARCH_RELATIVE

#define R_NUM		R_LARCH_NUM

#define ELF_START_ADDR	0x120000000
#define ELF_PAGE_SIZE	0x4000

#define PCRELATIVE_DLLPLT 1
#define RELOCATE_DLLPLT 1

#else /* !TARGET_DEFS_ONLY */

//#define DEBUG_RELOC
#define USING_GLOBALS
#include "tcc.h"

/* Returns 1 for a code relocation, 0 for a data relocation. For unknown
   relocations, returns -1. */
ST_FUNC int code_reloc(int reloc_type)
{
	tcc_error("%s: todo", __func__);
	return -1;
}

/* Returns an enumerator to describe whether and when the relocation needs a
   GOT and/or PLT entry to be created. See tcc.h for a description of the
   different values. */
ST_FUNC int gotplt_entry_type(int reloc_type)
{
	tcc_error("%s: todo", __func__);
	return -1;
}

ST_FUNC unsigned create_plt_entry(TCCState *s1, unsigned got_offset,
				  struct sym_attr *attr)
{
	tcc_error("%s: todo", __func__);
	return 0;
}

/* relocate the PLT: compute addresses and offsets in the PLT now that final
   address for PLT and GOT are known (see fill_program_header) */
ST_FUNC void relocate_plt(TCCState *s1)
{
	tcc_error("%s: todo", __func__);
}

ST_FUNC void relocate(TCCState *s1, ElfW_Rel *rel, int type, unsigned char *ptr,
		      addr_t addr, addr_t val)
{
	tcc_error("%s: todo", __func__);
}

#endif
