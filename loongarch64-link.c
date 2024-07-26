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
	switch (reloc_type) {
	case R_LARCH_B26:
		return 1;

	case R_LARCH_GOT_PC_HI20:
	case R_LARCH_GOT_PC_LO12:
		return 0;
	}
	return -1;
}

/* Returns an enumerator to describe whether and when the relocation needs a
   GOT and/or PLT entry to be created. See tcc.h for a description of the
   different values. */
ST_FUNC int gotplt_entry_type(int reloc_type)
{
	switch (reloc_type) {
	case R_LARCH_32:
	case R_LARCH_64:
	case R_LARCH_ADD16:
	case R_LARCH_ADD32:
	case R_LARCH_ADD64:
	case R_LARCH_SUB16:
	case R_LARCH_SUB32:
	case R_LARCH_SUB64:
	case R_LARCH_32_PCREL:
	case R_LARCH_RELAX:
	case R_LARCH_ALIGN:
	case R_LARCH_ADD6:
	case R_LARCH_SUB6:
		return NO_GOTPLT_ENTRY;

	case R_LARCH_GOT_PC_HI20:
	case R_LARCH_GOT_PC_LO12:
		return BUILD_GOT_ONLY;

	case R_LARCH_B26:
	case R_LARCH_PCALA_HI20:
	case R_LARCH_PCALA_LO12:
	case R_LARCH_PCALA64_LO20:
	case R_LARCH_PCALA64_HI12:
		return AUTO_GOTPLT_ENTRY;
	}
	return -1;
}

ST_FUNC unsigned create_plt_entry(TCCState *s1, unsigned got_offset,
				  struct sym_attr *attr)
{
	Section *plt = s1->plt;
	uint8_t *p;
	unsigned plt_offset;

	if (plt->data_offset == 0)
		section_ptr_add(plt, 32);
	plt_offset = plt->data_offset;

	p = section_ptr_add(plt, 16);
	write64le(p, got_offset);
	return plt_offset;
}

#define check_hi20_truncate(off, addr, pc) \
	if ((off + ((uint32_t)1 << 20)) >> 21)				\
		tcc_error_noabort("PLT entry truncated (off = 0x%lx, "	\
				  "addr = 0x%lx at 0x%lx)",		\
				  (long)off, (long)addr, (long)pc);

/* relocate the PLT: compute addresses and offsets in the PLT now that final
   address for PLT and GOT are known (see fill_program_header) */
ST_FUNC void relocate_plt(TCCState *s1)
{
	uint8_t *p, *p_end;

	if (!s1->plt)
		return;

	p = s1->plt->data;
	p_end = p + s1->plt->data_offset;

/*
	pcaddu12i $t2, %pcrel_hi20(.got.plt)
	sub.[wd]  $t1, $t1, $t3
	ld.[wd]   $t3, $t2, %pcrel_lo12(.got.plt)  ; t3 = _dl_runtime_resolve
	addi.[wd] $t1, $t1, -pltHeaderSize-12      ; t1 = &.plt[i] - &.plt[0]
	addi.[wd] $t0, $t2, %pcrel_lo12(.got.plt)
	srli.[wd] $t1, $t1, (is64?1:2)             ; t1 = &.got.plt[i] -
						     &.got.plt[0]
	ld.[wd]   $t0, $t0, Wordsize               ; t0 = link_map
	jr        $t3
*/

	if (p < p_end) {
		uint64_t plt = s1->plt->sh_addr;
		uint64_t got = s1->got->sh_addr;
		uint64_t hioff = (got - plt + 0x800) >> 12;
		uint64_t looff = (got - plt) & 0xfff;

		check_hi20_truncate(hioff, got, plt);

		write32le(p, 0x1c00000e | (hioff << 5));
		write32le(p + 4, 0x0011bdad);
		write32le(p + 8, 0x28c00000 | (0xf << 0) | (0xe << 5) |
				 (looff << 10));
		write32le(p + 12, 0x02c00000 | (0xd << 0) | (0xd << 5) |
				  ((-32 - 12) & 0xfff) << 10);
		write32le(p + 16, 0x02c00000 | (0xc << 0) | (0xe << 5) |
				  (looff << 10));
		write32le(p + 20, 0x00450000 | (0xd << 0) | (0xd << 5) |
				  (2 << 10));
		write32le(p + 24, 0x28c00000 | (0xc << 0) | (0xc << 5) |
				  (8 << 10));
		write32le(p + 28, 0x4c000000 | (0x0 << 0) | (0xf << 5) |
				  (0 << 10));
		p += 32;

		while (p < p_end) {
			uint64_t pc = plt + (p - s1->plt->data);
			uint64_t addr = got + read64le(p);
			uint64_t hioff = (addr - pc + 0x800) >> 12;
			uint64_t looff = (addr - pc) & 0xfff;

			check_hi20_truncate(hioff, addr, pc);

			/*
				pcaddu12i	$t3, pcrel_hi20(func@got)
				ld.d		$t3, t3, pcrel_lo12(func@got)
				jirl		$t1, $t3, 0
				(nop)andi	$zero, $zero, 0
			 */
			write32le(p, 0x1c00000f | (hioff << 5));
			write32le(p + 4, 0x28c00000 | (0xf << 0) | (0xf << 5) |
					 (looff << 10));
			write32le(p + 8, 0x4c000000 | (0xc << 0) | (0xf << 5) |
				  (0 << 10));
			write32le(p + 12, 0x3400000 | (0x0 << 0) | (0x0 << 5) |
				  (0 << 10));
			p += 16;
		}
	}

	if (s1->plt->reloc) {
		ElfW_Rel *rel;
		p = s1->got->data;
		for_each_elem(s1->plt->reloc, 0, rel, ElfW_Rel)
			write64le(p + rel->r_offset, s1->plt->sh_addr);
	}
}

