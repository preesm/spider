#uses "xillib.tcl"

proc local_runtime_drc {lib_handle} {
	puts "Local Runtime DRC ..."
}

proc open_include_file1 {file_name} {
	set filename [file join "../../include/" $file_name]
	if {[file exists $filename]} {
		set config_inc [open $filename a]
	} else {
		set config_inc [open $filename a]
		xprint_generated_header $config_inc "Local Runtime Parameters"
	}
	return $config_inc
}

proc open_include_file2 {file_name} {
	set filename [file join "./src/" $file_name]
#	set conffile [file join "../../include/" $file_name]
#	file delete $conffile
#  	set new_conffile [open $conffile w]
	if {[file exists $filename]} {
		set config_inc [open $filename a]
	} else {
		set config_inc [open $filename a]
		xprint_generated_header $config_inc "Local Runtime Parameters"
	}
	return $config_inc
}

proc fill_cfg_file {conffile lib_handle} {
	puts $conffile "#ifndef OS_CFG_H"
	puts $conffile "#define OS_CFG_H"
	set value [xget_value $lib_handle "PARAMETER" "OS_N_SYS_TASKS"]
	puts $conffile "#define OS_N_SYS_TASKS $value"
	set value [xget_value $lib_handle "PARAMETER" "OS_DEFAULT_STACK_SIZE"]
	puts $conffile "#define OS_DEFAULT_STACK_SIZE $value"
	set value [xget_value $lib_handle "PARAMETER" "OS_LOWEST_PRIO"]
	puts $conffile "#define OS_LOWEST_PRIO $value"
	set value [xget_value $lib_handle "PARAMETER" "OS_MAX_TASKS"]
	puts $conffile "#define OS_MAX_TASKS $value"
	set value [xget_value $lib_handle "PARAMETER" "NB_LOCAL_FUNCTIONS"]
	puts $conffile "#define NB_LOCAL_FUNCTIONS $value"
	set value [xget_value $lib_handle "PARAMETER" "OS_DEBUG_EN"]
	puts $conffile "#define OS_DEBUG_EN $value"
	set value [xget_value $lib_handle "PARAMETER" "CONTROL_COMM"]
	puts $conffile "#define CONTROL_COMM $value"
	set value [xget_value $lib_handle "PARAMETER" "OS_NB_IN_FIFO"]
	puts $conffile "#define OS_NB_IN_FIFO $value"
	set value [xget_value $lib_handle "PARAMETER" "OS_NB_OUT_FIFO"]
	puts $conffile "#define OS_NB_OUT_FIFO $value"
	set value [xget_value $lib_handle "PARAMETER" "OS_NB_FIFO"]
	puts $conffile "#define OS_NB_FIFO $value"
	puts $conffile "#endif"
}

proc generate {lib_handle} {
	puts "Local Runtime generate ..."
#	set conffile [open_include_file1 "lrt_cfg.h"]
#	fill_cfg_file $conffile $lib_handle
	
	set conffile [open_include_file2 "lrt_cfg.h"]
	fill_cfg_file $conffile $lib_handle
	close $conffile
}
