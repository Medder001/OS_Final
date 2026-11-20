CC := riscv64-unknown-elf-gcc
AS := riscv64-unknown-elf-as
LD := riscv64-unknown-elf-ld
QEMU := qemu-system-riscv32

FILES_H := kernel.h O5.h sbi.h strings.h io.h
FILES_C := kernel.c O5.c sbi.c strings.c io.c shell.c
FILES_O := boot.o kernel.o O5.o sbi.o strings.o io.o shell.o
X := O5

C_STD := c99
ARCH := rv32g
ABI := ilp32

LINK_ARGS := -nostdlib

all: link

compile: $(FILES_H) $(FILES_C)
	$(CC) -std=$(C_STD) -march=$(ARCH) -mabi=$(ABI) -c -o kernel.o kernel.c
	$(CC) -std=$(C_STD) -march=$(ARCH) -mabi=$(ABI) -c -o O5.o O5.c
	$(CC) -std=$(C_STD) -march=$(ARCH) -mabi=$(ABI) -c -o sbi.o sbi.c
	$(CC) -std=$(C_STD) -march=$(ARCH) -mabi=$(ABI) -c -o strings.o strings.c
	$(CC) -std=$(C_STD) -march=$(ARCH) -mabi=$(ABI) -c -o io.o io.c
	$(CC) -std=$(C_STD) -march=$(ARCH) -mabi=$(ABI) -c -o shell.o shell.c

assemble: boot.s
	$(AS) -march=$(ARCH) -mabi=ilp32 -o boot.o boot.s

link: compile assemble
	$(LD) $(LINK_ARGS) -T kernel.ld -m elf32lriscv ${FILES_O} -o $(X)

run: link
	$(QEMU) -machine virt -nographic -kernel $(X) -bios fw_dynamic32.bin

clean:
	rm -f $(X)
	rm -f $(FILES_O)

opensbi:
	git clone https://github.com/riscv-software-src/opensbi.git
	cd opensbi; \
	  make CROSS_COMPILE=riscv64-linux-gnu- ARCH=riscv PLATFORM_RISCV_XLEN=32 PLATFORM=generic; \
	  cp build/platform/generic/firmware/fw_dynamic.bin ../fw_dynamic32.bin;

opensbi-clean:
	rm -rf opensbi
	rm -f fw_*.bin
