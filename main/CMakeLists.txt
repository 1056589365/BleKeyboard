set(SIDIR "." "power_button" "power_voltage_sample" "uart_input" "state_led" "keyboard" "utils" "project")

set(COMPONENT_SRCDIRS ${SIDIR})
set(COMPONENT_ADD_INCLUDEDIRS ${SIDIR} "." "project" "utils")

register_component()