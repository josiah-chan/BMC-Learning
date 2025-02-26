SUMMARY = "Scutool application"
DESCRIPTION = "Scutool application for OpenBMC"
LICENSE = "CLOSED"

SRC_URI = "file://main.cpp \
           file://DataBaseAPI.h \
           file://Sqlite3Database.h \
           file://Sqlite3Database.cpp \
           file://scutool.db"

S = "${WORKDIR}"

# 在do_compile中编译所有源文件为 scutool 可执行文件
do_compile() {
    # 编译 main.cpp 和 Sqlite3Database.cpp，链接 sqlite3 库
    ${CXX} ${CXXFLAGS} ${LDFLAGS} ${S}/main.cpp ${S}/Sqlite3Database.cpp -o ${S}/scutool -lsqlite3
}

# 安装可执行文件和数据库文件到目标目录
do_install() {
    # 安装可执行文件
    install -d ${D}${bindir}
    install -m 0777 ${S}/scutool ${D}${bindir}/scutool

    # 安装数据库文件到 /etc/scutool 目录
    install -d ${D}${sysconfdir}/scutool
    install -m 0644 ${S}/scutool.db ${D}${sysconfdir}/scutool/scutool.db
}

FILES_${PN} = "${bindir} ${sysconfdir}/scutool/scutool.db"

DEPENDS = "sqlite3"