ENTRY(KernelLoader)
SECTIONS{
	. = 1M;
	.text :{ *(.text)}
	.data :{ *(.data)}
	.bss  :{ *(.bss) }
	. = ALIGN(32);
	. = . + 16384;
	end_of_kernel_code_pointer = .;
}