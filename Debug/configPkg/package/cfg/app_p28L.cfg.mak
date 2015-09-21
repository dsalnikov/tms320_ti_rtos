# invoke SourceDir generated makefile for app.p28L
app.p28L: .libraries,app.p28L
.libraries,app.p28L: package/cfg/app_p28L.xdl
	$(MAKE) -f C:\DOCUME~1\salnikov\workspace_v6_1\tms320_ti_rtos/src/makefile.libs

clean::
	$(MAKE) -f C:\DOCUME~1\salnikov\workspace_v6_1\tms320_ti_rtos/src/makefile.libs clean

