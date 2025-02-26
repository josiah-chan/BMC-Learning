SUMMARY = "Devmemtool Command"
DESCRIPTION = "Memory Access Tool like devmem"
LICENSE = "CLOSED"

SRC_URI = "file://devmemtool.c \
           file://devmemtool.h"

S = "${WORKDIR}"

do_compile () {
    ${CC} ${CFLAGS} ${LDFLAGS} -I${S} ${S}/devmemtool.c -o ${B}/devmemtool
}

do_install () {
    install -d ${D}${bindir}
    install -m 0755 ${B}/devmemtool ${D}${bindir}/devmemtool
}