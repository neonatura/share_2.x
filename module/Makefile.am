
udevdir = @sysconfdir@/udev/rules.d
initdir = @sysconfdir@/init.d

if KERNEL_2_6_32
if KMOD_DIR
all-local: $(abs_top_builddir)/module/2.6.32/shnet/net-pf-2-proto-145.ko

install-exec-hook: $(abs_top_builddir)/module/2.6.32/shnet/net-pf-2-proto-145.ko
	test -z "$(DESTDIR)$(KERNEL_MOD)" ||\
			$(MKDIR_P) "$(DESTDIR)$(KERNEL_MOD)"
	$(INSTALL) $(abs_top_builddir)/module/2.6.32/shnet/net-pf-2-proto-145.ko $(DESTDIR)$(KERNEL_MOD)/net-pf-2-proto-145.ko
	depmod

$(abs_top_builddir)/module/2.6.32/shnet/net-pf-2-proto-145.ko:
	$(MAKE) -C $(KERNEL_SRC) M=$(abs_top_builddir)/module/2.6.32/shnet/

clean-local:
	$(MAKE) -C $(KERNEL_SRC) M=$(abs_top_builddir)/module/2.6.32/shnet/ clean
endif
endif

