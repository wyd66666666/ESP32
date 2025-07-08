set(SOURCES "main.c"
            "LED/LED.c"
            "BUTTON/BUTTON.c"
            "DISPLAY/SPI.c"
            )


set(INCLUDE_DIRS "." 
                 "LED"
                 "BUTTON"
                 "DISPLAY"
                 )


idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS ${INCLUDE_DIRS})

