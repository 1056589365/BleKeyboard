set(SIDIR "." "power_button" "qadc" "io_extension" "uart_input" "state_led" "keyboard")

set(COMPONENT_SRCDIRS ${SIDIR})
set(COMPONENT_ADD_INCLUDEDIRS ${SIDIR} "_include")

register_component()