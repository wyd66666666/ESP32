set(SOURCES "main.c"
            "LED/LED.c"
            "BUTTON/BUTTON.c"
            )


set(INCLUDE_DIRS "." "LED" "BUTTON")

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS ${INCLUDE_DIRS})