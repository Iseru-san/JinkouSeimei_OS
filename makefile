OUTPUT_FILE=JinkouSeimei-0.0.1
BUILD_PATH=build
HDDIMG=disk.img



FILES_C+=$(wildcard *.c)
FILES_C+=$(wildcard */*.c)
FILES_NASM=$(wildcard *.asm)
FILES_NASM+=$(wildcard */*.asm)

all:
	@echo "/============================================================\\"
	@echo "|                Проект \"Синтетическая жизнь\"                |"
	@echo "|      Операционная система \"Синтетическая жизнь\" это часть  |"
	@echo "|  одноимённого проекта.                                     |"
	@echo "|      Доступные команды make представлены в таблице 1.      |"
	@echo "|                                  Таблица 1. make комадны.  |"
	@echo "|  +----------------+-------------------------------------+  |"
	@echo "|  |   \"make bin\"   | для создания двоичного файла        |  |"
	@echo "|  +----------------+-------------------------------------+  |"
	@echo "|  |  \"make test\"   | для запуска двоичного файла в QEMU  |  |"
	@echo "|  +----------------+-------------------------------------+  |"
	@echo "|  |  \"make image\"  | для создания файла диска            |  |"
	@echo "|  +----------------+-------------------------------------+  |"
	@echo "|                                                            |"
	@echo "\\============================================================/"

bin:
	@echo "/============================================================\\"
	@echo "|    Сборка операционной системы \"Синтетическая жизнь\"       |"
	@mkdir -p $(BUILD_PATH)
	@gcc -m32 -g -c $(FILES_C) -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-exceptions -fno-leading-underscore -fno-pic -W -Wall -Wunknown-pragmas
	@nasm -f elf32 $(FILES_NASM)
	@mv *.o $(BUILD_PATH)/
	@ld -melf_i386 -Tlink.ld -o $(BUILD_PATH)/$(OUTPUT_FILE) $(BUILD_PATH)/*.o --build-id=none
	@rm -f $(BUILD_PATH)/*.o
	@echo "|    Сборка завершена                                        |"
	@echo "\\============================================================/"

image:
	@echo "/============================================================\\"
	@echo "|                   Создание диска                           |"
	@dd if=/dev/zero of=./$(HDDIMG) bs=512 count=16065 1>/dev/null 2>&1
	@echo "|  Создание загрузочного раздела FAT32...                    |"
	@losetup /dev/loop1 ./$(HDDIMG)
	@(echo c; echo u; echo n; echo p; echo 1; echo ;  echo ; echo a; echo 1; echo t; echo c; echo w;) | fdisk /dev/loop1 1>/dev/null 2>&1 || true
	@echo "|  Монтирование раздела /dev/loop2...                        |"
	@losetup /dev/loop2 ./$(HDDIMG) --offset 32256 --sizelimit 8224768
	@losetup -d /dev/loop1
	@echo "|  Форматирование раздела...                                 |"
	@mkdosfs /dev/loop2
	@echo "|  Копирование ядра и файлов GRUB в раздел...                |"
	@mkdir -p tempdir
	@mount /dev/loop2 tempdir
	@mkdir tempdir/boot
	@cp -r grub tempdir/boot/
	@cp $(BUILD_PATH)/$(OUTPUT_FILE) tempdir/
	@sleep 1
	@umount /dev/loop2
	@rm -r tempdir
	@losetup -d /dev/loop2
	@echo "|  Установка GRUB...                                         |"
	@echo "device (hd0) $(HDDIMG) \n \
	       root (hd0,0)         \n \
	       setup (hd0)          \n \
	       quit\n" | grub --batch 1>/dev/null
	@echo "|  Создание диска завершено                                  |"
	@echo "\\============================================================/"

test:
	@echo "/============================================================\\"
	@echo "|  Тестовый запуск операционной системы \"Синтетическая жизнь\"|"
	@qemu-system-i386 -M pc-i440fx-2.8 -kernel $(BUILD_PATH)/$(OUTPUT_FILE)
	@echo "|  Тестовый запуск завершён                                  |"
	@echo "\\============================================================/"
