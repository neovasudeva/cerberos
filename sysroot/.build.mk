LIMINE=deps/limine/limine-install
SYSROOT=out/sysroot
SYSROOT_ISO=out/sysroot.iso

$(LIMINE):
	make -C deps/limine

$(SYSROOT): sysroot/limine.cfg $(KBIN)
	$(MKPDIR)
	mkdir $(SYSROOT)
	cp sysroot/limine.cfg           		$(SYSROOT)/limine.cfg
	cp $(KBIN)                        		$(SYSROOT)/kernel.elf
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