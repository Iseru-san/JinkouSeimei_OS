BITS 32
SECTION .text

MAGIC equ 0x1BADB002
FLAGS equ 0x00
CHECKSUM equ 0x100000000 - MAGIC - FLAGS

DD MAGIC
DD FLAGS
DD -(MAGIC + FLAGS)


;=================================== Импорт/экспорт функций
GLOBAL KernelLoader
EXTERN main
EXTERN end_of_kernel_code_pointer

;=================================== Запуск ядра ОС
KernelLoader:
	CLI
	MOV ESP, end_of_kernel_code_pointer
	CALL main
.end_of_work:
	HLT
	JMP .end_of_work