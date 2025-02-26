SUMMARY = "System Resource Monitor"
DESCRIPTION = "My Function - Monitor CPU and Memory usage"
LICENSE = "CLOSED"

SRC_URI = "file://system-monitor.cpp"

S = "${WORKDIR}"

do_compile() {
    ${CXX} ${CXXFLAGS} ${LDFLAGS} ${WORKDIR}/system-monitor.cpp -o systemtool
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 systemtool ${D}${bindir}
}

RDEPENDS_${PN} += "libstdc++"
