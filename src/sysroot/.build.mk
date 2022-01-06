LIMINE=deps/limine/limine-install
SYSROOT=build/sysroot
SYSROOT_ISO=build/sysroot.iso

$(LIMINE):
	make -C deps/limine

$(SYSROOT): src/sysroot/limine.cfg $(KERNEL_BIN)
	$(MKPDIR)
	mkdir $(SYSROOT)
	cp src/sysroot/limine.cfg           	$(SYSROOT)/limine.cfg
	cp $(KERNEL_BIN)                        $(SYSROOT)/kernel.elf
	cp deps/limine/limine.sys               $(SYSROOT)/limine.sys
	cp deps/limine/limine-cd.bin            $(SYSROOT)/limine-cd.bin
	cp deps/limine/limine-eltorito-efi.bin  $(SYSROOT)/limine-eltorito-efi.bin

$(SYSROOT_ISO): $(SYSROOT) $(LIMINE)
	$(MKPDIR)

	xorriso \
		-as mkisofs \
		-b limine-cd.bin \
		-no-emul-boot \
		-boot-load-size 4 \
		-boot-info-table \
		--efi-boot limine-eltorito-efi.bin \
		-efi-boot-part \
		--efi-boot-image \
		--protective-msdos-label \
		$(SYSROOT) \
		-o $(SYSROOT_ISO)

	$(LIMINE) $(SYSROOT_ISO)