ST_FUNC void relocate(TCCState *s1, ElfW_Rel *rel, int type, unsigned char *ptr,
		      addr_t addr, addr_t val)
{
	uint64_t off64;
	uint32_t off32;
	uint32_t tmp;
	int sym_idx = ELFW(R_SYM)(rel->r_info);
	ElfW(Sym) *sym = &((ElfW(Sym) *)symtab_section->data)[sym_idx];

	switch (type) {
	case R_LARCH_32:
		write32le(ptr, val);
		return;
	case R_LARCH_64:
		write64le(ptr, val);
		return;
	case R_LARCH_ADD16:
		write16le(ptr, read16le(ptr) + val);
		return;
	case R_LARCH_ADD32:
		write32le(ptr, read32le(ptr) + val);
		return;
	case R_LARCH_ADD64:
		write64le(ptr, read64le(ptr) + val);
		return;
	case R_LARCH_SUB16:
		write16le(ptr, read16le(ptr) - val);
		return;
	case R_LARCH_SUB32:
		write32le(ptr, read32le(ptr) - val);
		return;
	case R_LARCH_SUB64:
		write64le(ptr, read64le(ptr) - val);
		return;
	case R_LARCH_B26:
		off64 = val - addr;
		// TODO: check overflow and alignment
		write32le(ptr, read32le(ptr) 		|
			       ((off64 >> 18) & 0x7ff)	|
			       ((off64 & 0x7fffc) << 8));
		return;
	case R_LARCH_PCALA_HI20:
		off32 = ((val + 0x800) & ~0xfff) - (addr & ~0xfff);
		write32le(ptr, read32le(ptr) | ((off32 >> 12) << 5));
		return;
	case R_LARCH_PCALA_LO12:
		write32le(ptr, read32le(ptr) | ((val & 0x7ff) << 10));
		return;
	case R_LARCH_PCALA64_LO20:
		off64 = val + 0x80000000;
		off64 += val & 0x800 ? (0x1000 - 0x100000000) : 0;
		off64 -= (addr - 8) & ~0xfff;
		write32le(ptr, read32le(ptr) |
			       (((off64 >> 32) & 0xfffff) << 5));
		return;
	case R_LARCH_PCALA64_HI12:
		off64 = val + 0x80000000;
		off64 += val & 0x800 ? (0x1000 - 0x100000000) : 0;
		off64 -= (addr - 12) & ~0xfff;
		write32le(ptr, read32le(ptr) |
			       ((off64 >> 52) << 10));
		return;
	case R_LARCH_32_PCREL:
		write32le(ptr, val - addr);
		return;
	case R_LARCH_RELAX:
	case R_LARCH_ALIGN:	// TODO: REALLY DO THE ALIGNMENT
		return;
	case R_LARCH_ADD6:
		*ptr += val & 0x3f;
		return;
	case R_LARCH_SUB6:
		*ptr -= val & 0x3f;
		return;
	default:
		fprintf(stderr, "FIXME: unhandled reloc type %d\n", type);
	}
}

#endif
